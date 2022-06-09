#include "GcodeM22Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrSdCard.h"
#include "CrModel/CrPrinter.h"

#define CMD_M22  "M22"

S32 GcodeM22Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M22) != 0 )  //传进来的命令不是M18或M84命令
        return 0;

    return 1;
}

S32 GcodeM22Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !SdCardModel || !Printer )
        return 0;

    Printer->SetPrintStatus(STOP_PRINT);
    Printer->StopHandle();
    SdCardModel->Release();

    return 1;
}

S32 GcodeM22Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
