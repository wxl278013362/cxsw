/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     cx2470       the first version
 */

#include "GcodeM118Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrSleep.h"

#define CMD_M118  "M118"

S32 GcodeM118Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
     if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
         return 0;

     S8 Cmd[CMD_BUFF_LEN] = {0};
     if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
         return 0;

     if ( strcmp(Cmd, CMD_M118) != 0 )
         return 0;

     S32 Offset = strlen(CMD_M118) + sizeof(S8); //加1是越过分隔符
     struct ArgM118_t *CmdArg = (struct ArgM118_t *)Arg;

     while ( Offset < Len )  //处理结束
     {
         S8 Opt[64] = {0};
         if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
             break;

         S32 OptLen = strlen(Opt);
         if ( OptLen <= 1 )
         {
             rt_kprintf("Gcode M114 has no param opt = %s\n", CmdBuff);
         }

         switch ( Opt[0] )  //判断命令
         {
             case OPT_E:
             {
                 if ( OptLen > 1 )
                 {
                     S32 E = atoi(Opt + 1);
                     if ( E == 1 )
                         CmdArg->HasE1 = true;
                 }
                 break;
             }
             case OPT_A:
             {
                 if ( OptLen > 1 )
                 {
                     S32 A = atoi(Opt + 1);
                     if ( A == 1 )
                         CmdArg->HasA1 = true;
                 }
                 break;
             }
             case OPT_P:
             {
                 CmdArg->HasP = true;
                 if ( OptLen > 1 )
                     CmdArg->P = atoi(Opt + 1);

                 break;
             }
             default:
             {
                 CmdArg->HasStr = true;
                 strcpy(CmdArg->Str, Opt);
                 break;
             }
         }

         Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
     }

     return 1;
 }

S32 GcodeM118Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM118_t *CmdArg = (struct ArgM118_t *)ArgPtr;
    if ( (!(CmdArg->HasA1)) && (!(CmdArg->HasE1)) )
        return 1;

    //获取要输出的串口索引 ( 先保存源码的串口索引，然后使用新的串口，用完后再恢复原来的串口 )
    S32 Index = 0;
    if ( CmdArg->HasP )
        Index = CmdArg->P;

    if ( CmdArg->HasA1 )
    {
        if ( CmdArg->HasStr )
            printf("// %s\n", CmdArg->Str);
    }
    else if ( CmdArg->HasE1 )
    {
        if ( CmdArg->HasStr )
            printf("%s\n", CmdArg->Str);
    }

    return 1;
}

S32 GcodeM118Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
