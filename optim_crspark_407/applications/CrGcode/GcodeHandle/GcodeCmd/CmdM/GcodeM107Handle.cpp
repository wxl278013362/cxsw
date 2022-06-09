
/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     GcodeM107Handle.cpp
  Author :        FuMin
  Version:        1.0
  Description:    M107 GCODE Command
  Other:
  Modify History:
          <author>        <time>      <version>         <desc>
          FuMin         2021-01-21     V1.0.0.1
          cx2470        2020-10-28     first version
****************************************************************************/

#include "GcodeM107Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <rtthread.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrFans.h"

#define CMD_M107  "M107"

/* 解析M107指令
   M107 [P<index>]
     P<index>: 风扇索引
 */
S32 GcodeM107Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    //printf("[%s] %s \n", __FUNCTION__, CmdBuff);

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M107
    if ( strcmp(Cmd, CMD_M107) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M107) + sizeof(S8); // 加1是越过分隔符
    struct ArgM107_t *CmdArg = (struct ArgM107_t *)Arg;

    // P<index>
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
            case OPT_P:
                CmdArg->HasP = true;
                if ( OptLen > 1 ) CmdArg->P = atoi(Opt + 1);
                break;

            default:
                printf("Unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
 }

S32 GcodeM107Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM107_t *Param = (struct ArgM107_t *)ArgPtr;

    S32 Index = 0;  /* 假定Index = 0 为打印风扇， 当M107无参数时就是关闭此风扇 */
    if ( Param->HasP )
    {
        Index = Param->P;
    }

    // TODO: Fan Index
    if ( (PWM_FAN_COUNT < 1) || (Index < 0 )  || (Index >= PWM_FAN_COUNT) )
        return 1;
    //printf("Turn off fan \n");

    if ( !PwmFan[Index] )
        return 1;

    //设置速度为0
    struct PwmControllerAttr_t Attr = PwmFan[Index]->GetFanAttr();
    Attr.Duty = 0;
    PwmFan[Index]->SetFanAttr(Attr);

    return 1;
}

S32 GcodeM107Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

void OpenFan()
{
    if ( PwmFan[0] )
	{
	    struct PwmControllerAttr_t Attr = PwmFan[0]->GetFanAttr();
	    Attr.Duty = 255;
	    PwmFan[0]->SetFanAttr(Attr);
	}
}
MSH_CMD_EXPORT(OpenFan,OpenFan);

