#include "GcodeM25Handle.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrPrinter.h"

#define CMD_M25 "M25"

S32 GcodeM25Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M25) != 0 )  //传进来的命令不是M25命令
        return 0;

    return 1;
}

S32 GcodeM25Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    MotionModel->MotionPause();
    Printer->PauseHandle();

    return 1;
}

S32 GcodeM25Reply(VOID *ReplyResult, S8 *Buff)
{

    return 0;
}
