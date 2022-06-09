/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date            Author                 Notes
 * 2021-04-29     TingFengXuan       the first version
 */

#include "Bresenham.h"
#include "StepperApi.h"
#include "CrStepperRun.h"
#include "../Include/CrSleep.h"

#include <rtthread.h>
#include "CrInc/CrType.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "CrMotion/CrEndStop.h"

#define MIN_STEP_RATE           (32)                //16的原因是：定时器基准频率1MHz，最大计数65535，最大定时时间65.535ms，所以小于16无法完成定时。

//布雷森算法数据结构体,用于缓存直线运动数据
struct Bresenham_t Substance;
//每个轴Tick周期的累计
//static S32 CumulatePeriod[AxisNum] = {0};
//创建布雷森算法对象
Bresenham SyncAlgorithm;


#define MultiU32X24toH32(n1,n2)  (((S64)(n1) * (n2) + 0x00800000) >> 24) //32位乘24位转32位乘法

#define BASE_ISR_ENTER_EXIT             (900)    // from Marlin 由于我们使用了多次封装，该值需要调大，初步由792到900
#define TOGGLE_PIN_X                    (16)      // from Marlin
#define TOGGLE_PIN_Y                    (16)      // from Marlin
#define TOGGLE_PIN_Z                    (16)      // from Marlin
#define TOGGLE_PIN_E                    (16)      // from Marlin
#define ISR_LOOP_BASE_CYCLES            (4)       // from Marlin
#define MIN_STEPPER_PULSE               (3)         // from Marlin 由1改为3,CPU主频提高，但脉宽时长不变，所以提高脉宽对应的Cycle数
#define TOGGLE_PIN_CYCLE                (TOGGLE_PIN_X+TOGGLE_PIN_Y+TOGGLE_PIN_Z+TOGGLE_PIN_E)

#define _MIN_STEPPER_PULSE_CYCLES(N)    MAX(U32((F_CPU)/(MAX_STEPPER_RATE)), U32((F_CPU)/500000UL)*(N))
#define MIN_STEPPER_PULSE_CYCLES        _MIN_STEPPER_PULSE_CYCLES(U32(MIN_STEPPER_PULSE))
#define ISR_LOOP_CYCLES                 (ISR_LOOP_BASE_CYCLES + MAX(MIN_STEPPER_PULSE_CYCLES, TOGGLE_PIN_CYCLE))

//其公式和部分参数均来自于Marlin。
#define STEP_ISR_COST_CPU_CYCLES(N)     ((BASE_ISR_ENTER_EXIT + (ISR_LOOP_CYCLES)*(N))/(N))

#define MAX_STEP_ISR_FRE_1X             ((F_CPU)/STEP_ISR_COST_CPU_CYCLES(1))
#define MAX_STEP_ISR_FRE_2X             ((F_CPU)/STEP_ISR_COST_CPU_CYCLES(2))
#define MAX_STEP_ISR_FRE_4X             ((F_CPU)/STEP_ISR_COST_CPU_CYCLES(4))
#define MAX_STEP_ISR_FRE_8X             ((F_CPU)/STEP_ISR_COST_CPU_CYCLES(8))
#define MAX_STEP_ISR_FRE_16X            ((F_CPU)/STEP_ISR_COST_CPU_CYCLES(16))
#define MAX_STEP_ISR_FRE_32X            ((F_CPU)/STEP_ISR_COST_CPU_CYCLES(32))
#define MAX_STEP_ISR_FRE_64X            ((F_CPU)/STEP_ISR_COST_CPU_CYCLES(64))
#define MAX_STEP_ISR_FRE_128X           ((F_CPU)/STEP_ISR_COST_CPU_CYCLES(128))

static U32 FreqLimit[] =
{
        (MAX_STEP_ISR_FRE_1X),
        (MAX_STEP_ISR_FRE_2X >> 1),
        (MAX_STEP_ISR_FRE_4X >> 2),
        (MAX_STEP_ISR_FRE_8X >> 3),
        (MAX_STEP_ISR_FRE_16X >> 4),
        (MAX_STEP_ISR_FRE_32X >> 5),
        (MAX_STEP_ISR_FRE_64X >> 6),
        (MAX_STEP_ISR_FRE_128X >> 7)
};


/**
 * 函数功能: 打印保留2位小数的浮点数(rt-thread系统不支持打印浮点)
 * 函数参数: f:浮点数据
 * 输入: f:浮点数据
 * 输出: 无
 * 返回值: 无
 */
