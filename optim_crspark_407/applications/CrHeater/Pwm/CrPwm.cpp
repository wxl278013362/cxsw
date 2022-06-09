#include "CrPwm.h"
#include "CrGpio/CrGpio.h"
#include "rtthread.h"
#include <stdio.h>
#include "Common/Include/CrSleep.h"
#include <string.h>

#define PWM_COUNT_MAX       (255)
#define MAX(n,m)            ((n) > (m) ? (n) : (m))

static U32 NameId = 0;


/*
VOID Exec(VOID *PData)
{
    CrPwmController *Pwm = (CrPwmController *)PData;
    
    struct PwmControllerAttr_t Attr = Pwm->CrPwmControllerGetAttr();
    U32 Fd = Pwm->CrPwmControllerGetPwmFd();

    Pwm->CrPwmControllerSetTimer(Attr.Freq);
    
    U32 delay = MAX(1000000 / Attr.Freq / PWM_COUNT_MAX , 1);
    
    while ( Attr.Times ++ <= PWM_COUNT_MAX)
    {
        if (Attr.Times < Attr.Duty)
        {
            struct PinParam_t Temp = {(GPIO_PinState)Attr.DefaultLevel};
            CrGpioIoctl(Fd, GPIO_SET_PINSTATE, &Temp);
        }
        else
        {
            struct PinParam_t Temp = {(GPIO_PinState)!Attr.DefaultLevel};
            CrGpioIoctl(Fd, GPIO_SET_PINSTATE, &Temp);
        }
        CrU_Sleep(delay);
    }
}
*/

CrPwmController::CrPwmController(S32 Fd ,FuncType Func)
    :ExecFunc(Func), PwmFd(Fd)
{
    struct CrTimerAttr_t TimeAttr = {0};
    sprintf(TimeAttr.Name,"Heater%d", NameId);
    NameId ++;
    TimeAttr.HardwareTimer = 0;
    TimeAttr.Mode = RT_TIMER_FLAG_PERIODIC;
    timer = &TimeAttr;
//    CrSignalEvent_t Sig;
    CrSignalEvent_t SigEvent = {0, 0, NULL, (VOID (*) (union SigVal_t))CrPwmController::TimeOut, {0}};
    SigEvent.sigev_notify_function = (VOID (*) (union SigVal_t))CrPwmController::TimeOut;
    SigEvent.sigev_value.sival_ptr = (VOID *)this;
    CrClockid_t ClockId = 0;

    CrTimerCreate(ClockId, &SigEvent, &timer);
    if ( timer != NULL)
    {
        CrTimerSpec_t OldTimerSpac = {0};
        CrTimerSpec_t NewTimerSpac = {{0,1000000}};

        CrTimerSetTime(timer, 1, &NewTimerSpac, &OldTimerSpac);
    }
    else
    {
        printf("******************** %s timer Create failed!\n",TimeAttr.Name);
    }

    memset(&Attr, 0, sizeof(Attr));
}
VOID CrPwmController::TimeOut(VOID *Para)
{
#if 0
    Exec(Para);

#else

    CrPwmController *Pwm = (CrPwmController *)Para;
    Pwm->ExecFunc(Pwm);

#endif
}

VOID CrPwmController::CrPwmControllerSetTimer(U32 Freq)
{
    CrTimerSpec_t OldTimerSpac = {0};
    CrTimerSpec_t NewTimerSpac = {{0, (S32)Freq}};
    if ( timer != NULL)
    {
        CrTimerSetTime(timer, 1, &NewTimerSpac, &OldTimerSpac);
    }
}

CrPwmController::~CrPwmController()
{
    CrGpioClose(PwmFd);
    if (timer != NULL)
    {
        CrTimerDelete(timer);
        timer = NULL;
    }
}

S32 CrPwmController::CrPwmControllerSetAttr(struct PwmControllerAttr_t &Attr)
{
    this->Attr.DefaultLevel = Attr.DefaultLevel;
    this->Attr.Duty = Attr.Duty;
    this->Attr.Freq = Attr.Freq;
    this->Attr.Times = Attr.Times;
    
    return 0;
}

const struct PwmControllerAttr_t &CrPwmController::CrPwmControllerGetAttr()
{
    return Attr;
}

S32 CrPwmController::CrPwmControllerGetPwmFd()
{
    return PwmFd;
}



