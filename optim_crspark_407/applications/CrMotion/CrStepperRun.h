/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date            Author                 Notes
 * 2021-04-28     TingFengXuan       the first version
 */
#ifndef __CR_SETPPER_RUN_H__
#define __CR_SETPPER_RUN_H__


#ifdef __cplusplus
extern "C"
{
#endif

#include "Fifo.h"
#include "CrInc/CrType.h"


enum Axis_t
{
    X = 0,
    Y,
    Z,
    E,
    AxisNum,
    ALL_AXES = 0xFE     //配合M852命令
};
    
#define MIN(n,m)        ((n) < (m) ? (n) : (m))
#define MAX(n,m)        ((n) > (m) ? (n) : (m))
#define TRUE            (1)
#define FALSE           (0)
#define HWTIMER_DEV_NAME         "timer3"
#define TIMEOUT_US               (50)  //200
#define TICK_BUF_SIZE            (512)    //no less 3.
//------------------------------------------------------------------
//                            parameter
//------------------------------------------------------------------
extern CommonFiFo_t AxisFifo;

typedef struct
{
    U16 MotionFlag;            //高8位表示运动方向，低8位表示各轴运动状态。
    U16 StepPeriod;          //运动周期，范围1-65535
}MotionPeriod_t;

struct PlusPeriod_t
{
    MotionPeriod_t Count[TICK_BUF_SIZE];
};


//extern struct Fifo_t AxisFifo; //电机定时器脉冲周期的缓存，调用FIFO接口使用，不可直接操作.

S32 StepperRunInit(S32 AxisFd[], S32 Num);

S32 StepperPosGet(enum Axis_t Axis);
S32 StepperTimerRun();
VOID StepperTimerStop();
VOID SetStepperDir(U16 MotionFlag);

#ifdef __cplusplus
}
#endif

#endif