VOID PrintFloat(float f)
{
#if 0
    int temp = (int)(f * 100);
    int Integer = temp / 100;
    int n = temp % 100 / 10;
    int m = temp % 10;
    if(f < -0.000001)
       printf("-%d.%d%d\t",-Integer,-n,-m);
    else
       printf("%d.%d%d\t",Integer,n,m);
#else
    int temp = (int)(f * 10000);
    int Integer = temp / 10000;
    int n = temp % 10000 / 1000;
    int m = temp % 1000 / 100;
    int k = temp % 100 / 10;
    int l = temp % 10;
    if(f < -0.000001) 
       printf("-%d.%d%d%d%d\t",-Integer,-n,-m,-k,-l);
    else
       printf("%d.%d%d%d%d\t",Integer,n,m,k,l);
#endif
}
/**
 * 函数功能: 打印直线运动参数(主要用于调试信息查看)
 * 函数参数: Line:直线结构体指针
 * 输入: Line:直线结构体指针
 * 输出: 无
 * 返回值: 无
 */
VOID PrintLine(struct PlanLine_t *Line)
{
//    printf("Line:\n");
//
//    if (Line == NULL)
//    {
//        printf("(NULL)\n");
//        return;
//    }
//    printf("----Usabled----\n");
//    printf("Usabled:%d\n",Line->Usable);
//    printf("----PlanCalc----\n");
//    printf("EnterStatus:%d\n",(S32)Line->PlanCalc.EnterStatus);
//    printf("IsEndLine:%d\n",(S32)Line->PlanCalc.IsEndLine);
//    printf("Acceleration: ");
//    PrintFloat(Line->PlanCalc.Acceleration);
//    printf("\n");
//
//    printf("EntrySpeed: ");
//    PrintFloat(Line->PlanCalc.EntrySpeed);
//    printf("\n");
//
//    printf("ExitSpeed: ");
//    PrintFloat(Line->PlanCalc.ExitSpeed);
//    printf("\n");
//
//    printf("MaxSpeed: ");
//    PrintFloat(Line->PlanCalc.MaxSpeed);
//    printf("\n");
//
//    printf("LineLength: ");
//    PrintFloat(Line->PlanCalc.LineLength);
//    printf("\n");
//    printf("----AxisInfo----\n");
//    printf("----X:\n");
//    printf("Steps:%d\n",Line->Axis[X].Steps);
//    printf("Dir:%d\n",Line->Axis[X].Dir);
//    printf("DelSteps:%d\n",Line->Axis[X].DecelStartStep);
//    printf("UniSteps:%d\n",Line->Axis[X].UniformStartStep);
//    printf("----Y:\n");
//    printf("Steps:%d\n",Line->Axis[Y].Steps);
//    printf("Dir:%d\n",Line->Axis[Y].Dir);
//    printf("DelSteps:%d\n",Line->Axis[X].DecelStartStep);
//    printf("UniSteps:%d\n",Line->Axis[X].UniformStartStep);
//    printf("----Z:\n");
//    printf("Steps:%d\n",Line->Axis[Z].Steps);
//    printf("Dir:%d\n",Line->Axis[Z].Dir);
//    printf("DelSteps:%d\n",Line->Axis[X].DecelStartStep);
//    printf("UniSteps:%d\n",Line->Axis[X].UniformStartStep);
//    printf("----E:\n");
//    printf("Steps:%d\n",Line->Axis[E].Steps);
//    printf("Dir:%d\n",Line->Axis[E].Dir);
//    printf("DelSteps:%d\n",Line->Axis[X].DecelStartStep);
//    printf("UniSteps:%d\n",Line->Axis[X].UniformStartStep);
}
/**
 * 函数功能: 计算开平方
 * 函数参数: Num:被开平方数据
 * 输入: Num:被开平方数据
 * 输出: 参考返回值
 * 返回值: 开平方结果
 */
static FLOAT CalcSqrt(FLOAT Num)
{
    if (Num >= -EOL_DATA && Num <= EOL_DATA)
        return 0;
    FLOAT Xhalf = 0.5f * Num;
    U32 i = *(U32*)&Num;
    i = 0x5f3759df - (i >> 1);
    Num = *(float *)&i;
    Num = Num * (1.5f - Xhalf * Num * Num);
    return (1.0f / Num);
}

#if 0
/**
 * 函数功能: 计算脉冲周期Tick数
 * 函数参数: accel:加速度,InitV:初速度,steps:当前步数
 * 输入: accel:加速度,InitV:初速度,steps:当前步数
 * 输出: 参考返回值
 * 返回值: 电机硬件定时器的周期Tick数
 */
