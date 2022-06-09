/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date            Author                 Notes
 * 2021-04-30     TingFengXuan       the first version
 */
#ifndef __CR_MOTION_PLANNER__
#define __CR_MOTION_PLANNER__

#ifdef __cplusplus
extern "C"{
#endif

#include "CrInc/CrType.h"
#include "CrStepperRun.h"
#include "CrInc/CrConfig.h"

#define BUFF_MAX_SIZE   (16)     //NOTE: no less 3
#define EOL_DATA                (0.000001f) //浮点数精度
#define MIN_E_CHANGE            (0.01f)      //E轴最小运动距离
#define MIN_STEP_DELAY          (200)       //Tick的最小延时,根据硬件定时器周期变化而变化
#define SEC_TO_US               (1000000)   //秒转化微秒
#define MINIMUM_PLANNER_SPEED   (0.05f)    //最小进入初速度
//四舍五入
#define ROUND(Num)  ((Num) < -0.000001 ? (Num) - 0.5f : (Num) + 0.5f)

extern volatile U32 LineBuffHead;
extern volatile U32 LineBuffTail;


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * Feedrate Parameter
 */
struct FeedrateAttr_t
{
    U32         FeedrateRatio;      //进料速率比(进料速度/切片设定的进料速度, mm/s)

    FLOAT       Feedrate;           //切片设定的进料速度(mm/s)
    FLOAT       Jerk;               //合jerk(gcode中有此值，但目前没用到)
    FLOAT       JerkE;

    FLOAT       FlowRatio;          //E Axis Extrude Flow.
                                    //体积模式下：实际进料体积/设定进料体积
                                    //长度模式下：实际进料长度/设定进料长度
    FLOAT       RetractRate;        //回抽速率(mm/s)
    FLOAT       ExtrudeRate;        //挤出速率(mm/s)
    FLOAT       RetractAccel;       //回抽加速度(mm^2/s)
    /* be extended according to Gcode Command.*/

    S32         TravelAccelXYZ;     //不挤料时的合加速度
    S32         AccelXYZ;           //挤料时的合加速度
    S32         TravelVelMinXYZ;    //不挤料时的最小速度
    S32         VelMinXYZ;          //挤料时的最小合速度
};
/**
 * 运动状态
 */
enum CrMotionStatus_t
{
    STOP, ACCELERATE, DECELERATE, UNIFORM
};
/**
 * 运动坐标
 */
struct Coordinate3d_t
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
    FLOAT E;
};
/**
 * 运动方向
 */

enum Dir_t
{
    FORWARD  =  1,
    BACKWARD = -1
};

struct PlanLineResultant_t
{
    BOOL                    IsEndLine;
    enum CrMotionStatus_t   EnterStatus;

    FLOAT                   Acceleration;
    FLOAT                   MaxSpeed;           //TO DO: unit??
    FLOAT                   EntrySpeed;
    FLOAT                   EntrySpeedSqr;
    FLOAT                   ExitSpeed;
    FLOAT                   ExitSpeedSqr;
    FLOAT                   LineLength;
    FLOAT                   MaxEntrySpeed;
    U32                     StepEventCnt;
    U32                     EntryRate;
    U32                     EntryRateSqr;
    U32                     ExitRate;
    U32                     ExitRateSqr;
    U32                     NominalRate;
    U32                     AccelStepPerS2;
    U32                     AccelStepPerSecTick;
    //U32                     MinDelay;
    //U32                     SegmentTime;       //运动时间(unit:us)
};

struct PlanLineAxis_t
{
    enum Dir_t   Dir;
    U32          Steps;

    enum CrMotionStatus_t  EnterStatus; //合速度平滑策略中不用
    S32                    EntrySpeed;        //合速度平滑策略中不用
    S32                    ExitSpeed;         //合速度平滑策略中不用

    U32                    UniformStartStep;
    U32                    DecelStartStep;

    S32                    MaxSpeed;
    S32                    MaxAccel;
    S32                    MaxJerk;
};

struct PlanLine_t
{
    volatile U8                 Usable;      //线段内容可使用/修改标识
    struct PlanLineAxis_t       Axis[AxisNum];
    struct PlanLineResultant_t  PlanCalc;
};

extern struct PlanLine_t LineBuff[BUFF_MAX_SIZE];
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
/**
 * 函数功能: 初始化电机轴的Fd
 * 函数参数: AxisFd:电机轴Fd数组,AxisNum:轴数量
 * 输入: AxisFd
 * 输出: 无
 * 返回值: 无
 */
VOID PlannerInit(S32 AxisFd[], U32 AxisNum);

/**
 * 函数功能: 规划当前坐标到目标坐标的直线运动(非回抽/恢复)
 * 函数参数: LineBuff:直线缓存队列,CurCoordinate:当前坐标,Target:目标坐标
 * 输入: CurCoordinate:当前坐标,Target:目标坐标
 * 输出: LineBuff:缓存队列
 * 返回值: 0:入队成功,小于0:入队失败
 */
