#include "GcodeG38Handle.h"
#include "CrModel/CrMotion.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include "CrInc/CrMsgType.h"
#include "CrMotion/CrEndStop.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef G38_PROBE_TARGET

#define _BV(n) (1<<(n))
#define TEST(n,b) (!!((n)&_BV(b)))
#define ABS(n)                   ((n) > 0 ? (n) : -(n))     //绝对值

#define CMD_G38                 ("G38")
#define SUB_CODE_SEP            ('.')
#define MMM_TO_MMS(MM_M)        (MM_M / 60.0f)
#define G38_MINIMUM_MOVE        0.0275  // (mm) Minimum distance that will produce a move.
#define HOMING_FEEDRATE_MM_M    { (50*60), (50*60), (4*60) }   // Homing speeds (mm/min)
#define HOMING_BUMP_MM          { 5, 5, 2 }       // (mm) Backoff from endstops after first bump

//当前坐标缓存，源文件：StepperApi.cpp
extern struct Coordinate3d_t CurCoordination;
//目标坐标缓存
extern struct Coordinate3d_t Target;

static BOOL G38DidTrigger;
FLOAT FeedrateMmS = MMM_TO_MMS(1500);   //默认值
static FLOAT SavedFeedrateMmS;
static U16 SavedFeedratePercentage;
static U16 FeedratePercentage = 100;
static BOOL CancelableSkipping = false;
static BOOL RecoveryEnabled = true;
static U8 G38Move;
struct XYZFloat_t HomingFeedrateMm_M = HOMING_FEEDRATE_MM_M;

BOOL IsSdPrintg()
{
    return true;
}

/**
 * Save the current machine state to the power-loss recovery file
 */
VOID RecoverySave()
{
    //TODO...
}

static BOOL AxisIsRelative(const Axis_t Axis)
{
    return MotionModel->IsRelativeMotionMode();
}

static VOID RememberFeedrateScaling()
{
    SavedFeedrateMmS = FeedrateMmS;
    SavedFeedratePercentage = FeedratePercentage;
}

VOID RememberFeedrateScaling_Off()
{
    RememberFeedrateScaling();
    FeedratePercentage = 100;
}

VOID RestoreFeedrateAndScaling()
{
    FeedrateMmS = SavedFeedrateMmS;
    FeedratePercentage = SavedFeedratePercentage;
}

static FLOAT HomingFeedrate(const enum Axis_t a)
{
    FLOAT v;
    switch (a)
    {
        case X:
            v = HomingFeedrateMm_M.X;
            break;
        case Y:
            v = HomingFeedrateMm_M.Y;
            break;
        case Z:
            default:
            v = HomingFeedrateMm_M.Z;
            break;
    }

    return MMM_TO_MMS(v);
}

static FLOAT  HomeBumpMm(const Axis_t Axis)
{
    struct XYZFloat_t DefsProgmem = HOMING_BUMP_MM;
    float v = 0;
    switch (Axis)
    {
        case X:
            v = DefsProgmem.X;
            break;

        case Y:
            v = DefsProgmem.Y;
            break;

        case Z:
            v = DefsProgmem.Z;
            break;

        default:
            break;
    }

    return v;
}

VOID EndstopsNotHoming()
{
    // Disable / Enable endstops based on ENSTOPS_ONLY_FOR_HOMING and global enable
}

static VOID G38SingleProbe(const U8 MoveValue)
{
    CrEndStopOpen(ENDSTOPX_PIN_NAME, NULL, NULL);   //打开限位开关
    CrEndStopOpen(ENDSTOPY_PIN_NAME, NULL, NULL);
    CrEndStopOpen(ENDSTOPZ_PIN_NAME, NULL, NULL);
    G38Move = MoveValue;

    PlanLine(LineBuff, &CurCoordination, &Target);  //目的坐标放入运动规划
    while ( IsPlanLineBuffEmpty(LineBuff) )  // wait until the machine is idle
    {
        break;
    }

    G38Move = 0;
    EndStopHandler();
    struct MotionPositionMsg_t CurPos = MotionModel->GetCurPosition();
    CurCoordination.X = CurPos.PosX;
    CurCoordination.Y = CurPos.PosY;
    CurCoordination.Z = CurPos.PosZ;
    CurCoordination.E = CurPos.PosE;
    struct MotionGcode_t Pos = {0};
    memcpy(&Pos.Coord, &CurCoordination, sizeof(struct Coordinate3d_t));

    MotionModel->PutMotionCoord(Pos);
}

