/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date            Author                 Notes
 * 2021-04-30     TingFengXuan       the first version
 */
#include "CrMotion.h" //引入CrMotion::Deltamm[AxisNum]

#include "CrMotionPlanner.h"
#include "CrStepper.h"
#include "Bresenham.h"
#include "SmoothAlgorithm.h"
#include "../Include/CrSleep.h"

#include <string.h>
#include <stdio.h>
#include <rtthread.h>
#include <math.h>

//轴电机的描述符
static S32 Axis[AxisNum] = {0};
//直线队列数组
struct PlanLine_t LineBuff[BUFF_MAX_SIZE] = {0};
//直线队列的头索引
volatile U32 LineBuffHead = 0;
//直线队列的尾索引
volatile U32 LineBuffTail = 0;
//速度属性参数
static struct FeedrateAttr_t Feedrate = {0};
/**
 * 函数功能: 获取目标索引的后一个直线索引
 * 函数参数: Index:目标索引
 * 输入: Index:目标索引
 * 输出: 目标索引的后一个直线索引
 * 返回值: 目标索引的后一个直线索引
 */
U32 NextPlanLineIndex(U32 Index)
{
    return ((Index + 1) & (BUFF_MAX_SIZE - 1));
}
/**
 * 函数功能: 获取目标索引的前一个直线索引
 * 函数参数: Index:目标索引
 * 输入: Index:目标索引
 * 输出: 目标索引的前一个直线索引
 * 返回值: 目标索引的前一个直线索引
 */
U32 PrevPlanLineIndex(U32 Index)
{
    return ((Index - 1) & (BUFF_MAX_SIZE - 1));
}
/**
 * 函数功能: 计算开平方
 * 函数参数: Num:被开平方数据
 * 输入: Num:被开平方数据
 * 输出: 开平方结果
 * 返回值: 开平方结果
 */
static FLOAT CalcSqrt(FLOAT Num)
{
    if (Num >= -EOL_DATA && Num <= EOL_DATA)
        return 0;
    FLOAT Xhalf = 0.5f * Num;
    U32 i = *(U32 *)&Num;
    i = 0x5f3759df - (i >> 1);
    Num = *(float *)&i;
    Num = Num * (1.5f - Xhalf * Num * Num);
    return (1.0f / Num);
}
/**
 * 函数功能: 设置速度属性参数
 * 函数参数: FrAttr:速度参数地址指针
 * 输入: FrAttr:速度参数地址指针
 * 输出: 无
 * 返回值: 无
 */
VOID FeedrateAttrSet(struct FeedrateAttr_t *FrAttr)
{
    memcpy(&Feedrate, FrAttr, sizeof(struct FeedrateAttr_t));
}
/**
 * 函数功能: 获取速度属性参数
 * 函数参数: FrAttr:速度参数地址指针
 * 输入: 无
 * 输出: FrAttr:速度参数地址指针
 * 返回值: 无
 */
VOID FeedrateAttrGet(struct FeedrateAttr_t *FrAttr)
{
    memcpy(FrAttr, &Feedrate, sizeof(struct FeedrateAttr_t));
}
/**
 * 函数功能: 计算最大允许进入速度的平方（末速度、加速度和距离均已知的情况，只做匀加速运动或匀减速运动，此函数即计算初速度的平方）
 * 函数参数: accel:加速度,target_velocity_sqr:最大速度,distance:运动距离
 * 输入: accel:加速度,target_velocity_sqr:最大速度,distance:运动距离
 * 输出: 最大允许进入速度
 * 返回值: 最大允许进入速度
 */
FLOAT max_allowable_speed_sqr(const FLOAT accel, const FLOAT target_velocity_sqr, const FLOAT distance)
{
    return target_velocity_sqr - 2 * accel * distance;
}
/**
 * 函数功能: 初始化电机轴的Fd
 * 函数参数: AxisFd:电机轴Fd数组,AxisNum:轴数量
 * 输入: AxisFd
 * 输出: 无
 * 返回值: 无
 */
VOID PlannerInit(S32 AxisFd[], U32 AxisNum)
{
    for (int i = 0; i < AxisNum; i++)
    {
        Axis[i] = AxisFd[i];
    }
}

/**
 * 函数功能: 初步计算直线运动所需基本参数.
 * 函数参数: Delta:目标坐标与当前坐标差,Block:直线参数缓存地址
 * 输入: Delta
 * 输出: Block
 * 返回值: 0:成功,小于0:失败
 */
