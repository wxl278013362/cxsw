/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date            Author                 Notes
 * 2021-04-29     TingFengXuan       the first version
 */
#ifndef __BRESENHAM_H__
#define __BRESENHAM_H__

#include "Fifo.h"
#include "CrInc/CrType.h"
#include "CrMotionPlanner.h"

#define SQR(n)                   ((n) * (n))                //计算平方
#define CUBE(n)                  ((n) * (n) * (n))          //计算三次方
#define ABS(n)                   ((n) > 0 ? (n) : -(n))     //绝对值

#define STEPPER_TIMER_RATE              (1000000)
#define STEPPER_TIMER_TICKS_PER_US      (STEPPER_TIMER_RATE/1000000)
#define F_CPU                           (128000000)
#define MAX_STEPPER_RATE                (500000)

#define LOOP_XYZ(Axis)    for(int Axis = X; Axis <= Z; Axis++)          //轮询XYZ轴
#define LOOP_XYZE(Axis)   for(int Axis = X; Axis < AxisNum; Axis++)     //轮询总轴数

#define INT_MAX                  (0x7fffffff)   //int类型的最大值
#define CALC_TICK_NUM           (10)            //算法每一次计算的步数

#define NEW_BRESENHAM_ENABLE

struct Bresenham_t{
    enum Axis_t             LongestAxis;        //记录运动中的运动距离最长的轴
    U32                     StepsCount;         //计算步数计数（长轴）
    FLOAT                   Deviation[AxisNum]; //计算对应每个轴每一步的误差(每次计算累积,当误差大于0.5时，对应的轴走一步)
    FLOAT                   Slop[AxisNum];      //每一步相对于最长轴比例(最长轴每走一步,)

#ifdef NEW_BRESENHAM_ENABLE
    S32                     NewDivident[AxisNum];
    S32                     NewDevision;
    S32                     NewDeltaErr[AxisNum];
#endif
    /**
     * TO DO:
     */
    struct PlanLine_t Line;
};

class AxisSyncAlgorithm       // 已知line信息，转化为tick数
{
public:
    virtual VOID Calc() = 0;
//    BOOL Put2TickBuffer(S32 Ticks[], S32 TicksNum, struct Fifo_t *AxisFifo);
    S32 Put2TickBuffer(CommonFiFo_t *AxisFifo, MotionPeriod_t MotionPeriod);
};

S32 OverFlowAddS32(S32 Num1, S32 Num2, S32 Max);    //做溢出处理
BOOL IsOverFlowS32(S32 Dest, S32 Src);                  //判断是否溢出

class Bresenham : public AxisSyncAlgorithm
{
public:
    VOID Calc();                                //继承父类,无需实现
    VOID DiscardCurrentLine();                  //丢弃当前运动直线
    BOOL IsCurrentLineComplete();               //判断当前运动直线是否计算完成
    S32 Calc(struct PlanLine_t Line[], S32 Ticks[][CALC_TICK_NUM], U32 CalcNum);    //返回长轴的TicksNum
    S32 Calc(struct PlanLine_t Line[], U32 MotionNum);
#ifdef NEW_BRESENHAM_ENABLE
    BOOL BresenhamCalc(U32 Axis);                //Bresenham
#endif

//    U32 Put2TickBuffer(S32 Ticks[][CALC_TICK_NUM], S32 TicksNum, struct Fifo_t *AxisFifo);   //返回塞入buffer最少的ticks数量
    S32 Put2TickBuffer(CommonFiFo_t *AxisFifo, MotionPeriod_t MotionPeriod);
    /* 缓存Ticks塞入一部分到Fifo中后，未避免重复运算，Ticks 中的数据需向前平移最少塞入数据的个数，待下次塞入.*/
};
extern Bresenham SyncAlgorithm;

/**************** DEBUG ***********************/
VOID PrintLine(struct PlanLine_t *Line);
VOID PrintFloat(float f);

#endif
