/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     FanPwmCallback.cpp
  Author :        chenli
  Version:        1.0
  Description:    fan callback
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenli          2021-1-20    V1.0.0.1
****************************************************************************/

#include "FanPwmCallback.h"
#include "stm32f4xx_hal.h"
#include "CrGpio/CrGpio.h"

#include "Common/Include/CrSleep.h"
#include <stdio.h>
#include "CrPwm.h"
#include "CrModel/CrFans.h"
#include "../Fan/CrFanCtrl.h"

#define MAX(n,m)        ((n) > (m) ? (n) : (m))
#define PWM_COUNT_MAX    (255)

VOID FanPwmRun(VOID *PData)
{
    if ( !PData )
    {
        printf("PData is NULL");
        return;
    }
    CrPwmController *Pwm = (CrPwmController *)PData;

    struct PwmControllerAttr_t Attr = Pwm->CrPwmControllerGetAttr();
    U32 Fd = Pwm->CrPwmControllerGetPwmFd();

    U32 delay = MAX( (float)Attr.Freq / PWM_COUNT_MAX , 1);/* ms */
    U32 Timeout = (U32)1000000 * Attr.Duty * delay;    /* nsec */

    struct PinParam_t Pin = {GPIO_PIN_RESET};
    if (CrGpioIoctl(Fd, GPIO_GET_PINSTATE, &Pin) < 0)
    {
        return ;
    }
    if (Timeout >= 10000000)
    {
        Timeout = 10000000;
    }
    if (Pin.PinState == GPIO_PIN_SET)
    {
        Timeout = 10000000 - Timeout;
        S32 level = !Attr.DefaultLevel;
        CrFanIoctl(Fd, CMD_FAN_CLOSE, &level);
    }
    else
    {
        S32 level = Attr.DefaultLevel;
        if (Attr.Duty != 0)
        {
            CrFanIoctl(Fd, CMD_FAN_OPEN, &level);
        }
    }
    Timeout = MAX(Timeout,1000000);
    Pwm->CrPwmControllerSetTimer(Timeout);

}