U32 StuffLineOKFlag = 0;
static S32 StuffLineBuff(struct Coordinate3d_t *Delta, struct PlanLine_t * const Block, BOOL Segment)
{
    if ( IsPlanLineBuffFull(LineBuff) )
        return -1;/* queue is full.*/
    
    if(!Segment)
    {
        StuffLineOKFlag = 1;
        rt_kprintf("%s\n", MSG_OK); //20211227预处理完即可认为入队成功
    }

    FLOAT delta_XYZE[] = {Delta->X, Delta->Y, Delta->Z, Delta->E};
    U32 AxisStepPerMm[AxisNum] = {0};
    U32 AxisAccelStepPerS2[AxisNum] = {0};
    /* Compute motion attributes for each axis.*/
    struct StepperAttr_t Attr;
    struct StepperMotionAttr_t MotionAttr;
    LOOP_XYZE(i)
    {
        memset(&Attr, 0, sizeof(struct StepperAttr_t));
        memset(&MotionAttr, 0, sizeof(struct StepperMotionAttr_t));
        if (CrStepperIoctl(Axis[i], CRSTEPPER_GET_ATTR, &Attr) < 0)
        {
            return -1;
        }
        if ( CrStepperIoctl(Axis[i], CRSTEPPER_GET_MOTION_ATTR, &MotionAttr) < 0)
        {
            return -1;
        }
        AxisStepPerMm[i] = Attr.StepsPerMm;
        Block->Axis[i].MaxSpeed = MotionAttr.MaxVelocity;
        Block->Axis[i].MaxJerk = MotionAttr.Jerk;
        Block->Axis[i].MaxAccel = MotionAttr.Acceleration;
        AxisAccelStepPerS2[i] = Block->Axis[i].MaxAccel * AxisStepPerMm[i];         //计算分轴的加速度 step/s^2

//        rt_kprintf("Block->Axis[%d].MaxSpeed:%d\r\n",i,Block->Axis[i].MaxSpeed);
//        rt_kprintf("Block->Axis[%d].MaxJerk:%d\r\n",i,Block->Axis[i].MaxJerk);
//        rt_kprintf("Block->Axis[%d].MaxAccel:%d\r\n",i,Block->Axis[i].MaxAccel);
    }
    /* Compute direction for each axis.*/
    Block->Axis[X].Dir = Delta->X < EOL_DATA ? BACKWARD : FORWARD;
    Block->Axis[Y].Dir = Delta->Y < EOL_DATA ? BACKWARD : FORWARD;
    Block->Axis[Z].Dir = Delta->Z < EOL_DATA ? BACKWARD : FORWARD;
    Block->Axis[E].Dir = Delta->E < EOL_DATA ? BACKWARD : FORWARD;

    Block->PlanCalc.IsEndLine = TRUE;

#if 0
    Block->Axis[X].Steps = ROUND(ABS(Delta->X) * AxisStepPerMm[X]);
    Block->Axis[Y].Steps = ROUND(ABS(Delta->Y) * AxisStepPerMm[Y]);
    Block->Axis[Z].Steps = ROUND(ABS(Delta->Z) * AxisStepPerMm[Z]);
    /* apply flow.*/
    FLOAT FlowRatio = Feedrate.FlowRatio / 100.0;   //因为流量比百分比，不直接就是一个大于0的整数，要换算成小数。
    Block->Axis[E].Steps = ROUND(FlowRatio > EOL_DATA ?
                           FlowRatio * ABS(Delta->E) * AxisStepPerMm[E]
                           :
                           ABS(Delta->E) * AxisStepPerMm[E]);   // + 0.5f;
#else
    Block->Axis[X].Steps = (S32)ROUND(ABS(Delta->X) * AxisStepPerMm[X]);
    Block->Axis[Y].Steps = (S32)ROUND(ABS(Delta->Y) * AxisStepPerMm[Y]);
    Block->Axis[Z].Steps = (S32)floor(ABS(Delta->Z) * AxisStepPerMm[Z]);
    /* apply flow.*/
    FLOAT FlowRatio = Feedrate.FlowRatio / 100.0;   //因为流量比百分比，不直接就是一个大于0的整数，要换算成小数。
    Block->Axis[E].Steps = (S32)ROUND(FlowRatio > EOL_DATA ?
                           FlowRatio * ABS(Delta->E) * AxisStepPerMm[E]
                           :
                           ABS(Delta->E) * AxisStepPerMm[E]);   // + 0.5f;
#endif
    /*
    printf("********stepE,deltaE,stepE,RatioE:(%d ", AxisStepPerMm[E]);
    PrintFloat(ABS(Delta->E));
    PrintFloat(ABS(Delta->E) * AxisStepPerMm[E]);
    PrintFloat(Block->Axis[E].Steps);
    printf(")\n");
	*/
//    Block->Axis[E].Steps = ROUND(Feedrate.FlowRatio > EOL_DATA ?
//                           Feedrate.FlowRatio * ABS(Delta->E) * AxisStepPerMm[E]
//                           :
//                           ABS(Delta->E) * AxisStepPerMm[E]) + 0.5f;
    U32 MaxSteps = MAX(Block->Axis[X].Steps, MAX(Block->Axis[Y].Steps, Block->Axis[Z].Steps));
    MaxSteps = MAX(MaxSteps, Block->Axis[E].Steps);
    FLOAT Millimeters = CalcSqrt(SQR(Delta->X) + SQR(Delta->Y) + SQR(Delta->Z));
    Millimeters = Millimeters <= EOL_DATA ? ABS(Delta->E) : Millimeters;

    Block->PlanCalc.StepEventCnt = MaxSteps;
    Block->PlanCalc.LineLength = Millimeters;

    //满足二倍速曲面打印效果，特加测试
//    if(Millimeters < 1.2f)
//    {
//        Block->PlanCalc.MaxSpeed = MIN(Block->PlanCalc.MaxSpeed,35.0f); //上限
//
//        Block->PlanCalc.MaxSpeed = MAX(Block->PlanCalc.MaxSpeed,5.0f);  //下限
//    }

    const FLOAT InverseMillimeters = Millimeters <= EOL_DATA ? 0 : 1.0f / Millimeters;
    /* Max Speed setted by Gcode,Feedrate.*/
    FLOAT InverseSec = Block->PlanCalc.MaxSpeed * InverseMillimeters;

//    const U8 BuffNum = GetPlanLineBuffNum(LineBuff);

    //U32 SegmentTimeUs = round(1000000.0f / InverseSec);

    /* reset Max Speed.*/
    Block->PlanCalc.MaxSpeed =  Millimeters * InverseSec;//unit:mm/s
    Block->PlanCalc.NominalRate = ceil(Block->PlanCalc.StepEventCnt * InverseSec);

    U32 StepsSec = ceil(MaxSteps * InverseSec);
    FLOAT CurrentSpeed[AxisNum] = {0};
    FLOAT SpeedFactor = 1.0f;

    LOOP_XYZE(i)                                                                //调整速度，统一为mm/s
    {
        CurrentSpeed[i] = delta_XYZE[i] * InverseSec;
        FLOAT CurSpeed = ABS(CurrentSpeed[i]),
            MaxSpeed = Block->Axis[i].MaxSpeed;
       if (CurSpeed > MaxSpeed)
            SpeedFactor = MIN(SpeedFactor, MaxSpeed / CurSpeed);
    }
//    {
//        CurrentSpeed[E] = delta_XYZE[E] * InverseSec;                   //这一句重复了上面的LOOP_XYZE循环，注释掉
//
//    }

    // Correct the speed.
    if (SpeedFactor < 1.0f)
    {
        LOOP_XYZE(i)
        {
            CurrentSpeed[i] *= SpeedFactor;
        }
        StepsSec *= SpeedFactor;
        Block->PlanCalc.MaxSpeed *= SpeedFactor;

        Block->PlanCalc.NominalRate *= SpeedFactor;
    }

    // Compute and limit the acceleration rate for the trapezoid generator.
    const FLOAT StepPerMM = MaxSteps * InverseMillimeters;
 
    FLOAT Accel = Block->PlanCalc.Acceleration * StepPerMM;                         //调整加速度，统一为step/s^2

    if (Block->Axis[X].Steps || Block->Axis[Y].Steps || Block->Axis[Z].Steps)
    {
        LOOP_XYZE(i)
        {
            if (Block->Axis[i].Steps && AxisAccelStepPerS2[i] < Accel)            //此处Accel是step/s^2, 分轴加速度也要统一为step/s^2，在初始化中计算AxisAccelStepPerS2[i]
            {
                const FLOAT compare = AxisAccelStepPerS2[i] * MaxSteps;
                if (Accel * Block->Axis[i].Steps > compare)
                    Accel = compare / Block->Axis[i].Steps;
            }
        }
    }

    Block->PlanCalc.Acceleration = Accel / StepPerMM;                                       //后续使用mm/s^2

    Block->PlanCalc.AccelStepPerS2 = Accel;                                                 //step/s^2

    Block->PlanCalc.AccelStepPerSecTick = (U32)(Block->PlanCalc.AccelStepPerS2 * (SQR(4096.0f) / (STEPPER_TIMER_RATE))); //加速度单位转换，其中SQR(4096.0f)是为了浮点转整型计算

    /* Initial limit on the segment entry velocity (mm/s).*/
    static FLOAT PreviousSafeSpeed;
    static FLOAT PreviouSpeed[AxisNum];
    static FLOAT PreviousMaxSpeed = 0.0f;

    FLOAT SafeSpeed = Block->PlanCalc.MaxSpeed;

    U8 Limited = 0;
    LOOP_XYZE(i)
    {
        const FLOAT Jerk = ABS(CurrentSpeed[i]),
                    MaxJerk = Block->Axis[i].MaxJerk;
        if (Jerk > MaxJerk)
        {
            if (Limited)
            {
                const FLOAT MJerk = Block->PlanCalc.MaxSpeed * MaxJerk;
                if ( Jerk * SafeSpeed > MJerk )
                {
                    SafeSpeed = MJerk / Jerk;
                }
            }
            else
            {
                SafeSpeed *= MaxJerk /Jerk;
                Limited ++;
            }
        }
    }

    FLOAT VmaxJunction;
//    if (BuffNum && (PreviousMaxSpeed > EOL_DATA) || PreviousMaxSpeed < -EOL_DATA)
    if (PreviousMaxSpeed > EOL_DATA || PreviousMaxSpeed < -EOL_DATA)
    {
        FLOAT V_Factor = 1.0f;
        Limited = 0;

        FLOAT SmallerSpeedFactor = 1.0;

        if(Block->PlanCalc.MaxSpeed < PreviousMaxSpeed)
        {
            VmaxJunction = Block->PlanCalc.MaxSpeed;
            SmallerSpeedFactor = VmaxJunction / PreviousMaxSpeed;
        }
        else
        {
            VmaxJunction = PreviousMaxSpeed;
        }

        LOOP_XYZE(i)
        {
            FLOAT V_Exit = PreviouSpeed[i] * SmallerSpeedFactor,
                  V_Entry = CurrentSpeed[i];
            if (Limited)
            {
                V_Exit *= V_Factor;
                V_Entry *= V_Factor;
            }
        /* Calculate jerk depending on whether the axis is coasting in the same direction or reversing.*/
            const FLOAT Jerk = (V_Exit > V_Entry)
            ?
            ((V_Entry > 0 || V_Exit < 0) ? (V_Exit - V_Entry) : MAX(V_Exit,-V_Entry))
            :
            ((V_Entry < 0 || V_Exit > 0) ? (V_Entry - V_Exit) : MAX(-V_Exit,V_Entry));

            if ( Jerk > Block->Axis[i].MaxJerk)
            {
                V_Factor *= (FLOAT)(Block->Axis[i].MaxJerk) / Jerk;
                Limited ++;
            }
        }
        if (Limited)
            VmaxJunction *= V_Factor;

        const FLOAT VmaxJunctionThreshold = VmaxJunction * 0.99f;
        if (PreviousSafeSpeed > VmaxJunctionThreshold && SafeSpeed > VmaxJunctionThreshold)
            VmaxJunction = SafeSpeed;
    }
    else
        VmaxJunction = SafeSpeed;

    PreviousSafeSpeed = SafeSpeed;

    Block->PlanCalc.MaxEntrySpeed = VmaxJunction;   //VmaxJunction是最大进入速度
    //Block->PlanCalc.EntrySpeed = VmaxJunction;    //VmaxJunction不是进入速度

//    const FLOAT AllowableSpeedSqr = max_allowable_speed_sqr(-(Block->PlanCalc.Acceleration), SQR((FLOAT)(MINIMUM_PLANNER_SPEED)), Millimeters);
    /**
     * TO DO: make sure
     */
//    Block->PlanCalc.EntrySpeed = GetPlanLineBuffNum(LineBuff) < 3 ? MINIMUM_PLANNER_SPEED : MIN(CalcSqrt(AllowableSpeedSqr),VmaxJunction); //速度统一为mm/s, 原来存在(mm/s)^2
    Block->PlanCalc.ExitSpeed = (FLOAT)MINIMUM_PLANNER_SPEED;
    Block->PlanCalc.EntrySpeed = (FLOAT)MINIMUM_PLANNER_SPEED; //MIN(CalcSqrt(AllowableSpeedSqr),VmaxJunction);

    Block->PlanCalc.ExitSpeedSqr = (FLOAT)SQR(MINIMUM_PLANNER_SPEED);
    Block->PlanCalc.EntrySpeedSqr = (FLOAT)SQR(MINIMUM_PLANNER_SPEED);

    LOOP_XYZE(i)
    {
        PreviouSpeed[i] = CurrentSpeed[i];
    }
    PreviousMaxSpeed = Block->PlanCalc.MaxSpeed;

    Block->PlanCalc.EnterStatus = ACCELERATE;
//
//    rt_kprintf("Block->PlanCalc.MaxEntrySpeed");
//
//    PrintFloat(Block->PlanCalc.MaxEntrySpeed);
//
//    rt_kprintf("\r\n");
    return 0;
}
/**
 * 函数功能: 规划当前坐标到目标坐标的直线运动(非回抽/恢复)
 * 函数参数: LineBuff:直线缓存队列,CurCoordinate:当前坐标,Target:目标坐标
 * 输入: CurCoordinate:当前坐标,Target:目标坐标
 * 输出: LineBuff:缓存队列
 * 返回值: 0:入队成功,小于0:入队失败
 */