static inline S32 CalculateInterval(FLOAT accel, FLOAT InitV, S32 steps)
{
    if (accel <= 0)
    {
        return 0;
    }
    FLOAT TargetV = InitV + 2 * accel * steps;
    FLOAT Last_TargetV = InitV + 2 * accel * (steps - 1);
    S32 interval = ceil((CalcSqrt(TargetV) - CalcSqrt(Last_TargetV)) * 1000000 / (accel * TIMEOUT_US));
    return interval;
}

#endif

/**
 * 函  数  名：CalculateInterval
 * 函数功能: 计算给定输入速度下，对应的定时器频率和对应频率下执行的步数
 * 函数参数: StepRate:输入的步频速度，单位step/s
 * 函数参数: loop:输出在对应频率下执行的步数 ，单位：step
 * 返回值: 步进电机定时器的中断周期tick数，单位tick
 * 备注：
 * 步频速度：速度（mm/s）× steps_per_mm（step/mm）
 * 换个视角，可以写成：f（1/s,即Hz）× steps(step)
 * 前者是用来计算步频的公式；后者可用来执行步频的公式。后者的意义是：每秒执行1次，1次执行steps步。
 * 保持步频速度不变，可以通过提高执行频率，降低一次执行的步数；反之，提高一次执行的步数，降低执行的频率。
 */
static S32 CalculateInterval(U32 StepRate, U32 &loop)
{
#if 0
    U32 MultiStep = 1;

    U32 RetTimer = 0;

    U32 i = 0;

    //PRINTF_LOGD("StepRate = %d, loop = %d",StepRate,loop);

    for(i = 0; (i < 7) && (StepRate > FreqLimit[i]); i++)
    {
        StepRate >>= 1;

        MultiStep <<= 1;

        //PRINTF_LOGD("StepRate = %d, MultiStep = %d",StepRate,MultiStep);
    }

    if(StepRate > FreqLimit[0])
    {
        StepRate = FreqLimit[0];
    }

    loop = MultiStep;

    RetTimer = STEPPER_TIMER_RATE / StepRate;

//    RetTimer = MAX(RetTimer,100);

    return RetTimer;

#else

    U32 RetTimer = 0;

    RetTimer = STEPPER_TIMER_RATE / StepRate;

    loop = 1;

    return RetTimer;

#endif
}


/**
 * 函数功能: 数据内卷,Num1 + Num2超过最大值Max后，数据重0清算
 * 函数参数: Num1:数据,Num2:数据,Max:内卷的最大值
 * 输入: Num1:数据,Num2:数据,Max:内卷的最大值
 * 输出: 数据内卷后的结果
 * 返回值: 数据内卷后的结果
 */
S32 OverFlowAddS32(S32 Num1, S32 Num2, S32 Max)
{
    if (Max - Num1 >= Num2)
        return Num1 + Num2;
    else
        return (Num1 + Num2) % Max;
}
/**
 * 函数功能: 判断数据是否溢出Src
 * 函数参数: Dest:被判断数据,Src:溢出标准数据
 * 输入: Dest:被判断数据,Src:溢出标准数据
 * 输出: 是否溢出
 * 返回值: 1:溢出,0:未溢出
 */
BOOL IsOverFlowS32(S32 Dest, S32 Src)
{
    if (Dest >= Src)
        return FALSE;
    else
        return TRUE;
}
/**
 * 函数功能: 虚函数实例化,无实际意义
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID Bresenham::Calc()
{
    printf("function: %s is NULL\n",__FUNCTION__);
    return;
}
/**
 * 函数功能: 丢弃当前计算中的直线运动
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID Bresenham::DiscardCurrentLine()
{
    Substance.Line.Usable = FALSE;

}
/**
 * 函数功能: 判断当前直线运动步数的Tick是否计算完成
 * 函数参数: 无
 * 输入: 无
 * 输出: 当前直线运动步数的Tick是否计算完成
 * 返回值: 1:完成,0:未完成
 */
BOOL Bresenham::IsCurrentLineComplete()
{
    return (Substance.Line.Usable == FALSE);
}
/**
 * 函数功能: 计算直线运动步数的Tick
 * 函数参数: Line:直线运动缓存队列,Ticks:计算结果缓存,CalcNum:所需计算最多轴的步数
 * 输入: Line:直线运动缓存队列,CalcNum:所需计算步数
 * 输出: Ticks:计算结果缓存
 * 返回值: 实际计算最多轴的步数
 */
