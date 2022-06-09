#include "stm32f4xx_hal.h"
#include "../Include/CrTimer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USING_OS_RTTHREAD
#include <rtthread.h>
#include <rtdevice.h>
#endif

//是否是硬件定时器：0标识系统定时器，1标识高精度定时器
#define CR_USE_HW_TIMER  1
#define CR_USE_SW_TIMER  0

#define CR_TIMER_DEFAULT_TIME_OUT  (500*1000)         //定时器默认超时时间 ms

#define COUNT(a) (sizeof(a) / sizeof(*a))

#define RET_CHECK(a, Timer) \
do{                         \
    if (a != RT_EOK )       \
    {                       \
        rt_timer_delete(Timer->Dev);\
        Timer->Dev = NULL;\
        if ( Timer )\
            free(Timer);\
        Timer = NULL;\
        return a; \
    }\
}while(0);

struct __Timer_t
{
    VOID                 *Dev;     //设备指针或者定时器的指针
    CrSignalEvent_t      SigEvent; //在他的里面有超时回调函数和回调函数的传入参数
    clockid_t            ClockId;
    CrTimerSpec_t        Time;     //
    struct CrTimerAttr_t Attr;
};

static S32 CheckTimerName(S8 *Name)
{
    if ( strcmp(Name, "timer2") == 0 )
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
    else if ( strcmp(Name, "timer3") == 0 )
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
    else if ( strcmp(Name, "timer4") == 0 )
    {
        __HAL_RCC_TIM4_CLK_ENABLE();
    }
    else if ( strcmp(Name, "timer5") == 0 )
    {
        __HAL_RCC_TIM5_CLK_ENABLE();
    }
    else
    {
        return -1;
    }
    return 0;
}

//高精度定时器
S32 CrTimerCreate(CrClockid_t clockid, CrSignalEvent_t * SigEvent, CrTimer_t *Timer)
{
    S32 Ret = 0;
    if ( !Timer || !SigEvent )
        return Ret;

    struct CrTimerAttr_t *TimerInfo = (struct CrTimerAttr_t *)(*Timer);
    
    struct __Timer_t *TimerTemp = (struct __Timer_t *)rt_malloc(sizeof(struct __Timer_t));
    if ( !TimerTemp )
        return Ret;

    switch ( TimerInfo->HardwareTimer )
    {
        case CR_USE_HW_TIMER:
        {
            if ( CheckTimerName(TimerInfo->Name) < 0 )
            {
                Ret = -10;
                break;
            }

            rt_device_t TimDev  = rt_device_find(TimerInfo->Name);
            if (TimDev == RT_NULL)
            {
                Ret = -10;
                break;
            }

            S32 ret = rt_device_open(TimDev, RT_DEVICE_OFLAG_RDWR);
            if (ret != RT_EOK)
            {
                Ret = -10;
                break;
            }

            rt_device_set_rx_indicate(TimDev, (rt_err_t (*)(rt_device_t, rt_size_t))SigEvent->sigev_notify_function);

            ret = rt_device_control(TimDev, HWTIMER_CTRL_MODE_SET, &TimerInfo->Mode);
            if (ret != RT_EOK)
            {
                Ret = -10;
                break;
            }
            TimerTemp->Dev = TimDev;

            break;
        }
        case CR_USE_SW_TIMER:
        {
            rt_timer_t TimerHande = rt_timer_create(TimerInfo->Name,(VOID (*)(VOID *))SigEvent->sigev_notify_function,
                                                    SigEvent->sigev_value.sival_ptr,\
                                                    CR_TIMER_DEFAULT_TIME_OUT,\
                                                    TimerInfo->Mode | RT_TIMER_FLAG_SOFT_TIMER );
            if ( !TimerHande )
            {
                Ret = -10;
                break;
            }
            TimerTemp->Dev = (VOID *)TimerHande;
            
            break;
        }
        default:
            break;
    }

    if ( 0 == Ret )
    {
        memcpy((VOID *)&TimerTemp->SigEvent, (VOID *)SigEvent, sizeof(CrSignalEvent_t));
        memcpy((VOID *)&TimerTemp->Attr, (VOID *)(*Timer), sizeof(struct CrTimerAttr_t));
        TimerTemp->ClockId = clockid;
        *Timer = (VOID *)TimerTemp;
    }
    else if ( -10 == Ret )
    {
        *Timer = NULL;
        if ( TimerTemp )
            free(TimerTemp);
        TimerTemp = NULL;
    }

    return Ret;
}

S32 CrTimerDelete(CrTimer_t Timer)
{
    S32 Ret = -1;

    if ( !Timer )
        return Ret;

    struct __Timer_t *Temp = (struct __Timer_t *)Timer;

    switch (Temp->Attr.HardwareTimer)
    {
        case CR_USE_HW_TIMER:
        {
            rt_device_control(Temp->Dev, HWTIMER_CTRL_STOP, NULL);
            rt_device_close(Temp->Dev);
            Temp->Dev = NULL;
            free(Timer);
            Timer = NULL;
            break;
        }
        case CR_USE_SW_TIMER:
        {
            RET_CHECK(-1, Temp);
            break;
        }
        default:
            break;
    }

    return Ret;
}