S32  PlanLine(struct PlanLine_t *LineBuff, struct Coordinate3d_t *CurCoordinate, struct Coordinate3d_t *Target, BOOL Segment)
{
    if ( IsPlanLineBuffFull(LineBuff) )
        return -1;/* queue if full.*/

    struct Coordinate3d_t Dis;

    Dis.X = Target->X - CurCoordinate->X;
    Dis.Y = Target->Y - CurCoordinate->Y;
    Dis.Z = Target->Z - CurCoordinate->Z;
    Dis.E = Target->E - CurCoordinate->E;
	
    struct PlanLine_t *Line = &LineBuff[LineBuffTail];

    if (Feedrate.Feedrate < EOL_DATA)
        Feedrate.Feedrate = 10;
    /* complete Accel and feedrate.*/
    if ( ABS(Dis.E) < MIN_E_CHANGE)
    {
        Dis.E = 0.0f;/* less than min extrude.*/
        Line->PlanCalc.Acceleration = Feedrate.TravelAccelXYZ <= 0 ? ACCELERATION_TRAVEL : Feedrate.TravelAccelXYZ;
    }
    else
    {
        Line->PlanCalc.Acceleration = Feedrate.AccelXYZ <= 0 ? ACCELERATION_ACCEL : Feedrate.AccelXYZ;
        
    }
    Line->PlanCalc.MaxSpeed = Feedrate.FeedrateRatio > 0 
                                    ?
                                    Feedrate.Feedrate * (FLOAT)Feedrate.FeedrateRatio / 100
                                    : 
                                    Feedrate.Feedrate;

//    rt_kprintf("Line->PlanCalc.Acceleration");
//
//    PrintFloat(Line->PlanCalc.Acceleration);
//
//    rt_kprintf("\r\n");
//
//    rt_kprintf("Line->PlanCalc.MaxSpeed");
//
//    PrintFloat(Line->PlanCalc.MaxSpeed);
//
//    rt_kprintf("\r\n");

    Line->PlanCalc.MaxSpeed = MIN(Line->PlanCalc.MaxSpeed,VELOCITY_V_TRAV);
//    Line->PlanCalc.MaxSpeed = MAX(Line->PlanCalc.MaxSpeed,VELOCITY_V_TRAV * 0.25);

//    if(GetPlanLineBuffNum(LineBuff) < 7)
//    {
//        Line->PlanCalc.MaxSpeed *= (0.3f + GetPlanLineBuffNum(LineBuff) * 0.1f);
//
//        Line->PlanCalc.Acceleration *= (0.3f + GetPlanLineBuffNum(LineBuff) * 0.1f);
//    }

    /* populate Line buffer.*/
    if ( StuffLineBuff(&Dis, Line,Segment) < 0 )
        return -1;

    //CalculateForLine(&LineBuff[LineBuffTail]);
    /* set this line can be use.*/
    Line->Usable = TRUE;

    /* populate tail LineBuff first, then offset tail index.*/
    LineBuffTail = NextPlanLineIndex(LineBuffTail);
    
    /**
     * TO DO: coasting
     */
    Smooth.SmoothRecalc(LineBuff);
   
    return 0;
}
/**
 * 函数功能: 规划当前坐标到目标坐标的直线运动(回抽/恢复专用)
 * 函数参数: LineBuff:直线缓存队列,CurCoordinate:当前坐标,Target:目标坐标
 * 输入: CurCoordinate:当前坐标,Target:目标坐标
 * 输出: LineBuff:缓存队列
 * 返回值: 0:入队成功,小于0:入队失败
 */