#if 0
S32 Bresenham::Calc(struct PlanLine_t Line[], S32 Ticks[][CALC_TICK_NUM], U32 CalcNum)
{
    S32 CalcCount = 0;
    static U32 AxisStepCount[AxisNum] = {0};
    static enum CrMotionStatus_t Status = STOP;
    while( !(IsPlanLineBuffEmpty(Line) && Substance.Line.Usable == FALSE))
    {
        /* 检查是否有空间存储Ticks.*/
        LOOP_XYZE(i)
        {
            if ( Ticks[i][CALC_TICK_NUM - 1] != 0)
            {
                return CalcCount;
            }
        }
    
    /* if current line is consumed,take next line.*/
    if ( Substance.Line.Usable == FALSE )
    {
        /* if next line is NULL,nothing to do and return.*/
        if (GetNextPlanLine(&Substance.Line) <= 0)
        {
            return 0;/* No Move Commond,return.*/
        }
        else
        {
            /*找到最长轴*/
            Substance.LongestAxis = X;
            U32 MaxSteps = 0;
            LOOP_XYZE(i)
            {
                if (MaxSteps < Substance.Line.Axis[i].Steps )
                {
                    Substance.LongestAxis = (enum Axis_t)(i);
                    MaxSteps = Substance.Line.Axis[i].Steps;
                }
            }
#ifndef NEW_BRESENHAM_ENABLE
            /*计算相对长轴的斜率*/
            LOOP_XYZE(i)
            {
                Substance.Deviation[i] = 0.0f;
                Substance.Slop[i] = (FLOAT)(Substance.Line.Axis[i].Steps) / Substance.Line.Axis[Substance.LongestAxis].Steps;
            }
#else
            LOOP_XYZE(i)
            {
                Substance.NewDivident[i] = Substance.Line.Axis[i].Steps << 1;       //各分轴步数 wangcong
                //rt_kprintf("Substance.Divident[%d]=%d\r\n",i,Substance.Divident[i]);
                Substance.NewDeltaErr[i] = -MaxSteps;                          //wangcong
            }

            Substance.NewDevision = MaxSteps << 1;                                  //设置被除数，wangcong
#endif

            /* reset count,when each linear move start.*/
            Substance.StepsCount = 0;
            memset(AxisStepCount, 0, sizeof(AxisStepCount));
            Status = Substance.Line.PlanCalc.EnterStatus;
            if ( Substance.Line.Axis[Substance.LongestAxis].Steps > 0)
                Substance.Line.Usable = TRUE;
            else
                Substance.Line.Usable = FALSE;
        }
    }

    if ( Substance.Line.Usable == TRUE )
    {
        struct PlanLine_t *block = &Substance.Line;
        S32 Interval = 0;
        switch(Status)
        {
            case STOP:
                {
                    LOOP_XYZE(i)
                    {   /* correct the last step.*/
                        if (block->Axis[i].Steps > AxisStepCount[i])
                        {
                            U32 n = 0;
                            for (; n < CALC_TICK_NUM && Ticks[i][n] != 0; n++);

                            Ticks[i][n] = Substance.Line.Axis[i].Dir * CumulatePeriod[i];
                            AxisStepCount[i]++;
                        }
                    }
                    /*判断电机所走步数是否正确*/
//                    LOOP_XYZE(i)
//                    {
//                        if (block->Axis[i].Steps != AxisStepCount[i])
//                        {
//                            rt_kprintf("Count: %d \n",Substance.StepsCount);
//                            rt_kprintf("err: %d %d %d %d\n",AxisStepCount[0],AxisStepCount[1],AxisStepCount[2],AxisStepCount[3]);
//                            rt_kprintf("Real: %d %d %d %d\n",Substance.Line.Axis[X].Steps,Substance.Line.Axis[Y].Steps, Substance.Line.Axis[Z].Steps, Substance.Line.Axis[E].Steps);
//                        }
//                    }
                    block = NULL;
                    /* 当前线段ticks计算完成,可取下一条进行计算.*/
                    Substance.Line.Usable = FALSE;
                    return CalcCount;
                }
            break;
            case ACCELERATE:
                {
                    Substance.StepsCount ++;
                    //Interval = CalculateInterval(block->PlanCalc.Acceleration, block->PlanCalc.EntrySpeed, Substance.StepsCount);
                    if((ABS(block->PlanCalc.EntryRate - MINIMUM_PLANNER_SPEED) < 0.5f))
                    {
                        break;
                    }

                    Interval = CalculateInterval(block->PlanCalc.AccelStepPerS2, block->PlanCalc.EntryRateSqr, Substance.StepsCount);  //单位step/s step/s^2
                    if (Substance.StepsCount >= block->Axis[Substance.LongestAxis].Steps)
                    {
                        Status = STOP;
                    }
                    else
                    {
                        if(block->Axis[Substance.LongestAxis].UniformStartStep != 0 && Substance.StepsCount >= block->Axis[Substance.LongestAxis].UniformStartStep)
                        {
                            Status = UNIFORM;
                        }
                        if(block->Axis[Substance.LongestAxis].DecelStartStep != 0 && Substance.StepsCount >= block->Axis[Substance.LongestAxis].DecelStartStep)
                        {
                            Status = DECELERATE;
                        }
                    }
                }
            break;
            case UNIFORM:
                {
                    Substance.StepsCount ++;
                    /* calculate the count of delay by MaxSpeed.*/
//                    Interval = 1000000 / (block->PlanCalc.MaxSpeed * TIMEOUT_US);
                    Interval = 1000000 / (block->PlanCalc.NominalRate * TIMEOUT_US);
                    if (Substance.StepsCount >= block->Axis[Substance.LongestAxis].Steps)
                    {
                        Status = STOP;
                    }
                    if(block->Axis[Substance.LongestAxis].DecelStartStep != 0 && 
                        Substance.StepsCount >= block->Axis[Substance.LongestAxis].DecelStartStep)
                    {
                        Status = DECELERATE;
                    }
                }
            break;
            case DECELERATE:
                {
                    Substance.StepsCount ++;
                    /* The critical value cannot take the equal sign, otherwise the last step cannot be taken.*/
                    if(Substance.StepsCount >= block->Axis[Substance.LongestAxis].Steps)
                    {
                        Status = STOP;
                        continue;
                    }
//                    Interval = CalculateInterval(block->PlanCalc.Acceleration, block->PlanCalc.ExitSpeed,
//                                                    block->Axis[Substance.LongestAxis].Steps - Substance.StepsCount);
                    Interval = CalculateInterval(block->PlanCalc.AccelStepPerS2, block->PlanCalc.ExitRateSqr,
                                                                        block->Axis[Substance.LongestAxis].Steps - Substance.StepsCount);
                }
            break;
        }
        CalcCount++;

        Interval = MAX(Interval, 1);
        
        /* Save period count to Ticks buffer.*/
        LOOP_XYZE(i)
        {
#ifndef NEW_BRESENHAM_ENABLE
            CumulatePeriod[i] = OverFlowAddS32(CumulatePeriod[i], Interval, INT_MAX);
            if(Substance.Deviation[i] + Substance.Slop[i] < 0.5f)
            {
                Substance.Deviation[i] += Substance.Slop[i];
            }
            else
            {
                Substance.Deviation[i] = Substance.Deviation[i] + Substance.Slop[i] - 1.0f;
                int n = 0;
                /*找到Ticks存储在数组中的第一个0的位置.*/
                for (; n < CALC_TICK_NUM && Ticks[i][n] != 0; n++);

                AxisStepCount[i]++;
                Ticks[i][n] = Substance.Line.Axis[i].Dir * CumulatePeriod[i];
            }
#else
            CumulatePeriod[i] = OverFlowAddS32(CumulatePeriod[i], Interval, INT_MAX);

            if(SyncAlgorithm.BresenhamCalc(i))
            {
                int n = 0;
                /*找到Ticks存储在数组中的第一个0的位置.*/
                for (; n < CALC_TICK_NUM && Ticks[i][n] != 0; n++);

                AxisStepCount[i]++;
                Ticks[i][n] = Substance.Line.Axis[i].Dir * CumulatePeriod[i];
            }
#endif
        }

    }
    }
    return CalcCount;
}

