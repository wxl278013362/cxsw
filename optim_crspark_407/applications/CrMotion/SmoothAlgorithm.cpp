/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date            Author                 Notes
 * 2021-05-17     TingFengXuan       the first version
 */
#include "SmoothAlgorithm.h"
#include "Bresenham.h"

#include <stdio.h>
#include <math.h>
#include <rtthread.h>

//平滑算法实例化对象
ResultantSmooth Smooth;
/**
 * 函数功能: 计算开平方
 * 函数参数: Num:被开平方数据
 * 输入: Num:被开平方数据
 * 输出: 开平方结果
 * 返回值: 开平方结果
 */
static FLOAT CalcSqrt(FLOAT Num)
{
    if (Num < EOL_DATA && Num > -EOL_DATA)
        return 0;
    FLOAT Xhalf = 0.5f * Num;
    U32 i = *(U32*)&Num;
    i = 0x5f3759df - (i >> 1);
    Num = *(float *)&i;
    Num = Num * (1.5f - Xhalf * Num * Num);
    return (1.0f / Num);
}
/**
 * 函数功能: 调整向后扫描直线队列时的直线初速度
 * 函数参数: Current:当前计算直线, Next:下一条直线
 * 输入: Next:下一条直线
 * 输出: Current:当前直线
 * 返回值: 无
 */
static VOID ReverseAdjust(struct PlanLine_t *Current, struct PlanLine_t *Next)
{
    //注意：后继运动块的进入速度就是当前运动块的退出速度
    if(Current != NULL)
    {
        struct PlanLineResultant_t *CurPlanCalc = &Current->PlanCalc;
        struct PlanLineResultant_t *NextPlanCalc = NULL;
        if (Next != NULL)
        {
            NextPlanCalc = &Next->PlanCalc;
        }

        const FLOAT MaxEntrySpeedSqr = (FLOAT)SQR(CurPlanCalc->MaxEntrySpeed); //此处应该赋予最大进入速度

        if(CurPlanCalc->EntrySpeedSqr != MaxEntrySpeedSqr)
        {
            FLOAT MaxAllowSpeedSqr = max_allowable_speed_sqr(-(CurPlanCalc->Acceleration),
                                                        NextPlanCalc != NULL ? NextPlanCalc->EntrySpeedSqr : SQR((FLOAT)MINIMUM_PLANNER_SPEED)
                                                        , CurPlanCalc->LineLength);   //前后两个运动块合到一起计算的衔接速度 （SQR计算平方，CalcSqrt计算开平方）

            if (Current->Usable == TRUE)
            {
                CurPlanCalc->EntrySpeedSqr = MIN(MaxAllowSpeedSqr, MaxEntrySpeedSqr);

                CurPlanCalc->ExitSpeedSqr = NextPlanCalc != NULL? NextPlanCalc->EntrySpeedSqr : (FLOAT)SQR(MINIMUM_PLANNER_SPEED);
            }
        }
    }
    return ;
}
/**
 * 函数功能: 调整向前扫描直线队列时的直线初速度
 * 函数参数: Previous:前一个直线, Current:当前计算直线
 * 输入: Current:当前计算直线
 * 输出: Previous:前一个直线
 * 返回值: 无
 */
static VOID ForwardAdjust(struct PlanLine_t *Previous, struct PlanLine_t *Current)
{
    if ( Previous != NULL)
    {
        struct PlanLineResultant_t *CurPlanCalc = &Current->PlanCalc;
        struct PlanLineResultant_t *PrevPlanCalc = &Previous->PlanCalc;

        if(PrevPlanCalc->EntrySpeedSqr < CurPlanCalc->EntrySpeedSqr)
        {
            float NewEntrySpeedSqr = max_allowable_speed_sqr(-PrevPlanCalc->Acceleration,
                                            PrevPlanCalc->EntrySpeedSqr, PrevPlanCalc->LineLength);   //计算前继运动块的退出速度

            if((CurPlanCalc->EntrySpeedSqr > NewEntrySpeedSqr) && Current->Usable == TRUE)
            {
                CurPlanCalc->EntrySpeedSqr = NewEntrySpeedSqr;
            }
        }

        if (Previous->Usable == TRUE)
        {
            PrevPlanCalc->ExitSpeedSqr = Current != NULL ? CurPlanCalc->EntrySpeedSqr : SQR(MINIMUM_PLANNER_SPEED);
        }
    }
    return;
}
/**
 * 函数功能: 计算加速步数（计算运动块中梯形运动有匀速运动时加速运动的运动距离即加速阶段初速度、末速度和加速度均已知的）
 * 函数参数: initial_rate:初速度,target_rate:目标速度,accel:加速度
 * 输入: initial_rate:初速度,target_rate:目标速度,accel:加速度
 * 输出: 参考返回值
 * 返回值: 加速步数
 */