S32  PlanRetract(struct PlanLine_t *LineBuff, struct Coordinate3d_t *CurCoordinate, struct Coordinate3d_t *Target, BOOL Segment)
{
    if ( IsPlanLineBuffFull(LineBuff) )
        return -1;/* queue if full.*/

    struct Coordinate3d_t Dis  = {
                .X = Target->X - CurCoordinate->X,
                .Y = Target->Y - CurCoordinate->Y,
                .Z = Target->Z - CurCoordinate->Z,
                .E = Target->E - CurCoordinate->E,
    };
	
    struct PlanLine_t *Line = &LineBuff[LineBuffTail];

    /* complete Accel and feedrate.*/

    Line->PlanCalc.Acceleration = Feedrate.RetractAccel <= EOL_DATA ? ACCELERATION_RETRACT : Feedrate.RetractAccel;
    Line->PlanCalc.MaxSpeed = Feedrate.RetractRate <= EOL_DATA ? VELOCITY_V_RETRACT : Feedrate.RetractRate;
    /* populate Line buffer.*/
    if ( StuffLineBuff(&Dis, Line,Segment) < 0 )
        return -1;
    //CalculateForLine(&LineBuff[LineBuffTail]);
    /* set this line can be use.*/
    Line->Usable = TRUE;
    /* populate tail LineBuff first, then offset tail index.*/
    LineBuffTail = NextPlanLineIndex(LineBuffTail);
    /**
     * TO DO: coasting
     */
    Smooth.SmoothRecalc(LineBuff);

    return 0;
}
/**
 * 函数功能: 调整当前坐标到目标坐标的理论值与实际运动的误差
 * 函数参数: Current:当前坐标,Target:目标坐标,TargetCorrected:实际运动结果
 * 输入: Current:当前坐标,Target:目标坐标
 * 输出: TargetCorrected:实际运动结果
 * 返回值: 无
 * 注: 步进电机只能整数步运动,实际计算出多数情况并不是整数,所以存在误差,另:浮点运算存在精度
 */
