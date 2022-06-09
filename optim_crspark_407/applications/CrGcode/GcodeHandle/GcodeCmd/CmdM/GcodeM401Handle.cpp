#include "GcodeM401Handle.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrPrinter.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrInc/CrMsgType.h"
#include "Common/Include/CrEepromApi.h"
#include <stdio.h>
#include <stdlib.h>

#define EITHER(A, B)    (defined A || defined B)
#define DISABLED(A)     (!defined A)
#define TERN_(O,A)      (defined O ? A : NULL)
#define ENABLED(A)      (defined A)

#if HAS_BED_PROBE

#define CMD_M401 ("M401")
#define PROBE_TRIGGERED()  (true == 1)  //TOOD... 检查探针状态
#define PROBE_TARE 1

BOOL ZprobeEnabled = false;
extern struct Coordinate3d_t CurCoordination;

VOID SetZprobeEnabled(const BOOL Onoff)
{
    ZprobeEnabled = Onoff;
}

BOOL GetZprobeEnabled(VOID)
{
    return ZprobeEnabled;
}

VOID DoZraise(const FLOAT Zraise)     //Z轴抬升
{
    FLOAT Zdest = Zraise;
    if ( SettingData.HomeOffset.Z < 0 )
    {
        Zdest -= SettingData.HomeOffset.Z;
    }

    struct Coordinate3d_t TargetPosition;
    memcpy(&TargetPosition, &CurCoordination, sizeof(struct Coordinate3d_t));
    TargetPosition.Z = MIN(Zdest, Z_MAX_POS);
    PlanLine(LineBuff, &CurCoordination, &TargetPosition);
}

BOOL HomingNeededError(U8 AxisBits)
{
    //TODO...
    return true;
}

VOID ProbeSpecificAction(const BOOL Deploy)
{
    //TODO...真实的探针伸收函数
    //参考Marlin函数probe_specific_action
}

BOOL SetDeployed(const BOOL Deploy) //Depoly为true/false：伸出/收起
{
    if ( ZprobeEnabled == Deploy )
    {
        return false;
    }

#if (EITHER(FIX_MOUNTED_PROBE, NOZZLE_AS_PROBE) && DISABLED(PAUSE_BEFORE_DEPLOY_STOW))
    const BOOL ZraiseWanted = Deploy;
#else
    const BOOL ZraiseWanted = true;
#endif

    if ( ZraiseWanted )
    {
        DoZraise(MAX(Z_CLEARANCE_DEPLOY_PROBE, Z_CLEARANCE_BETWEEN_PROBES));   //如需伸出，先抬起Z轴
    }

#if EITHER(Z_PROBE_SLED, Z_PROBE_ALLEN_KEY)
    if ( HomingNeededError(TERN_(Z_PROBE_SLED, _BV(X))) )
    {
        MotionModel->MotionStop();
        return false;
    }
#endif

    struct MotionCoord_t OldPosition = MotionModel->GetLatestCoord();   //获取最新位置

#if ENABLED(PROBE_TRIGGERED_WHEN_STOWED_TEST)
    // Only deploy/stow if needed
    if ( PROBE_TRIGGERED() == Deploy )
    {
        if ( !Deploy )
        {
            SetZprobeEnabled(false); // Switch off triggered when stowed probes early
                                                     // otherwise an Allen-Key probe can't be stowed.
            ProbeSpecificAction(Deploy);
        }

    if ( PROBE_TRIGGERED() == Deploy )             // Unchanged after deploy/stow action?
    {
        if ( Printer->GetPrintStatus() == PRINTING ) {
            printf("Z-Probe failed!!!\n");
        }
        MotionModel->MotionStop();
        return true;
    }
    
#else
    ProbeSpecificAction(Deploy);
#endif

    struct MotionGcode_t Target = {0};
    /* M401 Fix me */
    Target.RetractFlag = 1;
    Target.FeedRate = 0.0f;
    Target.ProbeRaise = PROBE_PT_NONE;
    Target.Coord = OldPosition;
    MotionModel->PutMotionCoord(Target);

    SetZprobeEnabled(Deploy);

    return true;
}

static S32 ProbeDeploy(VOID)
{
    return (SetDeployed(true));   //伸出探头
}

VOID ProbeTare(VOID)
{
    //TODO...
}

VOID ReportCurrentPosition()
{
    struct MotionPositionMsg_t Pos = {0};
    if ( !MotionModel )
    {
        return ;
    }
    Pos = MotionModel->GetCurPosition();
    //报告本地坐标（加上偏移值的坐标）
    printf("X: %f Y: %f Z: %f E: %f",Pos.PosX,Pos.PosY,Pos.PosZ,Pos.PosE);//未加上偏移值

    //报告当前坐标
    printf("X: %f Y: %f Z: %f E: %f",Pos.PosX,Pos.PosY,Pos.PosZ,Pos.PosE);
}


S32 GcodeM401Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    if ( strncmp(CmdBuff, CMD_M401, strlen(CMD_M401)) != 0 )
    {
        return CrErr;
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM401Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    if ( !ProbeDeploy() )
    {
        return CrErr;
    }

#if PROBE_TARE
    ProbeTare();
#endif
    ReportCurrentPosition();

    return GCODE_EXEC_OK;
}

S32 GcodeM401Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

#endif
