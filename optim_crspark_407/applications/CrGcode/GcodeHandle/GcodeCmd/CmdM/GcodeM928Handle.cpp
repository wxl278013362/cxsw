
#include "GcodeM928Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrSdCard.h"

#define CMD_M928  "M928"

S32 GcodeM928Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    printf("[%s] %s \n", __FUNCTION__, CmdBuff);

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M710
    if ( strcmp(Cmd, CMD_M928) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M928) + sizeof(S8); // 加1是越过分隔符
    struct ArgM928_t *CmdArg = (struct ArgM928_t *)Arg;

    if ( Offset + 2 < Len)  // 2是文件名中含字符'.'且命令的文件名之间有空格符。
    {
        S8* Index = strchr(CmdBuff + Offset, '.');
        //S8* Index2 = strchr(CmdArg + Offset, ' ');  //不会有组合命令
        if ( Index  )
        {
            strcpy((S8*)(CmdArg->FileName), CmdBuff + Offset);
        }
    }

    return 1;
 }

S32 GcodeM928Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM928_t *Param = (struct ArgM928_t *)ArgPtr;
    if ( strlen((S8*)(Param->FileName)) <= 0 )
        return 1;

    if ( SdCardModel )
        SdCardModel->OpenLogFile((S8*)(Param->FileName));

    return 1;
}

S32 GcodeM928Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