BOOL G38RunProbe(const U8 SubCode)
{
    BOOL G38PassFail = false;

#if ( MULTIPLE_PROBING > 1 )
    struct XYZFloat_t Retract_mm;

    FLOAT Dist;
    Dist = Target.X - CurCoordination.X;
    Retract_mm.X = ABS(Dist) < G38_MINIMUM_MOVE ? 0 : HomeBumpMm((Axis_t)X) * (Dist > 0 ? -1 : 1);

    Dist = Target.Y- CurCoordination.Y;
    Retract_mm.Y = ABS(Dist) < G38_MINIMUM_MOVE ? 0 : HomeBumpMm((Axis_t)Y) * (Dist > 0 ? -1 : 1);

    Dist = Target.Z - CurCoordination.Z;
    Retract_mm.Z = ABS(Dist) < G38_MINIMUM_MOVE ? 0 : HomeBumpMm((Axis_t)Z) * (Dist > 0 ? -1 : 1);
#endif

    while ( IsPlanLineBuffEmpty(LineBuff) )  // wait until the machine is idle
    {
        break;
    }
#ifdef G38_PROBE_AWAY
    const U8 MoveValue = SubCode;
#else
    const U8 MoveValue = 1;
#endif

    G38DidTrigger = false;

    // Move until destination reached or target hit
    G38SingleProbe(MoveValue);

    if ( G38DidTrigger )
    {
        G38PassFail = true;
    #if MULTIPLE_PROBING > 1
        // Move away by the retract distance
        Target.X = CurCoordination.X + Retract_mm.X;
        Target.Y = CurCoordination.Y + Retract_mm.Y;
        Target.Z = CurCoordination.Z + Retract_mm.Z;

        EndStopHandler();
        PlanLine(LineBuff, &CurCoordination, &Target);  //目的坐标放入运动规划
        while ( IsPlanLineBuffEmpty(LineBuff) )  // wait until the machine is idle
        {
            break;
        }

        // Bump the target more slowly
        Target.X -= Retract_mm.X * 2;
        Target.Y -= Retract_mm.Y * 2;
        Target.Z -= Retract_mm.Z * 2;

        G38SingleProbe(MoveValue);
    #endif
    }

    EndstopsNotHoming();

    return G38PassFail;
}

S32 GcodeG38Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_G38, strlen(CMD_G38)) != 0 )
    {
        return CrErr;
    }

    struct ArgG38_t *CmdArg = (struct ArgG38_t *)Arg;
    S32 Offset = strlen(CMD_G38) + 1; //加1是越过分隔符
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode G38 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_F:
                CmdArg->HasF = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueF = false;
                }
                else
                {
                    CmdArg->HasValueF = true;
                    CmdArg->F = atof(Opt + 1);
                }
                break;

            case OPT_X:
                CmdArg->HasX = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueX = false;
                }
                else
                {
                    CmdArg->HasValueX = true;
                    CmdArg->X = atof(Opt + 1);
                }
                break;

            case OPT_Y:
                CmdArg->HasY = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueY = false;
                }
                else
                {
                    CmdArg->HasValueY = true;
                    CmdArg->Y = atof(Opt + 1);
                }
                break;

            case OPT_Z:
                CmdArg->HasZ = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueZ = false;
                }
                else
                {
                    CmdArg->HasValueZ = true;
                    CmdArg->Z = atof(Opt + 1);
                }
                break;

            case OPT_E:
                CmdArg->HasZ = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueE = false;
                }
                else
                {
                    CmdArg->HasValueE = true;
                    CmdArg->E = atof(Opt + 1);
                }
                break;

            case SUB_CODE_SEP:
                CmdArg->HasSubCode = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueSubCode = false;
                }
                else
                {
                    CmdArg->HasValueSubCode = true;
                    CmdArg->SubCode = atoi(Opt + 1);
                }
                break;

            default:
                printf("Gcode G38 has no define opt = %s\n", CmdBuff);
                break;
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}

S32 GcodeG38Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    struct ArgG38_t *Param = (struct ArgG38_t *)ArgPtr;
#ifdef G38_PROBE_AWAY
    U8 MaxSubCode = 5;