/**
 * 函数功能: 将计算出的Tick结果放入电机的缓存中
 * 函数参数: Ticks:计算结果缓存,TicksNum:每个轴缓存的最大Tick数量,AxisFifo:轴电机Tick缓存数组
 * 输入: Ticks:计算结果缓存,TicksNum:每个轴缓存的最大Tick数量,AxisFifo:轴电机Tick缓存数组
 * 输出: 无
 * 返回值: 实际放入电机运动Tick缓存的最少的步数
 */
U32 Bresenham::Put2TickBuffer(S32 Ticks[][CALC_TICK_NUM], S32 TicksNum, struct Fifo_t *AxisFifo)
{
    U32 WriteFlag = 0,Count = 0;;
    for  (int j = 0; j < TicksNum; j++)
    {
//        LOOP_XYZE(i)
//        {
//            /* stuff one of tick to Fifo buffers for each axis.*/
//            if(Ticks[i][j] != 0 && WriteFifoEx((struct Fifo_t *)&AxisFifo, &Ticks[i][j], sizeof(Ticks[i][j])) == 0)
//            {
//                return j;
//            }
//            /* clear data.*/
//            if ( Ticks[i][j] != 0)
//            {
//                Ticks[i][j] = 0;
//                WriteFlag = 1;
//            }
//        }
        if (WriteFlag != 0)
        {
            Count ++;
            WriteFlag = 0;
        }
    }
    return Count;
}
#else

