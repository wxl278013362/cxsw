/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-04     cx2375       the first version
 */
#include "PwmCallBack.h"
#include "stm32f4xx_hal.h"
#include "CrGpio/CrGpio.h"
#include "CrHeater.h"

#include "Common/Include/CrSleep.h"
#include <stdio.h>
#include "../HeaterDriver/CrHeaterDriver.h"
#define MAX(n,m)        ((n) > (m) ? (n) : (m))
#define PWM_COUNT_MAX    (255)

VOID PwmRunBed(VOID *PData)
{
#if 0
    if ( !PData )
    {
        printf("PData is NULL");
        return;
    }
    CrPwmController *Pwm = (CrPwmController *)PData;

    struct PwmControllerAttr_t Attr = Pwm->CrPwmControllerGetAttr();
    U32 Fd = Pwm->CrPwmControllerGetPwmFd();

    Pwm->CrPwmControllerSetTimer(Attr.Freq);

    U32 delay = MAX(1000000 / Attr.Freq / PWM_COUNT_MAX , 1);

    Attr.Times = 0;

    while ( Attr.Times ++ <= PWM_COUNT_MAX)
    {
        if (Attr.Times < Attr.Duty)
        {
            S32 level = 1;
            CrHeaterIoctl(Fd, HEATER_HEAT_POWERON, &level);
        }
        else
        {
            S32 level = 0;
            CrHeaterIoctl(Fd, HEATER_HEAT_POWERON, &level);
        }
        CrU_Sleep(delay);

    }
#endif
}

VOID HeartPwmRun(VOID *PData)
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
        CrHeaterIoctl(Fd, HEATER_HEAT_POWEROFF, &level);
    }
    else
    {
        S32 level = Attr.DefaultLevel;
        if (Attr.Duty != 0)
        {
            CrHeaterIoctl(Fd, HEATER_HEAT_POWERON, &level);
        }
    }
    Timeout = MAX(Timeout,1000000);
    Pwm->CrPwmControllerSetTimer(Timeout);

#if 0

    if ( !PData )
    {
        printf("PData is NULL");
        return;
    }
    CrPwmController *Pwm = (CrPwmController *)PData;

    struct PwmControllerAttr_t Attr = Pwm->CrPwmControllerGetAttr();
    U32 Fd = Pwm->CrPwmControllerGetPwmFd();

    Pwm->CrPwmControllerSetTimer(Attr.Freq);

    U32 delay = MAX(1000000 / Attr.Freq / PWM_COUNT_MAX , 1);

    Attr.Times = 0;

    while ( Attr.Times ++ <= PWM_COUNT_MAX)
    {

        if (Attr.Times < Attr.Duty)
        {
            S32 level = 1;
            CrHeaterIoctl(Fd, HEATER_HEAT_POWERON, &level);
        }
        else
        {
            S32 level = 0;
            CrHeaterIoctl(Fd, HEATER_HEAT_POWERON, &level);
        }
        CrU_Sleep(delay);
    }
#endif
}
