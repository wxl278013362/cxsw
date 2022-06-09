/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     GcodeM710Handle.cpp
  Author :        FuMin
  Version:        1.0
  Description:    M710 Gcode Command
  Other:
  Modify History:
          <author>        <time>      <version>     <desc>
          FuMin         2021-01-21     V1.0.0.1
****************************************************************************/

#include "GcodeM710Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrFans.h"

#define CMD_M710  "M710"

/* 解析M710指令
   M710 [A<bool>] [D<seconds>] [I<speed>] [R<bool>] [S<speed>]
 */
S32 GcodeM710Parse(const S8 *CmdBuff, VOID *Arg)
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
    if ( strcmp(Cmd, CMD_M710) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M710) + sizeof(S8); // 加1是越过分隔符
    struct ArgM710_t *CmdArg = (struct ArgM710_t *)Arg;

    // [A<bool>] [D<seconds>] [I<speed>] [R<bool>] [S<speed>]
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
            case OPT_A:
                CmdArg->HasA = true;
                if ( OptLen > 1 ) CmdArg->A = (BOOL)atoi(Opt + 1);
                break;

            case OPT_D:
                CmdArg->HasD = true;
                if ( OptLen > 1 ) CmdArg->D = atoi(Opt + 1);
                break;

            case OPT_I:
                CmdArg->HasI = true;
                if ( OptLen > 1 ) CmdArg->I = atof(Opt + 1);
                break;

            case OPT_R:
                CmdArg->HasR = true;
                if ( OptLen > 1 ) CmdArg->R = (BOOL)atoi(Opt + 1);
                break;

            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atof(Opt + 1);
                break;

            default:
                printf("Unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
 }

S32 GcodeM710Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM710_t *Param = (struct ArgM710_t *)ArgPtr;
    if ( PWM_FAN_COUNT < 1 )
        return 1;

    U8 i = 0;
    for ( i= 0; i < (PWM_FAN_COUNT) ; i++ )
    {
        if ( !PwmFan[i] )
            continue;

        struct PwmControllerAttr_t Attr = PwmFan[i]->GetFanAttr();

    // 执行A操作
    if (Param->HasA)
    {
        // 设置是否自动设置风扇速度。关闭控制器时风扇速度保持不变。
        // TODO
    }

        // 执行D操作
        if (Param->HasD)
        {
            // 设置额外的一段持续时间(单位:秒)，以在电动机关闭后风扇依然能保持较高速度
            PwmFan[i]->DelayedTurnOff(Param->D);
        }

    // 执行I操作
    if (Param->HasI)
    {
        // 设置电动机关闭时控制器风扇的速度
        // TODO
    }

    // 执行R操作
    if (Param->HasR)
    {
        // 恢复所有设置到默认值
        // TODO
    }

        // 执行S操作
        if ( Param->HasS )
        {
            // 设置电动机工作时控制器风扇的速度
            Attr.Duty = (S32)Param->S;
            PwmFan[i]->SetFanAttr(Attr);
        }
    }

    if (!(Param->HasA || Param->HasD || Param->HasI || Param->HasR || Param->HasS ))
    {
        for ( i= 0; i < (PWM_FAN_COUNT) ; i++ )
        {
            if ( !PwmFan[i] )
                continue;

            printf("controller fan %d speed is %d\n", i, PwmFan[i]->GetFanAttr().Duty);
        }
    }


    return 1;
}

S32 GcodeM710Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

