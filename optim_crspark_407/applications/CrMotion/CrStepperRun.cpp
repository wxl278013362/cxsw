/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date            Author                 Notes
 * 2021-04-28     TingFengXuan       the first version
 */
#include "stm32f4xx_hal.h"
#include "CrGpio/CrGpio.h"
#include "Common/Include/CrTimer.h"
#include "CrStepperRun.h"
#include "Bresenham.h"
#include "CrStepper.h"
#include "CrMotionPlanner.h"
#include <string.h>
#include <stdio.h>
#include <rtthread.h>
#include <rtdevice.h>


rt_device_t hw_dev = NULL;

//======================================================
//==================== Parameter =======================
//======================================================

CommonFiFo_t AxisFifo = {0};
static struct PlusPeriod_t AxisPeriod = {0};
static S32 Steppers[AxisNum] = {0};
/* Hardware Timer.*/
static CrTimer_t NewCrTimer = NULL;

static VOID StepperInterrupt(union SigVal_t SigVal);
static inline VOID MoveOneStep(S32 *AxisFd, U16 MotionFlag);
/**
 * 函数功能: 初始化电机控制参数Fd,并创建电机运动Tick周期的缓存
 * 输入: 电机的Fd数组,电机数量
 * 输出: 无
 * 返回值: 0:初始化成功,小于0:初始化失败
 */
S32 StepperRunInit(S32 AxisFd[], S32 Num)
{
    for (int i = 0; i < Num; i++)
    {
        if( AxisFd[i] < 0 )
        {
            return -1;
        }
        Steppers[i] = AxisFd[i];
    }

    if (0 != CommonCreateFifo(&AxisFifo,AxisPeriod.Count, sizeof(AxisPeriod.Count[0]), sizeof(AxisPeriod.Count)/sizeof(AxisPeriod.Count[0])))
    {
//        printf("Create Stepper Fifo failed!\n");
        return -1;
    }
    
    StepperTimerRun();

    return 0;
}

/**
 * 函  数  名： StepperTimerSet
 * 功能描述：给步进电机定时器设置中断周期
 * 参        数：timeperiod，定时器周期，单位：uS
 * 返  回  值：0，设置成功，否则失败。
 * 备注：
 * 在函数StepperTimerInit()中，配置定时器基准时钟是1MHz，所以1tick=1uS
 * */

S32 StepperTimerSet(U32 timeperiod)
{
    S32 Ret = 0;

    CrTimerSpec_t OldTimerSpac = {0};

//    CrTimerGetTime(NewCrTimer, &OldTimerSpac);

    CrTimerSpec_t NewTimerSpac = {0};

    NewTimerSpac.it_interval.tv_sec = 0;

    NewTimerSpac.it_interval.tv_nsec = timeperiod * 1000;  //将传入的uS转换成纳秒。之所以做转换，仅按照驱动接口赋值而已。

    // NewCrTimer由StepperTimerInit()内初始化；1无实际意义；NewTimerSpac设置中断周期；OldTimerSpac未使用。
    Ret = CrTimerSetTime(NewCrTimer, 1, &NewTimerSpac, &OldTimerSpac);

    return Ret;
}
/**
 * 函  数  名： StepperTimerGet
 * 功能描述：获取步进电机定时器当前计数值
 * 参        数：无
 * 返  回  值：大于等于0，当前计数值；小于0，异常。uS
 * 备注：
 * 选择硬件定时器；
 * 配置硬件定时器基准频率；
 * 设置硬件定时器中断服务函数入口地址。
 * */

