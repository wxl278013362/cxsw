/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-29     cx2470       the first version
 */
#include "GcodeM110Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <rtthread.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrInc/CrMsgType.h"
#include "CrModel/GcodeSource.h"

#define CMD_M110  "M110"

S32 GcodeM110Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
     if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
         return 0;
    
     S8 Cmd[CMD_BUFF_LEN] = {0};
     if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
         return 0;

     if ( strcmp(Cmd, CMD_M110) != 0 )
         return 0;

     S32 Offset = strlen(CMD_M110) + sizeof(S8); //加1是越过分隔符
     struct ArgM110_t *CmdArg = (struct ArgM110_t *)Arg;
     while ( Offset < Len )  //处理结束
     {
         S8 Opt[20] = {0};
         if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
             break;

         S32 OptLen = strlen(Opt);
         if ( OptLen <= 1 )
         {
             rt_kprintf("Gcode M109 has no param opt = %s\n", CmdBuff);
         }

         switch ( Opt[0] )  //判断命令
         {
             case OPT_N:
             {
                 CmdArg->HasN = true;
                 CmdArg->N = atoi(Opt + 1);
                 break;
             }
             default:
             {
                 //rt_kprintf("Gcode M110 No default Opt = %s\n", CmdBuff);
                 break;
             }
         }

         Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
     }
    return 1;
}

S32 GcodeM110Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        rt_kprintf("Param is null\n");
        return 0;
    }

    struct ArgM110_t *Param = (struct ArgM110_t *)ArgPtr;
    if (Param->HasN)
    {
        GcodeUart::LastCmdNumber = Param->N;
    }

    return 1;
}
S32 GcodeM110Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


