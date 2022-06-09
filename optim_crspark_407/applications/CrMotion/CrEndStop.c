/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date              Author                 Notes
 * 2020-12-21     TingFengXuan       the first version
 */
#include <stm32f4xx_hal.h>
#include "CrEndStop.h"
#include "CrGpio/CrGpio.h"
#include "CrStepperRun.h"

#include <string.h>
#include <rtdevice.h>
#include <drivers/pin.h>
#include <drv_common.h>

//限位开关的最大数量
#define ENDSTOP_MAX_NUM                (4)
#define TRUE            (1)
#define FALSE           (0)

struct EndStop_t
{
    S8  *PinName;
    U8  TragLevel;
    U8  Enable;
    /* */
    U8 Triggered;
    S32 PinFd;
};
static struct EndStop_t  Limit[ENDSTOP_MAX_NUM] = {0};
/**
 * 函数功能: 通过引脚字符串名称获取引脚对应底层的编号
 * 函数参数: Str: 引脚字符串名称
 * 输入: Str: 引脚字符串名称
 * 输出: 参考返回值
 * 返回值: 引脚对应底层寄存器的编号
 */
static S32 GetPinNumber(const char* Str)
{
    char *Ptr = Str;
    U32 ret = 0;

    while(*Ptr != '\0' && *Ptr != ' ')
    {
        if (*Ptr >= '0' && *Ptr <= '9')
        {
            ret = ret * 10 + *Ptr - '0';
        }
        Ptr++;
    }
    char *Pin = strchr(Str, 'P');
    if (Pin == NULL)
    {
        rt_kprintf("<error> Pin format NOT SUPPORT:%s ret = %d!\n",Str,ret);
        return -1;
    }
    S32 Num = -1;
     switch(*(++Pin))
    {
        case 'A':
            {
                Num = GET_PIN(A, ret);
            }
        break;
        case 'B':
            {
                Num = GET_PIN(B, ret);
            }
        break;
        case 'C':
            {
                Num = GET_PIN(C, ret);
            }
        break;
        case 'D':
            {
                Num = GET_PIN(D, ret);
            }
        break;
        case 'E':
            {
                Num = GET_PIN(E, ret);
            }
        break;
        case 'F':
            {
                Num = GET_PIN(F, ret);
            }
        break;
        case 'G':
            {
                Num = GET_PIN(G, ret);
            }
        break;
        case 'H':
            {
                Num = GET_PIN(H, ret);
            }
        break;
        default:
//            printf("<error> NOT SUPPORT pin: P%c\n",*Pin);
            return -1;
    }
    return Num;
}
/**
 * 函数功能: 限位开关中断处理函数
 * 函数参数: Arg: 限位开关结构体指针
 * 输入: 无
 * 输出: Arg: 限位开关结构体指针
 * 返回值: 无
 */
static VOID EndStopIrq(VOID *Arg)
{
    if(Arg == NULL)
        rt_kprintf("IRQ Arg NULL\n");

    struct EndStop_t *EndStop = (struct EndStop_t *)Arg;
    //rt_kprintf("endstop trig:%s\n",EndStop->PinName);
    EndStop->Triggered = TRUE;
}
/**
 * 函数功能: 限位开关应用层处理,限位开关未触发时,清除触发标识
 * 函数参数: 无
 * 输入: 无
 * 输出: 限位开关未触发时,清除触发标识
 * 返回值: 无
 */
VOID EndStopHandler()
{
    for (int i = 0; i < ENDSTOP_MAX_NUM; i++)
    {
        if(Limit[i].PinName == NULL)
            return;
        S32 PinNum = GetPinNumber(Limit[i].PinName);
        if(PinNum >= 0)
        {
            if (Limit[i].Triggered == TRUE && Limit[i].TragLevel != rt_pin_read(PinNum))
            {
                Limit[i].Triggered = FALSE;
            }
        }
    }
}
/**
 * 函数功能: 打开限位开关描述符
 * 函数参数:Name: 限位开关对应引脚名称,Flag:无实际意义,Mode:无实际意义
 * 输入: Name: 限位开关对应引脚名称,
 * 输出: 参考返回值
 * 返回值: 限位开关描述符
 */
