#include "GcodeM410Handle.h"
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrSleep.h"

#define CMD_M410           "M410"

S32 GcodeM410Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0] != GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M410) != 0 )
        return 0;

    return 1;
}


S32 GcodeM410Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel)
        return 0;

    MotionModel->StopMotionUrgently();    //快速停止

    return 1;
}

S32 GcodeM410Reply(VOID *ReplyResult, S8 *Buff)
{

    return 1;
}