VOID CorrectTargetCoordinate(struct Coordinate3d_t *Current,
                                struct Coordinate3d_t *Target, struct Coordinate3d_t *TargetCorrected)
{
    struct StepperAttr_t AxisAttr[AxisNum] = {0};
    FLOAT MmPerSteps[AxisNum] = {0};

    static struct Coordinate3d_t OldTarget = {0};

    S32 DeltaStep[AxisNum] = {0};


    for (int i = X; i < AxisNum; i++)
    {
        if (CrStepperIoctl(Axis[i], CRSTEPPER_GET_ATTR, &AxisAttr[i]) < 0)
        {
            return;
        }
        MmPerSteps[i] = 1.0f / AxisAttr[i].StepsPerMm;
    }


    S32 Steps[AxisNum] = {0};
#if 0
    Steps[X] = ROUND((Target->X - Current->X) * AxisAttr[X].StepsPerMm);
    Steps[Y] = ROUND((Target->Y - Current->Y) * AxisAttr[Y].StepsPerMm);
    Steps[Z] = ROUND((Target->Z - Current->Z) * AxisAttr[Z].StepsPerMm);

    FLOAT Dis_E = Target->E - Current->E;

    if (Dis_E < MIN_E_CHANGE && Dis_E > -MIN_E_CHANGE)
    {
        Steps[E] = 0;
    }
    else
    {
        //与Feedrate.FlowRatio无关 此处*Feedrate.FlowRatio，后面又/Feedrate.FlowRatio
        Steps[E] = ROUND(Dis_E * AxisAttr[E].StepsPerMm);
    }

#else

