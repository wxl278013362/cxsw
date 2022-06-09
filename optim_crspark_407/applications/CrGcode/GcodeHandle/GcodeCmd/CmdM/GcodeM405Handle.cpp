#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GcodeM405Handle.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrInc/CrMsgType.h"
#include "CrMotion/CrStepperRun.h"

#define CMD_M405 "M405"

/******额外定义的变量和函数***********/
#define MEASUREMENT_DELAY_CM        14  //从料丝传感器到挤出机融化室的距离
BOOL    SensorFlag = false;
S8      MeasDelayCm = 0;                //系统中保存的传感器到挤出机熔化室的距离
VOID SetDelayCm(const U8 Cm)
{
    MeasDelayCm = Cm < MEASUREMENT_DELAY_CM ? Cm : MEASUREMENT_DELAY_CM;
}

VOID FilamentEnable(const BOOL Flag)
{
    SensorFlag = Flag;

    //启用时需要再添加相应的传感器开启函数

}
/******额外定义的变量和函数***********/

S32 GcodeM405Parse(const S8 *CmdBuff, VOID *Arg)
{
    //1、判空
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    //2、判断Gcode命令长度是否大于1且是否为M命令
    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || CmdBuff[0] != GCODE_CMD_M )
    {
        return 0;
    }

    //3、获取命令
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )   //防止获取命令失败
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_M405) != 0 )    //判断是否是M405命令
    {
        return 0;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M405) + sizeof(S8);
    struct ArgM405_t *CmdArg = (struct ArgM405_t *)Arg;

    S32 OptLen = 0;
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;
        }

        OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M405 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_D:
            {
                CmdArg->HasD = true;
                if ( OptLen > 1 )
                {
                    CmdArg->D = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->HasD = false;
                }
                break;
            }
            default:
                break;
        }
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

S32 GcodeM405Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM405_t *Param = (struct ArgM405_t *)ArgPtr;

    //1、设置传感器到挤出机熔化室的距离(cm)
    if ( Param->HasD )
    {
        SetDelayCm(Param->D);
    }

    //2、开启料丝传感器(目前还未启用料丝传感器，故只留下实现接口)
    FilamentEnable(TRUE);


    return 1;
}

S32 GcodeM405Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
