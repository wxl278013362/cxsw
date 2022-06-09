#include "GcodeM20Handle.h"

#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "../../../../CrModel/CrMotion.h"
#include "../../../../CrModel/CrSdCard.h"
#include "../../../../Common/Include/CrDirentAndFile.h"
#include "../../../../CrSdCard/AppInterface/CrSdCardAppInterface.h"

#include <stdio.h>
#include <string.h>

#define CMD_M20 ("M20")

#define PRINT_MAX_FILES_NUM (20U) //定义最大打印文件数
#define SDCARD_BASE_PATH (".gcode") //SD卡的基地址

/* 打印SdCard的内容 */
static VOID PrintSdCardContent()
{
    S8 i;
    S32 FileCount;
    struct FileInfo_t FileList[PRINT_MAX_FILES_NUM] = {0};
    /* 轮询获取文件，如果是目录，则进去继续轮询，底层时打印文件路径和文件大小 */
    CrStat_t StatTemp = {0};
    FileCount = CrSdCardGetFileNameToBeShow(0, FileList, PRINT_MAX_FILES_NUM, (S8 *)SDCARD_BASE_PATH);
    for ( i = 0; i < FileCount; i++ )
    {
        printf("%s", FileList[i].Name);
        if ( FileList[i].IsDirectory )
        {
            CrStat(FileList[i].Name, &StatTemp);
            printf("\t%d\n", (S32)StatTemp.st_size);
        }
        else
        {
            printf("\t0\n");
        }
    }

}


S32 GcodeM20Parse(const S8 *CmdBuff, VOID *Arg)
{
    S8 *CmdArg = (S8 *)Arg;
    S8 Cmd[CMD_BUFF_LEN] = {0};
    S8 Opt[20] = {0};
    S32 Offset, CmdBuffLen, OptLen;
    if ( Arg == NULL )
    {
        return CrErr;
    }

    if ( (GetGcodeCmd(CmdBuff, Cmd) == 0) || (strcmp(Cmd, CMD_M20) != 0) )
    {
        return GCODE_CMD_ERROR;
    }
    Offset = strlen(CMD_M20) + sizeof(S8);
    CmdBuffLen = strlen(CmdBuff);
    while ( Offset < CmdBuffLen )
    {
        if ( GetCmdOpt(CmdBuff + Offset, Opt) == 0 )
        {
            break;
        }
        OptLen = strlen(Opt);
        switch ( Opt[0] )
        {
            case OPT_L:
            {
                *CmdArg = 1;
                break;
            }
            default:
                break;
        }
        Offset += OptLen + sizeof(S8);
    }

    return 1;
}

S32 GcodeM20Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !SdCardModel )
        return 0;
    /* 打印开始信息 */
    printf("Begin file list\n");

    /* 打印SDcard内文件信息（文件路径 + 大小） */
    PrintSdCardContent();

    /* 打印结束信息 */
    printf("End file list\n");
    return 1;
}

S32 GcodeM20Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