#else
    U8 MaxSubCode = 3;
#endif
    if ( !Param->HasSubCode || (Param->SubCode < 2 || Param->SubCode > MaxSubCode) )
    {
        return CrErr;
    }
#ifdef CANCEL_OBJECTS
    const BOOL SkipMove = CancelableSkipping;
#else
    const BOOL SkipMove = false;
#endif
    FLOAT Val;
    if ( Param->HasX )
    {
        if ( Param->HasValueX )
        {
            Val = Param->X;
        }
        else
        {
            Val = 0;
        }

        if ( SkipMove )
        {
            Target.X = CurCoordination.X;
        }
        else
        {
            Target.X = AxisIsRelative(Axis_t(X)) ? CurCoordination.X + Val : (Val - X);
        }
    }
    else
    {
        Target.X = CurCoordination.X;
    }

    if ( Param->HasY )
    {
        if ( Param->HasValueY )
        {
            Val = Param->Y;
        }
        else
        {
            Val = 0;
        }

        if ( SkipMove )
        {
            Target.Y = CurCoordination.Y;
        }
        else
        {
            Target.Y = AxisIsRelative(Axis_t(Y)) ? CurCoordination.Y + Val : (Val - Y);
        }
    }
    else
    {
        Target.Y = CurCoordination.Y;
    }

    if ( Param->HasZ )
    {
        if ( Param->HasValueZ )
        {
            Val = Param->Z;
        }
        else
        {
            Val = 0;
        }

        if ( SkipMove )
        {
            Target.Z = CurCoordination.Z;
        }
        else
        {
            Target.Z = AxisIsRelative(Axis_t(Z)) ? CurCoordination.Z + Val : (Val - Z);
        }
    }
    else
    {
        Target.Z = CurCoordination.Z;
    }

    if ( Param->HasE )
    {
        FLOAT v;
        if ( Param->HasValueE )
        {
            v = Param->E;
        }
        else
        {
            v = 0;
        }
        Target.E = AxisIsRelative(Axis_t(E)) ? CurCoordination.E + v : v;
    }
    else
    {
        Target.E = CurCoordination.E;
    }

#if ( defined POWER_LOSS_RECOVERY &&  !PIN_EXISTS(POWER_LOSS) )
    // Only update power loss recovery on moves with E
    if ( RecoveryEnabled && IsSdPrintg() && Param->HasE && (Param->HasX || Param->HasY) )
    {
      RecoverySave();
    }
#endif

    if ( Param->HasF && Param->HasValueF )
    {
        FeedrateMmS = MMM_TO_MMS(Param->F);
    }

    RememberFeedrateScaling_Off();

      const BOOL ErrorOnFail =
    #ifdef G38_PROBE_AWAY
      !TEST(Param->SubCode, 0)
    #else
      (Param->SubCode == 2)
    #endif
    ;

    if ( ABS(Target.X - CurCoordination.X) >= G38_MINIMUM_MOVE )
    {
        if ( !(Param->HasF && Param->HasValueF) )
        {
            FeedrateMmS = HomingFeedrate((Axis_t)X);
        }
        if ( !G38RunProbe(Param->SubCode) && ErrorOnFail )
        {
            printf("Failed to reach target!!!\n");
            goto G38_FAILED;
        }
    }

    if ( ABS(Target.Y - CurCoordination.Y) >= G38_MINIMUM_MOVE )
    {
       if ( !(Param->HasF && Param->HasValueF) )
        {
            FeedrateMmS = HomingFeedrate((Axis_t)Y);
        }
        if ( !G38RunProbe(Param->SubCode) && ErrorOnFail )
        {
            printf("Failed to reach target!!!\n");
            goto G38_FAILED;
        }
    }

    if ( ABS(Target.Z - CurCoordination.Z) >= G38_MINIMUM_MOVE )
    {
        if ( !(Param->HasF && Param->HasValueF) )
        {
            FeedrateMmS = HomingFeedrate((Axis_t)Z);
        }
        if ( !G38RunProbe(Param->SubCode) && ErrorOnFail )
        {
            printf("Failed to reach target!!!\n");
            goto G38_FAILED;
        }
    }

G38_FAILED:
    RestoreFeedrateAndScaling();

    return GCODE_EXEC_OK;
}

S32 GcodeG38Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

#endif