S32  PlanLine(struct PlanLine_t *LineBuff, struct Coordinate3d_t *CurCoordinate, struct Coordinate3d_t *Target, BOOL Segment = 0);
/**
 * 函数功能: 规划当前坐标到目标坐标的直线运动(回抽/恢复专用)
 * 函数参数: LineBuff:直线缓存队列,CurCoordinate:当前坐标,Target:目标坐标
 * 输入: CurCoordinate:当前坐标,Target:目标坐标
 * 输出: LineBuff:缓存队列
 * 返回值: 0:入队成功,小于0:入队失败
 */
S32  PlanRetract(struct PlanLine_t *LineBuff,   struct Coordinate3d_t *Current, struct Coordinate3d_t *Target, BOOL Segment = 0);  //回抽过程中，XYZ保持不动

//函数：CorrectTargetCoordinate     让目标坐标落在步进电机步距行程整数倍上
//输入：Current 当前坐标
//      Line 当前线段
//输出：TargetCorrected 校准后的目标坐标（刚好是步进电机步距行程整数倍）
//返回：无
VOID CorrectTargetCoordinate(struct Coordinate3d_t *Current, 
                              struct Coordinate3d_t *Target, struct Coordinate3d_t *TargetCorrected);
/**
 * 函数功能: 获取直线运动队列缓存数
 * 函数参数: LineBuff:直线缓存队列
 * 输入: LineBuff:直线缓存队列
 * 输出: 获取运动直线队列缓存数
 * 返回值: 获取运动直线队列缓存数
 */
U8   GetPlanLineBuffNum(struct PlanLine_t *LineBuff);                    //获取运动队列中运动块数量
/**
 * 函数功能: 判断直线运动队列是否为空
 * 函数参数: LineBuff:直线缓存队列
 * 输入: LineBuff:直线缓存队列
 * 输出: 直线运动队列是否为空
 * 返回值: 直线运动队列是否为空
 */
BOOL IsPlanLineBuffEmpty(struct PlanLine_t *LineBuff);                   //运动队列是否为空
/**
 * 函数功能: 判断直线运动队列是否为满
 * 函数参数: LineBuff:直线缓存队列
 * 输入: LineBuff:直线缓存队列
 * 输出: 直线运动队列是否为满
 * 返回值: 直线运动队列是否为满
 */
BOOL IsPlanLineBuffFull(struct PlanLine_t *LineBuff);                    //运动队列是否已满
/**
 * 函数功能: 清除直线运动队列所有缓存
 * 函数参数: LineBuff:直线缓存队列
 * 输入: LineBuff:直线缓存队列
 * 输出: 无
 * 返回值: 无
 */
VOID PlanLineBuffClear(struct PlanLine_t *LineBuff);                     //清除运动队列

//函数：IsMotionComplete     让目标坐标落在步进电机步距行程整数倍上
//输入：队列
//      TickBuffer队列
//输出：运动是否完成
//返回：无
//备注：判断逻辑，调用函数不再给队列和TickBuffer队列塞入数据，并且TickBuffer队列被取空
BOOL IsMotionComplete(struct PlanLine_t *LineBuff, CommonFiFo_t *AxisFifo);
/**
 * 函数功能: 获取直线运动队列缓存中的下一条直线缓存
 * 函数参数: Line:直线运动参数地址
 * 输入: 无
 * 输出: Line:直线运动参数地址
 * 返回值: 0:获取失败，1:获取成功
 */
BOOL GetNextPlanLine(struct PlanLine_t *Line);                 //获取下一个Line

/**
 * 函数功能: 设置速度属性参数
 * 函数参数: FrAttr:速度参数地址指针
 * 输入: FrAttr:速度参数地址指针
 * 输出: 无
 * 返回值: 无
 */
VOID FeedrateAttrSet(struct FeedrateAttr_t *FrAttr);           //设置进退料参数
/**
 * 函数功能: 获取速度属性参数
 * 函数参数: FrAttr:速度参数地址指针
 * 输入: 无
 * 输出: FrAttr:速度参数地址指针
 * 返回值: 无
 */
VOID FeedrateAttrGet(struct FeedrateAttr_t *FrAttr);           //获取进退料参数
/**
 * 函数功能: 计算最大允许进入速度
 * 函数参数: accel:加速度,target_velocity_sqr:最大速度,distance:运动距离
 * 输入: accel:加速度,target_velocity_sqr:最大速度,distance:运动距离
 * 输出: 最大允许进入速度
 * 返回值: 最大允许进入速度
 */
FLOAT max_allowable_speed_sqr(const FLOAT accel, const FLOAT target_velocity_sqr, 
                                        const FLOAT distance);
/**
 * 函数功能: 获取目标索引的后一个直线索引
 * 函数参数: Index:目标索引
 * 输入: Index:目标索引
 * 输出: 目标索引的后一个直线索引
 * 返回值: 目标索引的后一个直线索引
 */
U32 NextPlanLineIndex(U32 Index);
/**
 * 函数功能: 获取目标索引的前一个直线索引
 * 函数参数: Index:目标索引
 * 输入: Index:目标索引
 * 输出: 目标索引的前一个直线索引
 * 返回值: 目标索引的前一个直线索引
 */
U32 PrevPlanLineIndex(U32 Index);



extern struct PlanLine_t LineBuff[BUFF_MAX_SIZE];

#ifdef __cplusplus
}
#endif

#endif
