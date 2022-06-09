#include "GcodeM23Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrModel/CrPrinter.h"
#include "../../../../CrModel/CrSdCard.h"
#include "../../../../Common/Include/CrDirentAndFile.h"

#define CMD_M23  ("M23")

static S8 FileName[FILE_NAME_LENGTH] = {0};
static S8 FullName[MAX_PATH_LENGTH] = {0};

/* 保存文件名和完整路径 */
S32 SavePrintFile(S8 *FullFileName)
{
    /* 查看文件是否存在 */
    if ( (FullFileName == NULL) || (CrOpen(FullFileName, O_RDONLY, 0x755) < 0) )
    {
        return 0;
    }
    S8 *Index = strstr(FullFileName, "/");
    S8 *Index2 = strstr(FullFileName, "\\");
    S32 FileNameSize = sizeof(FileName),
        FullNameSize = sizeof(FullName),
        FullFileNameLen = strlen(FullFileName);

    memset(FileName, '\0', FileNameSize);
    memset(FullName, '\0', FullNameSize);
    strncpy(FullName, FullFileName, FullFileNameLen);


    /* 保存文件名和完整路径 */
    if ( (Index == NULL) && (Index2 == NULL) )
    {
        if ( FullFileNameLen < FileNameSize - 1 )
        {
            strncpy(FileName, FullFileName, FullFileNameLen);
        }
        else
        {
            rt_kprintf("File name is too long to save\n");
            return 0;
        }
    }
    else
    {
        Index = Index2;
        while ( Index != NULL )
        {
            Index = strstr(Index + 1, "/");
            if ( Index != NULL )
            {
                Index2 = Index;
            }
        }
        if ( strlen(Index2 + 1) < FileNameSize - 1 )
        {
            strncpy(FileName, Index2 + 1, strlen(Index2 + 2));
        }
        else
        {
            rt_kprintf("File name is too long to save\n");
            return 0;
        }
    }
    return 1;
}

S32 GcodeM23Parse(const S8 *CmdBuff, VOID *Arg)
{
    /* 传的是全路径 */
    struct ArgM23_t *CmdArg = (struct ArgM23_t *)Arg;
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( (CmdBuff == NULL) || (Arg == NULL) )
    {
        return 0;
    }

    /* 获取指令失败，获取到的命令不是M23 */
    if ( (GetGcodeCmd(CmdBuff, Cmd) == 0) || (strcmp(Cmd, CMD_M23) != 0) )
    {
        return 0;
    }

    /* 清除CmdArg->FileName数组内容 */
    memset(CmdArg->FileName, '\0', sizeof(CmdArg->FileName));
    /* 将指令后接的参数保存到数组内 */
    strncpy(CmdArg->FileName, CmdBuff + strlen(CMD_M23) + sizeof(S8), MAX_STRING_LENGTH - 1);

    return 1;
}

S32 GcodeM23Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    struct ArgM23_t *CmdArgM23 = (struct ArgM23_t *)ArgPtr;
    if ( !ArgPtr || !ReplyResult || !SdCardModel || !Printer )
        return 0;

    if ( (!SdCardModel->GetDevStatus()) || (strlen(CmdArgM23->FileName) <= 0) )
    {
        rt_kprintf("Open file error\n");
    }

    /* 将该文件保存到SdCardModel中 */
    if ( SavePrintFile(CmdArgM23->FileName) == 0 )
    {
        rt_kprintf("Fresh file failed %s\n", CmdArgM23->FileName);
        return 0;
    }
    rt_kprintf("Now fresh file: %s\n", CmdArgM23->FileName);

    return 1;
}

S32 GcodeM23Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
