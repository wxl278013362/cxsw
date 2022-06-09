#include "GcodeM7Handle.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrSleep.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include <stdio.h>
#include <stdlib.h>

#define CMD_M7 ("M7")

S32 GcodeM7Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    if ( strncmp(CmdBuff, CMD_M7, strlen(CMD_M7)) != 0 )
    {
        return CrErr;
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM7Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    while( !IsPlanLineBuffEmpty(LineBuff) ) //等待设备空闲
    {
        CrU_Sleep(1000);
    }

    struct PinParam_t Pin = {GPIO_PIN_RESET};
    Pin.PinState = (GPIO_PinState)!COOLANT_MIST_INVERT;
    CrGpioWrite(COOLANT_MIST_PIN, (S8 *)&Pin, 0);

    return GCODE_EXEC_OK;
}

S32 GcodeM7Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