    Steps[X] = (S32)ROUND((Target->X - Current->X) * AxisAttr[X].StepsPerMm);
    Steps[Y] = (S32)ROUND((Target->Y - Current->Y) * AxisAttr[Y].StepsPerMm);
    Steps[Z] = (S32)floor(ABS(Target->Z - Current->Z) * AxisAttr[Z].StepsPerMm);

    FLOAT Dis_E = Target->E - Current->E;

    if (Dis_E < MIN_E_CHANGE && Dis_E > -MIN_E_CHANGE)
    {
        Steps[E] = 0;
    }
    else
    {
        //与Feedrate.FlowRatio无关 此处*Feedrate.FlowRatio，后面又/Feedrate.FlowRatio
        Steps[E] = (S32)ROUND(Dis_E * AxisAttr[E].StepsPerMm);
    }

#endif


#if 1


    FLOAT TempF = 0.0f;

    S32 TempStep = 0;

//    rt_kprintf("B CrMotion::Deltamm[X]");
//    PrintFloat(CrMotion::Deltamm[X]);
//    printf("\r\n");

//    if(ABS(Current->X - Target->X) > EOL_DATA)
//    {
//        TempF = (FLOAT)((Current->X - Target->X) / MmPerSteps[X]);       //带余数步数
//
//        if(TempF > -EOL_DATA)   //大于等于0时，向0取整
//        {
//            DeltaStep[X] = (S32)floor(TempF); //负方向走
//        }
//        else
//        {
//            DeltaStep[X] = (S32)ceil(TempF); //正方向走
//        }
//
//        CrMotion::Deltamm[X] += (TempF - DeltaStep[X]); //计算误差,
//
//        if(CrMotion::Deltamm[X] > 1 - EOL_DATA)
//        {
//            TempStep = (S32)floor(CrMotion::Deltamm[X]);
//        }
//        else if(CrMotion::Deltamm[X] < -1 + EOL_DATA)
//        {
//            TempStep = (S32)ceil(CrMotion::Deltamm[X]);
//        }
//        else
//        {
//            TempStep = 0;
//        }
//
//        CrMotion::Deltamm[X] -= TempStep;
//
//        TargetCorrected->X = Target->X + TempStep * MmPerSteps[X];
//
//        OldTarget.X = TargetCorrected->X;
//    }
//    else
//    {
//        TargetCorrected->X = OldTarget.X;
//    }
//
//    if(ABS(Current->Y - Target->Y) > EOL_DATA)
//    {
//        TempF = (FLOAT)((Current->Y - Target->Y) / MmPerSteps[Y]);       //带余数步数
//
//        if(TempF > -EOL_DATA)   //大于等于0时
//        {
//            DeltaStep[Y] = (S32)floor(TempF);
//        }
//        else
//        {
//            DeltaStep[Y] = (S32)ceil(TempF);
//        }
//
//        CrMotion::Deltamm[Y] += (TempF - DeltaStep[Y]);
//
//        if(CrMotion::Deltamm[Y] > 1 - EOL_DATA)
//        {
//            TempStep = (S32)floor(CrMotion::Deltamm[Y]);
//        }
//        else if(CrMotion::Deltamm[Y] < -1 + EOL_DATA)
//        {
//            TempStep = (S32)ceil(CrMotion::Deltamm[Y]);
//        }
//        else
//        {
//            TempStep = 0;
//        }
//
//        CrMotion::Deltamm[Y] -= TempStep;
//
//        TargetCorrected->Y = Target->Y + TempStep * MmPerSteps[Y];
//
//        OldTarget.Y = TargetCorrected->Y;
//    }
//    else
//    {
//        TargetCorrected->Y = OldTarget.Y;
//    }

