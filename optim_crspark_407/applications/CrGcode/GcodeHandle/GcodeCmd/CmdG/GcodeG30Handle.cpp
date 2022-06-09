#include "GcodeG30Handle.h"

#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "../../GcodeHandle.h"
#include "CrModel/CrMotion.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CMD_G30 "G30"
#define Z_AFTER_PROBING 5

struct MotionCoord_t ProbeOffset = {
    .CoorX = 0.0f,
    .CoorY = 0.0f,
    .CoorZ = 0.0f,
    .CoorE = 0.0f
};

S32 CheckCoordReachable(struct MotionGcode_t *Coord)
{
    /* 检查坐标点 (x,y) 是否可到达 */
    return 1;
}

static S16 FeedratePct = 100;
static FLOAT OldFeedrate;
static S16 OldFeedratePct;
/* 保存之前的速率 */
void SavePreViousFeedRate(void)
{
    struct FeedrateAttr_t FeedRate = {0};
    StepperGetFeedrateAttr(&FeedRate);
    OldFeedrate = FeedRate.Feedrate;
    OldFeedratePct = FeedratePct;
    FeedratePct = 100;
}
/* 恢复速率 */
void RestoreFeedRate(void)
{
    struct FeedrateAttr_t FeedRate = {0};
    StepperGetFeedrateAttr(&FeedRate);
    FeedratePct = OldFeedratePct;
    FeedRate.Feedrate = OldFeedrate;
    StepperSetFeedrateAttr(&FeedRate);
}

/* Probe探针存放，执行，放回probe探测值 */
FLOAT PutProbeCoord(struct MotionGcode_t *ProbeCoord)
{
    /* 将探针坐标转换为运动坐标 */
    /* 将运动坐标入队（包含探针动作） */
    /* 等待probe探测完成 */
    /* 获取z轴的值 */
    return 1;
}

S32 GcodeG30Parse(const S8 *CmdBuff, VOID *Arg)
{
    /* G30 的操作有 E X Y */
    struct ArgG30_t *CmdArg = (struct ArgG30_t *)Arg;
    S8 Cmd[CMD_BUFF_LEN] = {0};
    S32 Offset, CmdBuffLen, OptLen;
    S8 Opt[20] = {0};
    S32 OptVal;
    if ( Arg == NULL )
    {
        return CrErr;
    }

    if ( (GetGcodeCmd(CmdBuff, Cmd) == 0) || (strcmp(Cmd, CMD_G30) != 0) )
    {
        return GCODE_CMD_ERROR;
    }

    Offset = strlen(CMD_G30) + sizeof(S8);
    CmdBuffLen = strlen(CmdBuff);
    while ( Offset < CmdBuffLen )
    {
        if ( GetCmdOpt(CmdBuff + Offset, Opt) == 0 )
        {
            break;
        }
        OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode G30 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_E:
            {
                CmdArg->HasE = 1;
                if ( OptLen <= 1 )
                {
                    CmdArg->E = true;
                }
                else
                {
                    OptVal = atoi(Opt + 1);
                    CmdArg->E = (BOOL)(OptVal < 0 ? 0 : (OptVal > 255 ? 255 : OptVal));
                    break;
                }
            }
            case OPT_X:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasX = 0;
                }
                else
                {
                    CmdArg->HasX = 1;
                    CmdArg->X = atof(Opt + 1);
                }
                break;
            }
            case OPT_Y:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasY = 0;
                }
                else
                {
                    CmdArg->HasY = 1;
                    CmdArg->Y = atof(Opt + 1);
                }
                break;
            }
            default:
            {
                printf("Gcode G30 has no define opt = %s\n", CmdBuff);
                break;
            }
        }
        Offset += OptLen + sizeof(S8);
    }

    return 1;
}

S32 GcodeG30Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    struct ArgG30_t *ParamG30 = (struct ArgG30_t *)ArgPtr;
    struct MotionGcode_t Target = {0};
    struct MotionCoord_t CurCoord = MotionModel->GetLatestCoord();
    FLOAT ProbeCoorZ;

    Target.Coord.CoorX = (ParamG30->HasX ? ParamG30->X : (CurCoord.CoorX + ProbeOffset.CoorX));
    Target.Coord.CoorY = (ParamG30->HasY ? ParamG30->Y : (CurCoord.CoorY + ProbeOffset.CoorY));
    Target.ProbeRaise = (ParamG30->HasE ? (ParamG30->E ? PROBE_PT_STOW : PROBE_PT_NONE) : PROBE_PT_STOW);

    if ( CheckCoordReachable(&Target) == 0 )
    {
        printf("error: The target position is unreachable. \n");
        return CrErr;
    }

    /* 保存速度 */
    SavePreViousFeedRate();
    /* 探针坐标入队 */
    ProbeCoorZ = PutProbeCoord(&Target);
    /* 等待执行完，获取z的坐标值 */
    /* 恢复速度 */
    RestoreFeedRate();
    if ( !isnan(ProbeCoorZ) )
    {
        printf("X: %f, Y: %f, Z: %f\n", Target.Coord.CoorX, Target.Coord.CoorY, ProbeCoorZ);
    }
#ifdef Z_AFTER_PROBING
    if ( Target.ProbeRaise == PROBE_PT_STOW )
    {
        Target.Coord.CoorZ = Z_AFTER_PROBING;
        (void)MotionModel->PutMotionCoord(Target);
    }
#endif /* Z_AFTER_PROBING */
    /* 等待规划完成，打印当前坐标 */
    CurCoord = MotionModel->GetLatestCoord();
    printf("X: %f, Y: %f, Z: %f, E: %f", CurCoord.CoorX, CurCoord.CoorY, CurCoord.CoorZ, CurCoord.CoorE);

    return 1;
}

S32 GcodeG30Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