S32  StepperTimerGet()
{
    S32 Ret = 0;

    //U32 Level = 0;

    CrTimerSpec_t TimeSpac = {0};

    //Level =  rt_hw_interrupt_disable();

    Ret = CrTimerGetTime(NewCrTimer, &TimeSpac);

    //rt_hw_interrupt_enable(Level);

    //获取定时器当前计数值正常，则转换读取的值由纳秒为微秒。
    if(Ret != -1)
    {
        Ret = TimeSpac.it_value.tv_nsec / 1000; //一直是0，异常

        //PRINTF_LOGD("Ret = %d",Ret);
    }

    return Ret;
}
/**
 * 函数功能: 停止电机绑定的硬件定时器;如需更换绑定的硬件定时器,请修改宏:HWTIMER_DEV_NAME
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID StepperTimerStop()
{
    if ( !NewCrTimer )
    {
        return ;
    }

    StepperTimerSet(0xFFFF);

    //是触发时间设置成很大的一个值或设置为0，但是不能删除
}

/**
 * 函数功能: 启动电机绑定的硬件定时器;如需更换绑定的硬件定时器,请修改宏:HWTIMER_DEV_NAME
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
S32 StepperTimerRun()
{
    S32 Ret= 0;

    if(NULL == NewCrTimer)
    {
        rt_kprintf("NewCrTimer is NULL\r\n");

        struct CrTimerAttr_t CrTimerInfo = {0};

        CrSignalEvent_t SigEvent;

        memset(&SigEvent,0,sizeof(CrSignalEvent_t));

        strncpy(CrTimerInfo.Name,HWTIMER_DEV_NAME,sizeof(HWTIMER_DEV_NAME));    //定时器设备名，用于查找注册设备

        CrTimerInfo.Mode = RT_TIMER_FLAG_PERIODIC;                              //定时器模式选择为周期启动

        CrTimerInfo.HardwareTimer = 1;                                          //定时器类型为硬件定时器，与之相对的是软件定时器

    //    CrTimerInfo.HardWareFre = STEPPER_TIMER_RATE;                           //设置定时器的基准频率1MHz，1个tick是1uS.系统默认设置基准频率为1MHz

        SigEvent.sigev_notify_attributes = NULL;                                //设置定时器中断服务程序参数置空

        SigEvent.sigev_notify_function = StepperInterrupt;                      //定时器中断服务入口函数

        CrClockid_t ClockId = 0;                                                //定时器Id号

        NewCrTimer = &CrTimerInfo;

        Ret = CrTimerCreate(ClockId, &SigEvent, &NewCrTimer);                   //创建定时器

        if ( NULL == NewCrTimer )
        {
           printf("**** CrMotionInit create timer failed! ****\r\n");

            return Ret;
        }
    }

    StepperTimerSet(10000);                                                 //先设置一个初始定时周期

    return 0;
}
/**
 * 函数功能: 获取电机当前步数
 * 函数参数: 电机轴
 * 输入: 电机轴:X,Y,Z,E
 * 输出: 无
 * 返回值: 轴电机的步数
 */
S32 StepperPosGet(enum Axis_t axis)
{
    if (axis >= sizeof(Steppers) / sizeof(Steppers[0]))
    {
        return -1;
    }
    S32 Pos = -1;
    if(CrStepperIoctl(Steppers[axis],CRSTEPPER_GET_POS,&Pos) < 0)
    {
        Pos = ~((U32)(1));
        printf("Get %d axis pos failed!\n",Steppers[axis]);
    }
    return Pos;
}


VOID AwaitPulse(U32 StepTimerTicks)
{
    //StepperTimerSet(1000);                    //防止后续读数操作出现回滚异常

    S32 StartPulseCount = StepperTimerGet();

    while(StepTimerTicks > (StepperTimerGet() - StartPulseCount));

    return;
}

/**
 * 函数功能: 移动轴电机一步,并更新当前步数
 * 输入: AxisFd:电机Fd      Dir:电机运动方向(大于0或小于0)
 * 输出: 无
 * 返回值: 无
 * U16 MotionFlag示例
 * MSB  xxxx  xxxx  xxxx  xxxx LSB
 *      方向    方向     运动    运动
 * 方向：从高到低，分别表示XYZE(E1,E2...)轴方向。1表示正方向运动，0表示负方向运动
 * 运动：从高到低，分别表示XYZE(E1,E2...)运动状态。1表示运动，0表示不运动
 * 目前只用XYZE四个轴，预留了扩展，但需要修改代码。
 */

