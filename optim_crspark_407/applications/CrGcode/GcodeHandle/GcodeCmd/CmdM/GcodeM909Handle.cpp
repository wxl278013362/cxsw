#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GcodeM909Handle.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrInc/CrMsgType.h"
#include "CrMotion/CrStepperRun.h"

#define CMD_M909 "M909"

/******额外定义的变量和函数***********/
#define DAC_STEPPER_ORDER   { 0, 1, 2, 3 }
#define WITHIN(A,B,C)       ((A >= B) && (A <= C))
#define NEAR_ZERO(x)        WITHIN(x, -0.000001f, 0.000001f)
#define RECIPROCAL(x)       (NEAR_ZERO(x) ? 0 : (1 / float(x)))
#define DAC_STEPPER_SENSE   0.11

BOOL DacPresent = false;    //是否有DAC的标志位（在DAC初始化时被置为true）
U8 DacOrder[] = DAC_STEPPER_ORDER;
U8 Mcp4728GetDrvPct(U8 Channel)     //留出接口后面根据驱动更改成相应的接口
{
    return 0;
}
U16 Mcp4728GetValue(const U8 Channel)//留出接口后面根据驱动更改成相应的接口
{
    return 0;
}

FLOAT DacPerc(S8 n)
{
    return Mcp4728GetDrvPct(DacOrder[n]);
}
FLOAT DacAmps(S8 n)
{
    return Mcp4728GetValue(DacOrder[n]) * 0.125 * RECIPROCAL(DAC_STEPPER_SENSE * 1000);
}

VOID StepperDACPrintValue()
{
    if ( !DacPresent )
    {
        return ;
    }
    printf("Stepper current values in %% (Amps): X:%f (%f) Y:%f (%f) Z:%f (%f) E:%f (%f)\n",DacPerc(X),DacAmps(X),DacPerc(Y),DacAmps(Y),DacPerc(Z),DacAmps(Z),DacPerc(E),DacAmps(E));
}
/******额外定义的变量和函数***********/

S32 GcodeM909Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd,CMD_M909) != 0 )    //判断是否是M909命令
    {
        return 0;
    }
    return 1;
}

S32 GcodeM909Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    //打印DAC的值
    StepperDACPrintValue();
    return 1;
}

S32 GcodeM909Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
