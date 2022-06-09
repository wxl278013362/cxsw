/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-29     cx2470       the first version
 */

#include "GcodeM30Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrGcode.h"
#include "CrModel/CrSdCard.h"

#define CMD_M30  "M30"

S32 GcodeM30Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M30) != 0 )  //传进来的命令不是M18或M84命令
        return 0;

    S32 Offset = strlen(CMD_M30) + sizeof(S8); //加1是越过分隔符
    struct ArgM30_t *CmdArg = (struct ArgM30_t *)Arg;

    /*cmdbuff 远远小于CmdArg的长度，所以拷贝最大的长度以cmdbuff为准*/
    strncpy(CmdArg->FileName, CmdBuff + Offset, MAX_STRING_LENGTH);

    return 1;
}

S32 GcodeM30Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !SdCardModel )
        return 0;

    struct ArgM30_t *CmdArg = (struct ArgM30_t *)ArgPtr;

    if ( !SdCardModel->GetDevStatus() || strlen(CmdArg->FileName) <= 0 )
    {
        rt_kprintf("delete file error\n");
        return 1;
    }

    /*删除文件*/
    unlink(CmdArg->FileName);

    return 1;
}

S32 GcodeM30Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
