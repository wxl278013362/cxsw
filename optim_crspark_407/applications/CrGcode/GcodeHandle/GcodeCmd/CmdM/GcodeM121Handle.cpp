/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     cx2470       the first version
 */

#include "GcodeM121Handle.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"

#define CMD_M121  "M121"

S32 GcodeM121Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
     if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
         return 0;

     S8 Cmd[CMD_BUFF_LEN] = {0};
     if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
         return 0;

     if ( strcmp(Cmd, CMD_M121) != 0 )
         return 0;

     return 1;
 }

S32 GcodeM121Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    MotionModel->EndStopGloballyMark(false);

    return 1;
}

S32 GcodeM121Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
