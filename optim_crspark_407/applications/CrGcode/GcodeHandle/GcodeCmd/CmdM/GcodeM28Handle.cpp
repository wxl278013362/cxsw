#include "GcodeM28Handle.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrSdCard.h"
#include "CrModel/CrPrinter.h"
#include "../../../../Common/Include/CrDirentAndFile.h"

#define CMD_M28  ("M28")

static S32 FileFd = -1;

S32 SaveFileFd(S8 *FileFullName)
{
    if ( FileFullName == NULL )
    {
        return 0;
    }
    FileFd = CrOpen(FileFullName, O_CREAT | O_APPEND | O_WRONLY | O_TRUNC, 0x777);
    if ( FileFd < 0 )
    {
        printf("Open file error %s\n", FileFullName);
        return 0;
    }
    return 1;
}

S32 GcodeM28Parse(const S8 *CmdBuff, VOID *Arg)
{
    struct ArgM28_t *CmdArg = (struct ArgM28_t *)Arg;
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( (CmdBuff == NULL) || (Arg == NULL) )
    {
        return 0;
    }

    /* 获取指令失败，或者不是M28命令 */
    if ( (GetGcodeCmd(CmdBuff, Cmd) == 0) || (strcmp(Cmd, CMD_M28) != 0) )
    {
        return 0;
    }

    /* 清除CmdArg->FileName的内容 */
    memset(CmdArg->FileName, '\0', MAX_STRING_LENGTH - 1);
    /* 将指令后接的参数保存到数组内 */
    strncpy(CmdArg->FileName, CmdBuff + strlen(CMD_M28) + sizeof(S8), MAX_STRING_LENGTH - 1);

    return 1;
}

S32 GcodeM28Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    struct ArgM28_t *CmdArgM28 = (struct ArgM28_t *)ArgPtr;
    if ( !ArgPtr || !ReplyResult || !SdCardModel || !Printer )
        return 0;
    if ( FileFd >= 0 )
    {
        CrClose(FileFd);
    }
    /* 打开文件，保存文件描述符 */
    if ( SaveFileFd(CmdArgM28->FileName) )
    {
        return 0;
    }

    return 1;
}

S32 GcodeM28Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