static inline VOID MoveOneStep(U16 MotionFlag)
{
    S32 *AxisFd = Steppers;

    S32 Low = 0,High = 1;

    U8 Motion = (MotionFlag & 0x00FF);                    //获取运动状态，从高到低依次为XYZE，1表示移步，0表示停止；

    LOOP_XYZE(i)                                                                //运动轴发脉冲，拉到电机驱动脉冲有效电平
    {
        if(Motion & (0x80 >> i))                                                //对应各轴的位是否置1
        {
            if (CrStepperIoctl(AxisFd[i],CRSTEPPER_SET_PIN_STATUS,&Low) < 0)
            {
                return;
            }

            if (CrStepperIoctl(AxisFd[i],CRSTEPPER_SET_PIN_STATUS,&High) < 0)
            {
                return;
            }
        }
    }

    //有效电平脉冲的持续时间，可以忽略

//    LOOP_XYZE(i)                                                                //运动轴发脉冲，拉到电机驱动脉冲有效电平
//    {
//        if(Motion & (0x80 >> i))
//        {
//            if (CrStepperIoctl(AxisFd[i],CRSTEPPER_SET_PIN_STATUS,&High) < 0)
//            {
//                return;
//            }
//        }
//    }
}

VOID SetStepperDir(U16 MotionFlag)
{
    S32 Pos[AxisNum] = {0};

    S32 *AxisFd = Steppers;

    U8 MotionDir = (MotionFlag & 0xFF00) >> 8;               //从高到低依次为XYZE，1表示正方向，0表示负方向；

    LOOP_XYZE(i)        //各轴设置方向
    {
        if (AxisFd[i] < 0)
        {
            return;
        }

        CrStepperIoctl(AxisFd[i],CRSTEPPER_GET_POS,&Pos[i]);

        if((0x80 >> i) & MotionDir)                             //对应各轴的位是否置1
        {
            if (CrStepperIoctl(AxisFd[i],CRSTEPPER_SET_POSITIVE_DIR,NULL) < 0)  //发送正方向信号
            {
                return;
            }

            Pos[i] ++;                                                             //坐标加1
        }
        else
        {
            if (CrStepperIoctl(AxisFd[i],CRSTEPPER_SET_NEGATIVE_DIR,NULL) < 0) //发送负方向信号
            {
                return;
            }

            Pos[i] --;
        }

        if (CrStepperIoctl(AxisFd[i],CRSTEPPER_SET_POS,&Pos[i]) < 0)           //有效脉冲输出，则认为已经运动一步
        {
            return;
        }
    }

    return;
}

/**
 * 函数功能: 电机硬件定时器的中断处理函数
 * 输入: SigVal:该参数无实际意义,可忽略
 * 输出: 无
 * 返回值: 无
 */
U32 Step = 1;
static VOID StepperInterrupt(union SigVal_t SigVal)
{
    S32 Ret = 0;

    S32 IsrSteps = 1;

    MotionPeriod_t MotionPeriod = {0};

    static U16 MotionDir = 0xFF00;

//    StepperTimerSet(60000);
    Ret = CommonReadFifo(&AxisFifo,&MotionPeriod,IsrSteps);          //从缓冲里面读取一个待执行参数

    if(Ret < IsrSteps)
    {
//        StepperTimerSet(500);
        return;
    }

    StepperTimerSet(MotionPeriod.StepPeriod);           //设置下一个运行周期

    if(MotionDir != (MotionPeriod.MotionFlag & 0xFF00))
    {
        SetStepperDir(MotionPeriod.MotionFlag);

        MotionDir = MotionPeriod.MotionFlag & 0xFF00;
    }

    MoveOneStep(MotionPeriod.MotionFlag);      //运动参数传入，方向和移步

//    StepperTimerSet(MotionPeriod.StepPeriod);           //设置下一个运行周期

    return;
}