    if(ABS(Current->Z - Target->Z) > EOL_DATA)
    {
        TempF = (FLOAT)((Current->Z - Target->Z) / MmPerSteps[Z]);       //带余数步数

        if(TempF > -EOL_DATA)   //大于等于0时
        {
            DeltaStep[Z] = (S32)floor(TempF);
        }
        else
        {
            DeltaStep[Z] = (S32)ceil(TempF);
        }

        CrMotion::Deltamm[Z] += (TempF - DeltaStep[Z]);

        if(CrMotion::Deltamm[Z] > 1 - EOL_DATA)
        {
            TempStep = (S32)floor(CrMotion::Deltamm[Z]);
        }
        else if(CrMotion::Deltamm[Z] < -1 + EOL_DATA)
        {
            TempStep = (S32)ceil(CrMotion::Deltamm[Z]);
        }
        else
        {
            TempStep = 0;
        }

        CrMotion::Deltamm[Z] -= TempStep;

        TargetCorrected->Z = Target->Z + TempStep * MmPerSteps[Z];

        OldTarget.Z = TargetCorrected->Z;
    }
    else
    {
        TargetCorrected->Z = OldTarget.Z;
    }

//    if(ABS(Current->E - Target->E) > EOL_DATA)
//    {
//        TempF = (FLOAT)((Current->E - Target->E) / MmPerSteps[E]);       //带余数步数
//
//        if(TempF > -EOL_DATA)   //大于等于0时
//        {
//            DeltaStep[E] = (S32)floor(TempF);
//        }
//        else
//        {
//            DeltaStep[E] = (S32)ceil(TempF);
//        }
//
//        CrMotion::Deltamm[E] += (TempF - DeltaStep[E]);
//
//        if(CrMotion::Deltamm[E] > 1 - EOL_DATA)
//        {
//            TempStep = (S32)floor(CrMotion::Deltamm[E]);
//        }
//        else if(CrMotion::Deltamm[E] < -1 + EOL_DATA)
//        {
//            TempStep = (S32)ceil(CrMotion::Deltamm[E]);
//        }
//        else
//        {
//            TempStep = 0;
//        }
//
//        CrMotion::Deltamm[E] -= TempStep;
//
//        TargetCorrected->E = Target->E + TempStep * MmPerSteps[E];
//
//        OldTarget.E = TargetCorrected->E;
//    }
//    else
//    {
//        TargetCorrected->E = OldTarget.E;
//    }

    TargetCorrected->X = Current->X + (FLOAT)(Steps[X]) * MmPerSteps[X];
    TargetCorrected->Y = Current->Y + (FLOAT)(Steps[Y]) * MmPerSteps[Y];

//    TargetCorrected->Z = Current->Z + (FLOAT)(Steps[Z]) * MmPerSteps[Z];
    TargetCorrected->E = Current->E + (FLOAT)(Steps[E]) * MmPerSteps[E];

#else
    TargetCorrected->X = Current->X + (FLOAT)(Steps[X]) * MmPerSteps[X];
    TargetCorrected->Y = Current->Y + (FLOAT)(Steps[Y]) * MmPerSteps[Y];
    TargetCorrected->Z = Current->Z + (FLOAT)(Steps[Z]) * MmPerSteps[Z];

