#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GcodeM910Handle.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrInc/CrMsgType.h"
#include "CrMotion/CrStepperRun.h"
#include "Common/Include/CrEepromApi.h"

#define CMD_M910 "M910"

/******额外定义的变量和函数***********/
#define SEQWRITE            0b01010000

#define DAC_STEPPER_VREF    1
#define DAC_STEPPER_GAIN    0

#define LowByte(w)          ((uint8_t) ((w) & 0xff))
#define HighByte(w)         ((uint8_t) ((w) >> 8))

U8 DacValues[4] = {0};      //保存Dac四个通道的数据值

/******额外定义的变量和函数***********/

S32 GcodeM910Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd,CMD_M910) != 0 )    //判断是否是M910命令
    {
        return 0;
    }
    return 1;
}

S32 GcodeM910Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }
    S8 Data = 0;
    //1、开始传输任务
    EepromStart();

    //2、写入数据
    Data = SEQWRITE;
    EepromWrite(&Data,1);

    //3、循环写入
    for ( int i = 0; i < 4; i++ )
    {
        Data = DAC_STEPPER_VREF << 7 | DAC_STEPPER_GAIN << 4 | HighByte(DacValues[i]);
        EepromWrite(&Data, sizeof(Data));
        Data = LowByte(DacValues[i]);
        EepromWrite(&Data,sizeof(Data));
    }

    //4、完成写入，关闭传输
    EepromFinish();
    return 1;
}

S32 GcodeM910Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