S32 CrTimerSetTime(CrTimer_t Timer, S32 Flag, const CrTimerSpec_t *NewTimerSpac, CrTimerSpec_t *OldTimerSpac)
{
    S32 Ret = -1;
    if ( !Timer || !NewTimerSpac || !OldTimerSpac ||  !((struct __Timer_t *)Timer)->Dev )
        return Ret;

    struct __Timer_t *Temp = (struct __Timer_t *)Timer;

    switch ( Temp->Attr.HardwareTimer )
    {
        case CR_USE_HW_TIMER:
        {
            rt_device_control(Temp->Dev, HWTIMER_CTRL_STOP, NULL);
            rt_hwtimerval_t timeout_s = {0};
            timeout_s.sec = NewTimerSpac->it_interval.tv_sec;      /* 秒 */
            timeout_s.usec = NewTimerSpac->it_interval.tv_nsec / 1000;     /* 微秒 */

            if (rt_device_write(Temp->Dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
            {
                rt_device_close(Temp->Dev);
                Temp->Dev = NULL;
                if ( Timer )
                    free(Timer);
                Timer = NULL;
                return RT_ERROR;
            }
            memcpy(&(Temp->Time), NewTimerSpac, sizeof(CrTimerSpec_t));
            memcpy(OldTimerSpac, NewTimerSpac, sizeof(CrTimerSpec_t));
            //rt_kprintf("CrTimerSetTime CR_USE_HW_TIMER successful.\r\n");
            break;
        }
        case CR_USE_SW_TIMER:
        {
            rt_tick_t Tick = 0;

            Ret = rt_timer_control(Temp->Dev, RT_TIMER_CTRL_GET_TIME, &Tick);
            RET_CHECK(Ret, Temp);
            Tick = (S32)(NewTimerSpac->it_interval.tv_sec * RT_TICK_PER_SECOND) + (S32)(NewTimerSpac->it_interval.tv_nsec / 1000000);
            Ret = rt_timer_control(Temp->Dev, RT_TIMER_CTRL_SET_TIME, &Tick);
            RET_CHECK(Ret, Temp);

            Ret = rt_timer_start((rt_timer_t)(Temp->Dev));
            RET_CHECK(Ret, Temp);
            
            OldTimerSpac->it_interval.tv_sec = Tick / RT_TICK_PER_SECOND;
            OldTimerSpac->it_interval.tv_nsec = (Tick % RT_TICK_PER_SECOND) * 1000000;
            break;
        }
        default:
            break;
    }

    return Ret;
}

/*plese call CrTimerSetTime() before use this function*/
S32 CrTimerGetTime(CrTimer_t Timer, CrTimerSpec_t *TimeSpac)
{
    if ( !Timer || !TimeSpac )
        return -1;
    struct __Timer_t *Temp = (struct __Timer_t *)Timer;
    switch (Temp->Attr.HardwareTimer)
    {
        case CR_USE_HW_TIMER:
        {
            rt_hwtimerval_t timeout_s;
            rt_device_control(Temp->Dev, HWTIMER_CTRL_INFO_GET, NULL);
            rt_device_read(Temp->Dev, 0, &timeout_s, sizeof(timeout_s));

            U64 OldTimeMs = Temp->Time.it_interval.tv_sec * 1000 + Temp->Time.it_interval.tv_nsec / 1000000;
            U64 CurrentMs = timeout_s.sec * 1000 + timeout_s.usec / 1000;
            S32  TimeOutMs = OldTimeMs - (CurrentMs % OldTimeMs);

            TimeSpac->it_interval.tv_sec = TimeOutMs / 1000;
            TimeSpac->it_interval.tv_nsec = (TimeOutMs % 1000) * 1000000;

            break;
        }
        case CR_USE_SW_TIMER:
        {
            U64 CurrentTick = rt_tick_get();
            S32 TimeOutTick = ((rt_timer_t)(Temp->Dev))->timeout_tick - CurrentTick;

            TimeSpac->it_interval.tv_sec  = TimeOutTick / RT_TICK_PER_SECOND;
            TimeSpac->it_interval.tv_nsec = (TimeOutTick % RT_TICK_PER_SECOND) * 1000000;
            break;
        }
        default:
            break;
    }

    return 0;
}

#ifdef TEST_TIMER

VOID TestTimerCallback(VOID *Param)
{
    rt_kprintf("%s\n", __FUNCTION__);

    return;
}

VOID TestTimer()
{
    CrSignalEvent_t SigEvent =
    {
        .sigev_notify = 0,
        .sigev_notify_attributes = NULL,
        .sigev_signo = 0,
        .sigev_notify_function = TestTimerCallback,
        .sigev_value = {0},
    };

    CrClockid_t ClockId = 0;

    struct CrTimerAttr_t CrTimerInfo = {
        .Name = "timer4",
        .HardwareTimer = 1,
        .Mode = RT_TIMER_FLAG_PERIODIC
    };

    CrTimer_t Crtimer = &CrTimerInfo;
    CrTimerCreate(ClockId, &SigEvent, &Crtimer);
    CrTimerSpec_t OldTimerSpac = {0};
    CrTimerSpec_t NewTimerSpac = {{1}};
    CrTimerSetTime(Crtimer, 1, &NewTimerSpac, &OldTimerSpac);

    printf("Old time out = %lds, %ldns\n",  OldTimerSpac.it_interval.tv_sec, OldTimerSpac.it_interval.tv_nsec);

    S32 Ret = 500;
    CrTimerSpec_t TimeOut = {0};
    while( Ret -- )
    {
        CrTimerGetTime(Crtimer, &TimeOut);
        printf("Remaining trigger time = %ds, %dms\n", TimeOut.it_interval.tv_sec, TimeOut.it_interval.tv_nsec / 1000000);

        rt_thread_delay(10);
    }

    CrTimerDelete(Crtimer);

    return ;
}

MSH_CMD_EXPORT(TestTimer , Test timer)

#endif