static float AccelerationDistance(const float &initial_rate_sqr, const float &target_rate_sqr, const float &accel)
{
  if (accel <= EOL_DATA && accel >= -EOL_DATA) 
      return 0; // accel was 0, set acceleration distance to 0
  return (target_rate_sqr - initial_rate_sqr) / (accel * 2);
}
/**
 * 函数功能: 计算交点步数(直线运动的距离不能到达最大速度时,无匀速阶段,加速后开始减速的点这里称为交点)(计算运动块中无匀速运动时加速运动的运动距离)
 * 函数参数: initial_rate:初速度,final_rate:末速度,accel:加速度，distance:直线距离
 * 输入: initial_rate:初速度,final_rate:末速度,accel:加速度，distance:直线距离
 * 输出: 参考返回值
 * 返回值: 交点步数
 */
static float IntersectionDistance(const float &initial_rate_sqr, const float &final_rate_sqr, const float &accel, const float &distance)
{
    if (accel <= EOL_DATA && accel >= -EOL_DATA) 
        return 0; // accel was 0, set intersection distance to 0
    return (accel * 2 * distance - initial_rate_sqr + final_rate_sqr) / (accel * 4);
}
/**
 * 函数功能: 计算直线运动参数(加速步数、减速步数、匀速步数等长轴运动步数信息)
 * 函数参数: Line:直线
 * 输入: Line:直线
 * 输出: Line:直线
 * 返回值: 无
 */

VOID CalculateForLine(struct PlanLine_t *Line)
{
    struct PlanLineResultant_t *LineCalc = &Line->PlanCalc;
    U32 EventSteps = Line->Axis[X].Steps;   //运动的总步数（实在运动块进入运动队列时就计算好了，长轴的运动步数）
    U32 LongestAxis = X;
    for(int i = X; i < AxisNum; i++)
    {
        if (EventSteps < Line->Axis[i].Steps)
        {
            EventSteps = Line->Axis[i].Steps;
            LongestAxis = i;
        }
    }

    FLOAT StepPerMm = (FLOAT)(EventSteps) / LineCalc->LineLength;   //长轴每一个mm走多少步。

    FLOAT EntryRateSqr = SQR(StepPerMm) * LineCalc->EntrySpeedSqr;
    FLOAT ExitRateSqr = SQR(StepPerMm) * LineCalc->ExitSpeedSqr;

    EntryRateSqr = MIN(EntryRateSqr,SQR(LineCalc->NominalRate));
    ExitRateSqr = MIN(ExitRateSqr,SQR(LineCalc->NominalRate));

    U32 AccelerateSteps = (S32)ceil(AccelerationDistance(EntryRateSqr, SQR(LineCalc->NominalRate), LineCalc->AccelStepPerS2));   //计算梯形运动中有匀速运动时加速阶段的步数
    U32 DecelerateSteps = (S32)floor(AccelerationDistance(SQR(LineCalc->NominalRate), ExitRateSqr, -LineCalc->AccelStepPerS2));  //计算梯形运动中有匀速运动时减速阶段的步数

    S32 PlateauSteps = EventSteps - AccelerateSteps - DecelerateSteps;   //运动运动的步数

    if (PlateauSteps < 0)   //运动过程中没有匀速阶段
    {
        const FLOAT AccelStepsFloat = IntersectionDistance(EntryRateSqr, ExitRateSqr, LineCalc->AccelStepPerS2, EventSteps);  //计算加速阶段的步数

        AccelerateSteps = MIN((U32)(MAX(AccelStepsFloat, 0)), EventSteps);
        PlateauSteps = 0;
    }

    LineCalc->EnterStatus = ACCELERATE;
    Line->Axis[LongestAxis].DecelStartStep = MAX(AccelerateSteps + PlateauSteps, 0);  //减速阶段开始的步数索引
    Line->Axis[LongestAxis].UniformStartStep = AccelerateSteps;    //匀速阶段开始的步数索引


    LineCalc->EntryRateSqr = EntryRateSqr;
    LineCalc->ExitRateSqr = ExitRateSqr;

//    rt_kprintf("LineCalc->EntryRateSqr=%d\r\n",LineCalc->EntryRateSqr);
//    rt_kprintf("LineCalc->ExitRateSqr=%d\r\n",LineCalc->ExitRateSqr);

    return;
}

