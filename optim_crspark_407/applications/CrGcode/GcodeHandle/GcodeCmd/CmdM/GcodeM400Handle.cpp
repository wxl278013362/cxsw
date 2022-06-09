#include "GcodeM400Handle.h"
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrSleep.h"

#define CMD_M400           "M400"

S32 GcodeM400Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0] != GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M400) != 0 )
        return 0;


    return 1;
}


S32 GcodeM400Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel)
        return 0;

    while ( !(MotionModel->IsMotionMoveFinished()) )
    {
        CrM_Sleep(5);
    }

    return 1;
}

S32 GcodeM400Reply(VOID *ReplyResult, S8 *Buff)
{

    return 1;
}
