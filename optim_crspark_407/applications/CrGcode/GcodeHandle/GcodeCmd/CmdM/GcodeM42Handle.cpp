#include "GcodeM42Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrFans.h"
#include "CrGpio/CrGpio.h"

#define CMD_M42  "M42"

S32 GcodeM42Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M42) != 0 )  //传进来的命令不是M18或M84命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    S32 Offset = strlen(CMD_M42) + sizeof(S8); //加1是越过分隔符
    struct ArgM42_t *CmdArg = (struct ArgM42_t *)Arg;

    while ( Offset < Len )  //处理结束
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            //printf(" Cmd get opt failed  cmd = %s\n", CmdBuff);
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_P:
            {
                CmdArg->HasP = true;
                if ( OptLen > 1 ) CmdArg->P = atoi( Opt + 1 );
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atoi( Opt + 1 );

                break;
            }
            case OPT_M:
            {
                CmdArg->HasM = true;
                if ( OptLen > 1 ) CmdArg->M = atoi( Opt + 1 ) ? true : false;

                break;
            }
            case OPT_I:
            {
                CmdArg->HasI = true;
                if ( OptLen > 1 ) CmdArg->I = atoi(Opt + 1) ? true : false;

                break;
            }
            default:   /*关闭所有电机*/
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}

S32 GcodeM42Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM42_t *Arg = (struct ArgM42_t *)ArgPtr;
    if ( !Arg->HasS )
        return 1;

    S32 Pin = -1;
    if ( !Arg->HasP )
    {
        Pin = CrGpioOpen((S8 *)LED_PIN, 0, 0);
    }
    else
    {
        //要有pin编号和组的转换的给个书
        S8 Name[6] = {0};
        CrGpioGetPinName(Arg->P, Name);
        Pin = CrGpioOpen(Name, 0, 0);
    }

    if ( Pin < 0 )
        return 1;

    //没有设置引脚的模式
    //没有设置引脚的0-255状态
    //没有获取引脚保护
    CrGpioIoctl(Pin, GPIO_SET_PINSTATE, &(Arg->S));

    if ( PWM_FAN_COUNT < 1 )
        return 1;

//    S8 i = 0;  //warning: array subscript has type 'char'
    S32 i = 0;
    for ( i = 0; i < PWM_FAN_COUNT ; ++i )
    {
        if ( !PwmFan[i] )
            continue;

        if ( PwmFan[i]->GetFanFd() == Pin )
        {
            struct PwmControllerAttr_t Attr = PwmFan[i]->GetFanAttr();
            Attr.Duty = Arg->S;
            PwmFan[i]->SetFanAttr(Attr);
            break;
        }
    }

    return 1;
}

S32 GcodeM42Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
