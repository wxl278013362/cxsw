#include "GcodeM402Handle.h"
#include "CrModel/CrMotion.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include "CrInc/CrMsgType.h"
#include <stdio.h>
#include <stdlib.h>

#define CMD_M402 ("M402")

#if HAS_BED_PROBE

static BOOL ProbeStow(VOID)
{
    return (SetDeployed(false));   //收起探头，调用M401指令函数
}

static VOID MoveZafterProbing(VOID)
{
#ifdef Z_AFTER_PROBING
    struct MotionGcode_t Target = {0};
    struct MotionCoord_t CurCoord = MotionModel->GetLatestCoord();
    struct FeedrateAttr_t *FrAttr;
    StepperGetFeedrateAttr(FrAttr);
    Target.Coord.CoorX = CurCoord.CoorX;
    Target.Coord.CoorY = CurCoord.CoorY;
    Target.Coord.CoorZ = Z_AFTER_PROBING;
    Target.Coord.CoorE = CurCoord.CoorE;
    Target.ProbeRaise == PROBE_PT_STOW;
    Target.FeedRate = FrAttr.Feedrate;

    MotionModel->PutMotionCoord(Target);
#endif
}

S32 GcodeM402Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    if ( strncmp(CmdBuff, CMD_M402, strlen(CMD_M402)) != 0 )
    {
        return CrErr;
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM402Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    if ( !ProbeStow() )
    {
        return CrErr;
    }

    MoveZafterProbing();
    ReportCurrentPosition();

    return GCODE_EXEC_OK;
}

S32 GcodeM402Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

#endif