S32 CrEndStopOpen(S8 *Name, S32 Flag, S32 Mode)
{

    if ( !Name )
        return -1;

    U32 i = 0,m = -1;
    while ( i < ENDSTOP_MAX_NUM )
    {
        if (Limit[i].PinName == NULL && m == -1)
        {
            m = i;
        }
        if (strcmp((S8 *)Limit[i].PinName,Name) == 0)
        {
            return -1;
        }
        i ++;
    }

    if ( m < 0 || m >= ENDSTOP_MAX_NUM )
        return -1;

    S32 Fd = -1;
#if 0

    if ( (Fd = CrGpioOpen(Name, 0, 0)) < 0 )
        return -1;

    struct PinInitParam_t PinInit = {0};
    PinInit.Mode = GPIO_MODE_INPUT;
    PinInit.Pull = GPIO_PULLUP;
    PinInit.Speed = GPIO_SPEED_FREQ_HIGH;
    if ( CrGpioIoctl(Fd,GPIO_INIT,&PinInit) < 0 )
        return -1;
#endif
    Limit[m].PinName = Name;
    Limit[m].PinFd = Fd;
    Limit[m].TragLevel = 0xff;   //还没有对触发电平进行初始化
    /**
     * TO DO: PA5/PA6/PA7
     */
    /**
     * 绑定中断，下降沿模式，回调函数名为EndStopIrq,參數為struct                     EndStop_t *
     */
    S32 Num = GetPinNumber(Name);
    rt_pin_mode(Num, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(Num, PIN_IRQ_MODE_FALLING, EndStopIrq, &Limit[m]);
    rt_pin_irq_enable(Num, PIN_IRQ_ENABLE);
    return m;
}
/**
 * 函数功能: 关闭限位开关描述符
 * 函数参数:Fd:限位开关描述符
 * 输入: Fd:限位开关描述符
 * 输出: 无
 * 返回值: 无
 */
VOID CrEndStopClose(S32 Fd)
{
    if (Fd < 0 || Fd >= ENDSTOP_MAX_NUM)
    {
        return;
    }
    CrGpioClose(Limit[Fd].PinFd);
    
    Limit[Fd].PinFd = -1;
    Limit[Fd].PinName = NULL;
    Limit[Fd].TragLevel = 0xff;
    //要清除Fd对应的数组的内容
}
/**
 * 函数功能: 限位开关输入输出控制
 * 函数参数:Fd:限位开关描述符,Cmd: 控制指令,Arg:拓展参数
 * 输入: Fd:限位开关描述符,Cmd: 控制指令,Arg:拓展参数
 * 输出: Arg:拓展参数
 * 返回值: 小于0: 失败,0:成功
 */
S32 CrEndStopIoctl(S32 Fd, S32 Cmd, VOID *Arg)
{
    if ( Cmd < 0 )
        return -1;

    if ( Fd < 0 )
        return -1;

    int ret = -1;
    switch ( Cmd )
    {
        case CRENDSTOP_SET_TRAGLEVEL:
        {
            if ( Arg == NULL)
                return -1;

            Limit[Fd].TragLevel = *(U8 *)Arg;
            ret = 0;
            break;
        }
        case CRENDSTOP_GET_TRAGLEVEL:
        {
            if ( Arg == NULL)
                return -1;

            *(U8 *)Arg = Limit[Fd].TragLevel;
            ret = 0;
            break;
        }
        case CRENDSTOP_GET_TRAGSTATUS:
        {
            //指针判空
            if (Arg == NULL)
            {
                return -1;
            }
            /* check trag status.*/
            if (Limit[Fd].TragLevel == 0xff || Limit[Fd].Enable != 1)
            {
                return -1;
            }
            /**
             * TO DO:
             */
             #if 0
            struct PinParam_t Buf;
            ret = CrGpioRead(Limit[Fd].PinFd, (S8 *)&Buf, sizeof(Buf));
            *(U8 *)Arg = (Buf.PinState == Limit[Fd].TragLevel);
            #else
            S32 PinNum = GetPinNumber(Limit[Fd].PinName);
            if(Limit[Fd].TragLevel == rt_pin_read(PinNum))
                Limit[Fd].Triggered = TRUE;
            else
                Limit[Fd].Triggered = FALSE;

            *(U8 *)Arg = Limit[Fd].Triggered;
            
            ret = 1;
            #endif
            break;
        }
        case CRENDSTOP_SET_ENABLE:
        {
            Limit[Fd].Enable = 1;
            ret = 0;
            break;
        }
        case CRENDSTOP_SET_DISABLE:
        {
            Limit[Fd].Enable = 0;
            ret = 0;
            break;
        }
        default:
        {
            ret = 0;
            break;
        }
    }

    return ret;
}