S32 Bresenham::Calc(struct PlanLine_t Line[], U32 MotionNum)
{

//#define LOOP_INDEX_MAX      20

    static U32 AxisStepCount[AxisNum] = {0};            //用于存放各轴在一个Block中的实际走动的步数

    static enum CrMotionStatus_t Status = STOP;

    static U16 MotionDir = 0;

    static U32 AccCompletedTime = 0;

    static U32 DecCompletedTime = 0;

    static U32 AxisStepsCnt[AxisNum] = {0};

    S32 WriteRet = 0;

    U32 LocalMotionNum = MotionNum;

    U32 StepRate = 0;

    U32 StepInterval = 0;

    U32 StepPerIsr = 0;

    U32 TempSub = 0;

    U32 LoopIndex = 0;

    U32 MaxSteps = 0;

    S32 Ret = -1;

    MotionPeriod_t MotionPeriod;

//    rt_kprintf("LocalMotionNum = %d\r\n",LocalMotionNum);

    while( !(IsPlanLineBuffEmpty(Line) && Substance.Line.Usable == FALSE) && (LoopIndex != LocalMotionNum))
    {
        //rt_kprintf("1 Substance.Line.Usable = %d\r\n",Substance.Line.Usable);
        /* if current line is consumed,take next line.*/
        if ( Substance.Line.Usable == FALSE )
        {
            /* if next line is NULL,nothing to do and return.*/
            if (GetNextPlanLine(&Substance.Line) <= 0)
            {
                return -1;/* No Move Commond,return.*/
            }
            else
            {
                /*找到最长轴*/
                Substance.LongestAxis = X;

                LOOP_XYZE(i)
                {
                    if (MaxSteps < Substance.Line.Axis[i].Steps )
                    {
                        Substance.LongestAxis = (enum Axis_t)(i);

                        MaxSteps = Substance.Line.Axis[i].Steps;
                    }

                    AxisStepsCnt[i] = 0;                        //20220210,在每个新线段开始时，对其初始化
                }

//                rt_kprintf("MaxSteps[%d]=%d\r\n",Substance.LongestAxis,MaxSteps);

    #ifndef NEW_BRESENHAM_ENABLE
                /*计算相对长轴的斜率*/
                LOOP_XYZE(i)
                {
                    Substance.Deviation[i] = 0.0f;
                    Substance.Slop[i] = (FLOAT)(Substance.Line.Axis[i].Steps) / Substance.Line.Axis[Substance.LongestAxis].Steps;
                }
    #else
                LOOP_XYZE(i)
                {
                    Substance.NewDivident[i] = Substance.Line.Axis[i].Steps << 1;       //各分轴步数
//                    rt_kprintf("Substance.NewDivident[%d]=%d\r\n",i,Substance.NewDivident[i]);
                    Substance.NewDeltaErr[i] = -MaxSteps;
                }

                Substance.NewDevision = MaxSteps << 1;                                  //设置被除数
    #endif
                MotionDir = 0;

                LOOP_XYZE(i)            //获取方向。每个新Block公用一个方向
                {
//                    rt_kprintf("Axis[%d].Dir=%d\r\n",i,Substance.Line.Axis[i].Dir);

                    if(Substance.Line.Axis[i].Dir == FORWARD)
                    {
                        MotionDir |= 0x01 << (7 - i);
                    }
                }

                MotionDir = MotionDir << 8;
//                rt_kprintf("MotionDir=%d\r\n",MotionDir);

                /* reset count,when each linear move start.*/
                Substance.StepsCount = 0;

                memset(AxisStepCount, 0, sizeof(AxisStepCount));

                Status = Substance.Line.PlanCalc.EnterStatus;

                if ( Substance.Line.Axis[Substance.LongestAxis].Steps > 0)
                {
                    Substance.Line.Usable = TRUE;
                }
                else
                {
                    Substance.Line.Usable = FALSE;
                }
            }
        }

        //rt_kprintf("2 Substance.Line.Usable = %d\r\n",Substance.Line.Usable);

        if ( Substance.Line.Usable == TRUE )
        {
//            EndStopHandler();

            struct PlanLine_t *block = &Substance.Line;

            switch(Status)
            {
                case STOP:
                    {
//                        //rt_kprintf("STOP\r\n");

//                        MotionPeriod_t MotionPeriodLocal = {0};
//
//                        MotionPeriodLocal.StepPeriod = StepInterval;
//
//                        MotionPeriodLocal.MotionFlag = MotionDir << 8;
//
//                        do
//                        {
//                            LOOP_XYZE(i)
//                            {
//                                if(AxisStepsCnt[i] < block->Axis[i].Steps)
//                                {
//                                    MotionPeriodLocal.MotionFlag |= (0x01 << (7 - i));
//
//                                    AxisStepsCnt[i]++;
//
//                                    //rt_kprintf("AxisStepsCnt[%d] = %d/%d\r\n",i,AxisStepsCnt[i],block->Axis[i].Steps);
//                                }
//                            }
//
//                            if((MotionPeriodLocal.MotionFlag & 0x0FF) != 0)   //存在没有走完的情况
//                            {
//                                for(;;)
//                                {                     //存在写入失败的情况，所以通过此方法确保写成功
//
//                                    WriteRet = CommonWriteFifo(&AxisFifo, &MotionPeriodLocal,1);
//
//                                    if(WriteRet != 1)
//                                    {
//                                        CrM_Sleep(25);
//                                    }
//                                    else
//                                    {
//                                        break;
//                                    }
//                                }
//                            }
//                        }while((MotionPeriodLocal.MotionFlag & 0x0FF) != 0);        //存在没有走完的情况

//                        LOOP_XYZE(i)
//                        {
//                            if(AxisStepsCnt[i] != block->Axis[i].Steps)
//                            {
//                                rt_kprintf("AxisStepsCnt[%d] = %d/%d\r\n",i,AxisStepsCnt[i],block->Axis[i].Steps);
//                            }
//                        }

                        block = NULL;
                        /* 当前线段ticks计算完成,可取下一条进行计算.*/
                        Substance.Line.Usable = FALSE;

                        AccCompletedTime = 0;                   //加速时间复位

                        DecCompletedTime = 0;                   //减速时间复位

                        Substance.StepsCount = 0;               //当前线段累计走过的步数。当一条线段运行完毕，清零

                        MotionDir = 0;

                        return 0;
                    }
                break;
                case ACCELERATE:
                    {
                        //rt_kprintf("ACCELERATE\r\n");
                        //Substance.StepsCount ++;
                        //Interval = CalculateInterval(block->PlanCalc.Acceleration, block->PlanCalc.EntrySpeed, Substance.StepsCount);
//                        if((ABS(block->PlanCalc.EntryRate - MINIMUM_PLANNER_SPEED) < 0.5f))
//                        {
//                            break;
//                        }

                        StepRate = MultiU32X24toH32(AccCompletedTime, block->PlanCalc.AccelStepPerSecTick) + block->PlanCalc.EntryRate;

                        if(StepRate > block->PlanCalc.NominalRate)
                        {
                            StepRate = block->PlanCalc.NominalRate;
                        }

                        StepRate = MAX(StepRate, MIN_STEP_RATE);

                        //计算经过修正后的步频速度StepPerIsr和与之对应的步进定时器周期
                        StepInterval = CalculateInterval(StepRate, StepPerIsr);

                        AccCompletedTime += StepInterval;       //统计加速时间，用于计算下个加速点的速度

                        TempSub =  block->PlanCalc.StepEventCnt - Substance.StepsCount;

                        if(StepPerIsr < TempSub)
                        {
                            Substance.StepsCount += StepPerIsr;     //保持原来的参数不变
                        }
                        else
                        {
                            Substance.StepsCount += TempSub;

                            StepPerIsr = TempSub;
                        }

                        if (Substance.StepsCount >= block->Axis[Substance.LongestAxis].Steps)
                        {
                            Status = STOP;
                        }
                        else
                        {
                            if(block->Axis[Substance.LongestAxis].UniformStartStep != 0 && Substance.StepsCount >= block->Axis[Substance.LongestAxis].UniformStartStep)
                            {
                                Status = UNIFORM;
                            }
                            if(block->Axis[Substance.LongestAxis].DecelStartStep != 0 && Substance.StepsCount >= block->Axis[Substance.LongestAxis].DecelStartStep)
                            {
                                Status = DECELERATE;
                            }
                        }
                    }
                break;
                case UNIFORM:
                    {
                        //rt_kprintf("UNIFORM\r\n");

                        StepRate = block->PlanCalc.NominalRate;

                        StepRate = MAX(StepRate, MIN_STEP_RATE);

                        StepInterval = CalculateInterval(StepRate, StepPerIsr);

                        TempSub = block->PlanCalc.StepEventCnt - Substance.StepsCount;

                        if(StepPerIsr < TempSub)
                        {
                            Substance.StepsCount += StepPerIsr;     //保持原来的参数不变
                        }
                        else
                        {
                            Substance.StepsCount += TempSub;

                            StepPerIsr = TempSub;
                        }

                        if (Substance.StepsCount >= block->Axis[Substance.LongestAxis].Steps)
                        {
                            Status = STOP;
                        }

                        if(block->Axis[Substance.LongestAxis].DecelStartStep != 0 && Substance.StepsCount >= block->Axis[Substance.LongestAxis].DecelStartStep)
                        {
                            Status = DECELERATE;
                        }
                    }
                break;
                case DECELERATE:
                    {
                        //rt_kprintf("DECELERATE\r\n");

                        if(Substance.StepsCount >= block->Axis[Substance.LongestAxis].Steps)
                        {
                            Status = STOP;

                            break;
                        }

                        //以下速度公式为：v1 = at
                        StepRate = MultiU32X24toH32(DecCompletedTime, block->PlanCalc.AccelStepPerSecTick);

                        if(StepRate < block->PlanCalc.NominalRate)
                        {
                            StepRate = block->PlanCalc.NominalRate - StepRate;             //计算减速运动时的速度值

                            if(StepRate < block->PlanCalc.ExitRate)
                            {
                                StepRate = block->PlanCalc.ExitRate;                   //修正减速运动的速度值不得小于退出速度
                            }
                        }

                        StepRate = MAX(StepRate, MIN_STEP_RATE);

                        //计算经过修正后的步频速度StepPerIsr和与之对应的步进定时器周期
                        StepInterval = CalculateInterval(StepRate, StepPerIsr);

                        DecCompletedTime += StepInterval;

                        TempSub = block->PlanCalc.StepEventCnt - Substance.StepsCount;

                        if(StepPerIsr < TempSub)
                        {
                            Substance.StepsCount += StepPerIsr;     //保持原来的参数不变
                        }
                        else
                        {
                            Substance.StepsCount += TempSub;

                            StepPerIsr = TempSub;
                        }
                    }
                break;
            }


            MotionPeriod.StepPeriod = StepInterval;

            MotionPeriod.MotionFlag = MotionDir & 0xFF00;

//          rt_kprintf("MotionPeriod[j].MotionFlag = %d\r\n",MotionPeriod[j].MotionFlag);

            LOOP_XYZE(i)
            {
                if(SyncAlgorithm.BresenhamCalc(i) && AxisStepsCnt[i] < block->Axis[i].Steps)      //Bresenham算法计算各轴是否需要移动,且各轴运动的步数不得大于各分轴步数
                {
                    MotionPeriod.MotionFlag |= (0x01 << (7 - i));

                    AxisStepsCnt[i]++;                  //统计各轴移动的步数 20220210

                    if(AxisStepsCnt[i] == 1)
                    {
                        MotionPeriod.StepPeriod = MIN(StepInterval, 1000000 / 2000);
                    }
                }
            }

            Ret = 0;

            do
            {                     //存在写入失败的情况，所以通过此方法确保写成功

                WriteRet = CommonWriteFifo(&AxisFifo, &MotionPeriod,1);

                if(WriteRet != 1)
                {
                    Ret = 1;

                    CrM_Sleep(50); //50
                }
            }while(WriteRet != 1);

            LoopIndex++;
        }
    }

    return Ret; //返回值需要仔细考虑
}
#endif
//返回1，插入成功；其他值失败。
S32 Bresenham::Put2TickBuffer(CommonFiFo_t *AxisFifo, MotionPeriod_t MotionPeriod)
{
    return CommonWriteFifo(AxisFifo, &MotionPeriod,1);
}

#ifdef NEW_BRESENHAM_ENABLE
/**
 * 函  数  名：BresenhamCalc
 * 功能描述：执行布雷森汉姆算法，求出StepNeeded用以确定对应分轴是否需要步进
 * 参        数：Axis，坐标轴索引
 * 返  回  值：无
 * 备注：
 *
 * */
BOOL Bresenham::BresenhamCalc(U32 Axis)
{
    BOOL RetStepNeeded = 0;

    Substance.NewDeltaErr[Axis] += Substance.NewDivident[Axis];

    RetStepNeeded = !!(Substance.NewDeltaErr[Axis] >= 0);

    if (RetStepNeeded)
    {
        Substance.NewDeltaErr[Axis] -= Substance.NewDevision;
    }

    return RetStepNeeded;
}

#endif
