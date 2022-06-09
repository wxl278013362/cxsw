#include "GcodeM29Handle.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrSdCard.h"
#include "CrModel/CrPrinter.h"
#include "../../../../Common/Include/CrDirentAndFile.h"

#define CMD_M29  ("M29")

static S32 FileFd = -1;

S32 GcodeM29Parse(const S8 *CmdBuff, VOID *Arg)
{
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( Arg == NULL )
    {
        return 0;
    }

    /* 获取命令失败，传进来的不是M29命令 */
    if ( (GetGcodeCmd(CmdBuff, Cmd) == 0) || (strcmp(Cmd, CMD_M29) != 0) )
    {
        return 0;
    }

    return 1;
}

S32 GcodeM29Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( (ArgPtr == NULL) || (ReplyResult == NULL) || (Printer == NULL) )
    {
        return 0;
    }

    if ( FileFd >= 0 )
    {
        CrClose(FileFd);
    }

    return 1;
}

S32 GcodeM29Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