    TargetCorrected->E = Current->E + (FLOAT)(Steps[E]) * MmPerSteps[E];
#endif

    return;
}

/**
 * 函数功能: 获取直线运动队列缓存数
 * 函数参数: LineBuff:直线缓存队列
 * 输入: LineBuff:直线缓存队列
 * 输出: 获取运动直线队列缓存数
 * 返回值: 获取运动直线队列缓存数
 */
U8 GetPlanLineBuffNum(struct PlanLine_t *LineBuff)
{
    return ( LineBuffTail + BUFF_MAX_SIZE - LineBuffHead ) % BUFF_MAX_SIZE;
}
/**
 * 函数功能: 判断直线运动队列是否为空
 * 函数参数: LineBuff:直线缓存队列
 * 输入: LineBuff:直线缓存队列
 * 输出: 直线运动队列是否为空
 * 返回值: 直线运动队列是否为空
 */
BOOL IsPlanLineBuffEmpty(struct PlanLine_t *LineBuff)
{
    return (LineBuffHead == LineBuffTail);
}
/**
 * 函数功能: 判断直线运动队列是否为满
 * 函数参数: LineBuff:直线缓存队列
 * 输入: LineBuff:直线缓存队列
 * 输出: 直线运动队列是否为满
 * 返回值: 直线运动队列是否为满
 */
BOOL IsPlanLineBuffFull(struct PlanLine_t *LineBuff)
{
    return (LineBuffHead == NextPlanLineIndex(LineBuffTail));
}
/**
 * 函数功能: 清除直线运动队列所有缓存
 * 函数参数: LineBuff:直线缓存队列
 * 输入: LineBuff:直线缓存队列
 * 输出: 无
 * 返回值: 无
 */
VOID PlanLineBuffClear(struct PlanLine_t *LineBuff)
{
    while(!IsPlanLineBuffEmpty(LineBuff))
    {
        LineBuff[LineBuffHead].Usable = FALSE;
        LineBuffHead = NextPlanLineIndex(LineBuffHead);
    }
}
/**
 * 函数功能: 判断直线运动队列缓存是否运动完
 * 函数参数: LineBuff:直线缓存队列,运动Tick缓存队列
 * 输入: LineBuff:直线缓存队列,运动Tick缓存队列
 * 输出: 直线运动队列缓存是否运动完
 * 返回值: 直线运动队列缓存是否运动完
 */
#if 0
BOOL IsMotionComplete(struct PlanLine_t *LineBuff, struct Fifo_t *AxisFifo)
{
//    for(int i = X; i < AxisNum; i++)
//    {
        if (GetFifoDataSize(AxisFifo, TICK_BUF_SIZE * (sizeof(S32)), sizeof(S32)) > 0)
            return FALSE;
//    }
    return (SyncAlgorithm.IsCurrentLineComplete() && IsPlanLineBuffEmpty(LineBuff));
}
#else
BOOL IsMotionComplete(struct PlanLine_t *LineBuff, CommonFiFo_t *AxisFifo)
{
//    for(int i = X; i < AxisNum; i++)
//    {
//        if (GetFifoDataSize(AxisFifo, TICK_BUF_SIZE * (sizeof(S32)), sizeof(S32)) > 0)
//            return FALSE;
//    }
    if(CommonIsEmptyFifo(AxisFifo) == 0) //如果缓冲区不是空，一定没有执行完
    {
        return FALSE;
    }
    return (SyncAlgorithm.IsCurrentLineComplete() && IsPlanLineBuffEmpty(LineBuff));
}
#endif
/**
 * 函数功能: 获取直线运动队列缓存中的下一条直线缓存
 * 函数参数: Line:直线运动参数地址
 * 输入: 无
 * 输出: Line:直线运动参数地址
 * 返回值: 0:获取失败，1:获取成功
 */
BOOL GetNextPlanLine(struct PlanLine_t *Line)
{
    if ( IsPlanLineBuffEmpty(Line) )
        return 0;
    else
    {
        if(LineBuff[LineBuffHead].Usable == TRUE)
        {
            LineBuff[LineBuffHead].Usable = FALSE;
            memset(Line, 0, sizeof(struct PlanLine_t));
            memcpy(Line, &LineBuff[LineBuffHead], sizeof(struct PlanLine_t));
            Line->Usable = TRUE;
            LineBuffHead = NextPlanLineIndex(LineBuffHead);
            return 1;
        }
        else
        {
            return 0;
        }
    }
}