static VOID TrapezoidCalculate(struct PlanLine_t *Line)
{
    U32 LineBuffIndex = LineBuffHead,
        TailLineIndex = LineBuffTail;
    struct PlanLine_t *Current = NULL;
    while( TailLineIndex != LineBuffIndex )
    {
        Current = &Line[LineBuffIndex];
        if (Current->Usable == TRUE)
        {
            Current->Usable = FALSE;
            CalculateForLine(Current);    //梯形运动的步数计算
            Current->Usable = TRUE;
        }
        LineBuffIndex = NextPlanLineIndex(LineBuffIndex);
    }

    return;
}
/**
 * 函数功能: 向后扫描直线队列，计算合适的初速度 （环形队列）
 * 函数参数: LineBuff:直线队列
 * 输入: LineBuff:直线队列
 * 输出: LineBuff:直线队列
 * 返回值: 无
 */
VOID ResultantSmooth::ReverseScan(struct PlanLine_t *LineBuff)
{
    U32 LineBuffIndex = PrevPlanLineIndex(LineBuffTail);
    struct PlanLine_t *Next = NULL;
    struct PlanLine_t *Current = NULL;
    while( LineBuffIndex != SmoothIndex)
    {
        Current = &LineBuff[LineBuffIndex];

        ReverseAdjust(Current, Next);    //调整运动块的进入速度

        Next = Current;

        LineBuffIndex = PrevPlanLineIndex(LineBuffIndex);
        /* this line is consumed.*/
        if(LineBuff[LineBuffIndex].Usable != TRUE)
            return;
    }
}
/**
 * 函数功能: 向前扫描直线队列，计算合适的初速度 （环形队列）
 * 函数参数: LineBuff:直线队列
 * 输入: LineBuff:直线队列
 * 输出: LineBuff:直线队列
 * 返回值: 无
 */
VOID ResultantSmooth::ForwardScan(struct PlanLine_t *LineBuff)
{
    U32 LineBuffIndex = SmoothIndex;
    struct PlanLine_t *Current = NULL;
    struct PlanLine_t *Previous = NULL;
    while(LineBuffIndex != LineBuffTail)
    {
        /**
         * TO DO:
         */
        Current = &LineBuff[LineBuffIndex];
        if (Current->Usable == TRUE)
        {
            if (Previous != NULL)
            {
                ForwardAdjust(Previous, Current);   //调整运动块的退出速度
            }
            Previous = Current;
        }
        else
        {
            SmoothIndex = LineBuffIndex;
        }
        LineBuffIndex = NextPlanLineIndex(LineBuffIndex);
    }
}
//输入输出都是PlanLineFifo
VOID ResultantSmooth::SmoothRecalc(struct PlanLine_t *LineBuff)
{
    U32 PlanLineIndex = PrevPlanLineIndex(LineBuffTail);  //获取LineBuffTail的前一个索引
    if (PlanLineIndex != SmoothIndex)   //开始平滑规划的LineBuf索引和
    {
        ReverseScan(LineBuff);   //调整运动块的进入速度
        ForwardScan(LineBuff);   //调整运动块的退出速度
    }
    TrapezoidCalculate(LineBuff);  //重新计算运动块的步数
}
VOID ResultantSmooth::SmoothRecalc()
{

}

