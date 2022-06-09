/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     CrBeep.c
  Author :        FuMin
  Version:        1.0
  Description:    蜂鸣BSP接口
  Other:
  Mode History:
          <author>        <time>      <version>     <desc>
          FuMin         2021-01-18     V1.0.0.1
****************************************************************************/

#include "CrBeep.h"
#include "CrGpio/CrGpio.h"
#include "Common/Include/CrSleep.h"

/* 打开蜂鸣器
 * @Name: 蜂鸣器引脚名称
 * @Flag: NA
 * @Mode: NA
 * @Return: 设备节点Fd
 */
S32 CrBeepOpen(S8 *Name, U32 Flag, U32 Mode)
{
    struct PinInitParam_t Pin = { 0 };

    Pin.Mode = GPIO_MODE_OUTPUT_PP;
    Pin.Pull = GPIO_NOPULL;
    Pin.Speed = GPIO_SPEED_FREQ_HIGH;

    S32 Fd = CrGpioOpen((S8*)Name, Flag, Mode);
    if ( Fd < 0 )
    {
        return 0;
    }

    CrGpioIoctl(Fd, GPIO_INIT, (VOID *)&Pin);

    return Fd;
}

/* 关闭蜂鸣器
 * @Fd: 设备节点Fd
 */
VOID CrBeepClose(S32 Fd)
{
    if ( Fd >= 0 )
    {
        CrGpioClose(Fd);
    }
}

static VOID CrBeepSet(S32 Fd, U32 DurationMs)
{
    struct PinParam_t Pin = { GPIO_PIN_RESET };

    Pin.PinState = GPIO_PIN_SET;
    CrGpioWrite(Fd, (S8 *)&Pin, 0);

    CrM_Sleep(DurationMs);

    Pin.PinState = GPIO_PIN_RESET;
    CrGpioWrite(Fd, (S8 *)&Pin, 0);
}

/* IO控制蜂鸣器
 * @Fd: 设备节点Fd
 * @Cmd:
 *     BEED_CMD_SHORT_BEEPS    设置短鸣
 *     BEED_CMD_LONG_BEEPS     设置长鸣
 *     BEED_CMD_UNFIXED_BEEPS  设置指定时间的蜂鸣 (MS)
 * @Param:
 *           指定时间 (BEED_CMD_UNFIXED_BEEPS), 数值类型强制转换成VOID*
 * @Return:
 *     -1: 失败
 *     0:  成功
 */
S32 CrBeepIoctl(S32 Fd, S32 Cmd, VOID *Param)
{
    S32 Ret = -1;
    U8 IsValidCmd = 1;
    U32 DurationMs = 0;

    if ( Fd < 0 )
    {
        return Ret;
    }

    switch ( Cmd )
    {
        /* 短鸣 */
        case  BEED_CMD_SHORT_BEEPS:
            DurationMs = SHORT_BEEP_DURATION;
            CrBeepSet(Fd, DurationMs);
            break;

        /* 长鸣 */
        case BEED_CMD_LONG_BEEPS:
            DurationMs = LONG_BEEP_DURATION;
            CrBeepSet(Fd, DurationMs);
            break;

        /* 指定时间的蜂鸣 (毫秒) */
        case BEED_CMD_UNFIXED_BEEPS:
            DurationMs = (U32)Param;
            CrBeepSet(Fd, DurationMs);
            break;

        default:
            IsValidCmd = 0;
            break;
    }

    if ( IsValidCmd )
    {
        Ret = CrGpioIoctl(Fd, Cmd, NULL);
    }

    return Ret;
}


/*
 * API单元测试
 */
#ifdef BEEP_UNIT_TEST

#include <rtthread.h>

#define DBG_TAG "Beep"
#include <rtdbg.h>


#define BEEP_DURATION_MAX  15
static S32 DurationMs = BEEP_DURATION_MAX;
static VOID TimeoutCallback(void *context)
{
    LOG_D("TimeoutCallback \n");

    /*
          * 反复的调小蜂鸣时间
     */
    DurationMs -= 1;
    if ( DurationMs <= 0 )
    {
        DurationMs = BEEP_DURATION_MAX;
    }

    S32 BeepFd = (S32)context;
    CrBeepIoctl(BeepFd, BEED_CMD_UNFIXED_BEEPS, (VOID *)DurationMs);
}


void BeepUnitTest()
{
    /* 打开蜂鸣器 */
    S32 BeepFd = CrBeepOpen(BEEP_PIN_NAME, 0, 0);
    if ( BeepFd < 0 ) return;

    S32 Count = 2;
    while ( Count-- > 0 )
    {
        /* 短鸣 */
        CrBeepIoctl(BeepFd,  BEED_CMD_SHORT_BEEPS, NULL);
        CrM_Sleep(1000);

        /* 长鸣 */
        CrBeepIoctl(BeepFd, BEED_CMD_LONG_BEEPS, NULL);
        CrM_Sleep(1000);
    }

    /* 关闭蜂鸣器 */
    CrBeepClose(BeepFd);

    /* 重新打开蜂鸣器 */
    BeepFd = CrBeepOpen("PC6", 0, 0);
    if ( BeepFd < 0 ) return;

    /*
          * 创建可反复执行的定时器，在定时器回调中设置指定时间的蜂鸣
     */
    rt_timer_t BeepTimer = rt_timer_create("beep_timer",
                                            TimeoutCallback,
                                            (void*)BeepFd,
                                            1000,
                                            RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_PERIODIC);
    if ( BeepTimer == NULL )
    {
        LOG_E("Create BeepTimer failed \n");

        /* 关闭蜂鸣器 */
        CrBeepClose(BeepFd);
    }
    else
    {
        rt_timer_start(BeepTimer);
    }
}
#endif /* BEEP_UNIT_TEST */










