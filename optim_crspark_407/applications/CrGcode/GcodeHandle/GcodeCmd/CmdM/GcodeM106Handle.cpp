/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     GcodeM106Handle.cpp
  Author :        FuMin
  Version:        1.0
  Description:    M106 GCODE Command
  Other:
  Modify History:
          <author>        <time>      <version>         <desc>
          FuMin         2021-01-21     V1.0.0.1
          cx2470        2020-10-28     first version
****************************************************************************/

#include "GcodeM106Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <rtthread.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrFans.h"

#define CMD_M106  "M106"

/* 解析M106指令
   M106 [I<index>] [P<index>] [S<speed>] [T<secondary>]
 */
S32 GcodeM106Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    // printf("[%s] %s \n", __FUNCTION__, CmdBuff);

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M106
    if ( strcmp(Cmd, CMD_M106) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M106) + sizeof(S8); // 加1是越过分隔符
    struct ArgM106_t *CmdArg = (struct ArgM106_t *)Arg;

    // [I<index>] [P<index>] [S<speed>] [T<secondary>]
    while ( Offset < Len )
    {
        S8 Opt[20] = {0}; // 操作参数
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) ) break;

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 ) // 没有操作码
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        // 解析出操作码
        switch ( Opt[0] ) // 操作类型
        {
            case OPT_I:
                CmdArg->HasI = true;
                if ( OptLen > 1 ) CmdArg->I = atof(Opt + 1);
                break;

            case OPT_P:
                CmdArg->HasP = true;
                if ( OptLen > 1 ) CmdArg->P = atoi(Opt + 1);
                break;

            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atof(Opt + 1);
                break;

            case OPT_T:
                CmdArg->HasT = true;
                if ( OptLen > 1 ) CmdArg->T = atoi(Opt + 1);
                break;

            default:
                printf("Unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
 }

static VOID *GetOptAsIndex(S32 Index)
{
    return NULL;
}
S32 GcodeM106Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM106_t *Param = (struct ArgM106_t *)ArgPtr;

    S32 Index = 0; /* 假定Index = 0 为打印风扇， 当M106无参数时就是全速打开此风扇 */
    if ( Param->HasP )
    {
        Index = Param->P;
    }

    // TODO: Fan Index
    //GetOptAsIndex(Index);
    if ( ((PWM_FAN_COUNT) < 1 ) || (Index < 0) || (Index >= (PWM_FAN_COUNT)) )
        return 1;

    if ( !PwmFan[Index] )
        return 1;

    struct PwmControllerAttr_t Attr = PwmFan[Index]->GetFanAttr();
    if ( Param->HasS )
    {
        //设置风扇速度
        Attr.Duty = (S32)Param->S;
        PwmFan[Index]->SetFanAttr(Attr);
    }

    if ( Param->HasI )
    {
        //TODO: 暂时不支持
        rt_kprintf("Warn:not support parameter [I<index>] \n");
    }

    if ( !Param->HasS && !Param->HasI )
    {
        //当未给定参数设置速度时，则设置为全速
        Attr.Duty = 255;
        PwmFan[Index]->SetFanAttr(Attr);
    }

    if ( Param->HasT )
    {
        //TODO: 暂时不支持
        rt_kprintf("Warn:not support parameter [T<secondary>] \n");
    }

    return 1;
}

S32 GcodeM106Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

