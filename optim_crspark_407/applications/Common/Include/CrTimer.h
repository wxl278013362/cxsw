#ifndef _CRTIMER_H
#define _CRTIMER_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef USING_OS_LINUX
#include <time.h>

typedef struct itimerspec CrTimerSpec_t;
typedef clockid_t CrClockid_t;
typedef struct sigevent CrSignalEvent_t;
typedef timer_t CrTimer_t;

#endif

#ifdef USING_OS_RTTHREAD

//#include <libc/libc_signal.h>
#include <rtthread.h>
#include <time.h>

//typedef void (*TimerCallBack)(rt_device_t dev, rt_size_t size);   //这是函数原型
//typedef VOID (*TimerCallBack)(VOID* dev, S32 size);  //这是函数原型的变种


union SigVal_t      /* Data passed with notification */
{
    S32  sival_int;         /* Integer value */
    VOID *sival_ptr;         /* Pointer value */
};

typedef struct SigEvent_t
{
    S32   sigev_notify; /* Notification method */
    S32   sigev_signo;  /* Notification signal */
    VOID  *sigev_notify_attributes;  /* Attributes for notification thread(SIGEV_THREAD) */
    VOID (*sigev_notify_function) (union SigVal_t);  /* 回调函数 */
    union SigVal_t sigev_value;  /* 回调函数参数 */
}CrSignalEvent_t;

typedef struct itimerspec CrTimerSpec_t;
typedef unsigned int CrClockid_t;
//typedef struct SigEvent_t CrSigEvent_t;
typedef VOID* CrTimer_t;

#endif

#ifdef __cplusplus
extern "C" {
#endif

//设置回调函数
struct CrTimerAttr_t
{
    S8  Name[12];    //定时器名称
    S16 Mode;   //HWTIMER_MODE_ONESHOT = 0x01 (单次), HWTIMER_MODE_PERIOD (周期性), 如果是系统定时器，还可以使用标志
    S16 HardwareTimer;   //是否是硬件定时器：0标识系统定时器，1标识高精度定时器
};

/*
* 功能：生成定时器的句柄。句柄不和CrTimerAttr_t相关
* 参数：CrTimer_t：将CrTimerAttr_t变量地址传递给CrTimerCreate，
*     ：使用sigevent的sigev_notify_function传递定时器的超时回调函数
*     ：使用sigevent的sigev_value传递定时器超时回调函数的传入参数
*/
/*只能使用 timer2, timer3,timer4,timer5*/
S32 CrTimerCreate(CrClockid_t clockid, CrSignalEvent_t *SigEvent, CrTimer_t *Timer);

S32 CrTimerDelete(CrTimer_t Timer);   //删除定时器
S32 CrTimerSetTime(CrTimer_t Timer, S32 Flag, const CrTimerSpec_t *NewTimerSpac, CrTimerSpec_t *OldTimerSpac);  //开启/关闭定时器

/*plese call CrTimerSetTime() before use this function*/
S32 CrTimerGetTime(CrTimer_t Timer, CrTimerSpec_t *TimeSpac);    //定时器触发的剩余时间

#ifdef TEST_TIMER
VOID TestTimer();
#endif

#ifdef __cplusplus
}
#endif

#endif
