#include "GcodeM524Handle.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrPrinter.h"
#include "../../GcodeHandle.h"

#define CMD_M524 ("M524")

#if 0
VOID GcodeM524Init(VOID *Print)
{
    Printer = (CrPrinter *)Print;
    return ;
}
#endif

S32 GcodeM524Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    if ( strncmp(CmdBuff, CMD_M524, strlen(CMD_M524)) != 0)
        return 0;

    return GCODE_EXEC_OK;
}

S32 GcodeM524Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel || !Printer )
        return 0;

    rt_kprintf("-----------------%s-----------\n", __FUNCTION__);

    MotionModel->MotionStop();
    Printer->StopHandle();

    return GCODE_EXEC_OK;
}

S32 GcodeM524Reply(VOID *ReplyResult, S8 *Buff)
{

    return 0;
}
