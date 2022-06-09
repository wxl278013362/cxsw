#include "StepperApi.h"
#include "CrEndStop.h"
#include "Common/Include/CrSleep.h"
#include "Bresenham.h"

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdio.h>
#include <string.h>



extern volatile U32 LineBuffHead;
extern volatile U32 LineBuffTail;
extern struct Bresenham_t Substance;
//当前坐标缓存
struct Coordinate3d_t CurCoordination = {0};
//目标坐标缓存
struct Coordinate3d_t Target = {0};
//电机描述符数组
static S32 StepperFd[AxisNum] = {-1,-1,-1,-1};
//限位开关描述符数组
S32 EndStopFd[AxisNum] = {-1,-1,-1,-1};
//X轴归零标识位
#define HOME_X_FLAG     (0x01 << 1)
//Y轴归零标识位
#define HOME_Y_FLAG     (0x01 << 2)
//Z轴归零标识位
#define HOME_Z_FLAG     (0x01 << 3)

static U8 HomeSetFlag = 0;
/**
 * 函数功能: 电机初始化
 * 函数参数: Stepper_Fd: 电机Fd数组,EndStop_Fd: 限位开关Fd数组,AxisNum: 轴数量
 * 输入: Stepper_Fd: 电机Fd数组,EndStop_Fd: 限位开关Fd数组,AxisNum: 轴数量
 * 输出: 无
 * 返回值: 无
 */
VOID StepperInit(S32 Stepper_Fd[], S32 EndStop_Fd[], U32 AxisNum)
{
    for (int i = X; i < AxisNum; i++)
    {
        StepperFd[i] = Stepper_Fd[i];
    }

    for (int i = X; i < AxisNum; i++)
    {
        EndStopFd[i] = Stepper_Fd[i];
    }
}
/**
 * 函数功能: 归零Z轴
 * 函数参数: 无
 * 输入: 无
 * 输出: 是否归零成功
 * 返回值: 1:成功,0:未归零成功
 */
BOOL MoveHomeZ(VOID)
{

    /* Enable Z Axis EndStop.*/
    if (CrEndStopIoctl(EndStopFd[Z], CRENDSTOP_SET_ENABLE,0) < 0)
        {
        rt_kprintf("EndStop Z Not Exist.\n");
        return 0;
    }

    struct Coordinate3d_t CurCoordinate = {0};
    struct Coordinate3d_t Target = {0};
    
    U8 EndStopStatus = 0xff;
    if(CrEndStopIoctl(EndStopFd[Z],CRENDSTOP_GET_TRAGSTATUS,&EndStopStatus) < 0)
    {
        rt_kprintf("<error> Z Axis Limit is Invalid!\n");
        return FALSE;
    }
    if( EndStopStatus == FALSE )
    {
        Target.E = CurCoordinate.E;
        Target.X = CurCoordinate.X;
        Target.Y = CurCoordinate.Y;
        Target.Z = CurCoordinate.Z - Z_AXIS_MAX_SIZE;

        while ( PlanLine(LineBuff, &CurCoordinate, &Target) < 0 )
        {
            CrM_Sleep(SAFE_DELAY_MS);
        }
        while ( !IsMotionComplete(LineBuff, &AxisFifo))
        {
            if (CrEndStopIoctl(EndStopFd[Z], CRENDSTOP_GET_TRAGSTATUS, &EndStopStatus) >= 0
                && EndStopStatus == TRUE)
                break;
            CrM_Sleep(SAFE_DELAY_MS);
        }
    }
    /**
     * first probe.
     */
    if (CrEndStopIoctl(EndStopFd[Z], CRENDSTOP_GET_TRAGSTATUS, &EndStopStatus) >= 0
            && EndStopStatus == TRUE)
    {
        /* probe success,and stop move stepper.*/
        PlanLineBuffClear(LineBuff);
        SyncAlgorithm.DiscardCurrentLine();
        SteppersStopQuick();
        CrEndStopIoctl(EndStopFd[Z], CRENDSTOP_SET_DISABLE, 0);
    }
    else
    {
        return FALSE;
    }
    /**
     * Second Probe: Z Axis offset probe.
     */
    Target.E = CurCoordinate.E;
    Target.X = CurCoordinate.X;
    Target.Y = CurCoordinate.Y;
    Target.Z = CurCoordinate.Z + HOME_Z_OFFSET;
    while ( PlanLine(LineBuff, &CurCoordinate, &Target) < 0 )
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }
    while ( !(IsMotionComplete(LineBuff, &AxisFifo)))
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }

    /*Enable EndStop.*/
    CrEndStopIoctl(EndStopFd[Z], CRENDSTOP_SET_ENABLE, 0);

    Target.E = CurCoordinate.E;
    Target.X = CurCoordinate.X;
    Target.Y = CurCoordinate.Y;
    Target.Z = CurCoordinate.Z - HOME_Z_OFFSET;
    while ( PlanLine(LineBuff, &CurCoordinate, &Target) < 0 )
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }
    
    /*Waiting for Moving finished.*/
    while ( !(IsMotionComplete(LineBuff, &AxisFifo)))
    {
        if (CrEndStopIoctl(EndStopFd[X], CRENDSTOP_GET_TRAGSTATUS, &EndStopStatus) >= 0
                && EndStopStatus == TRUE)
                break;
        CrM_Sleep(SAFE_DELAY_MS);
    }
    /* Set Z Axis Posintion.*/
    S32 Pos = 0;
    S32 ret = CrStepperIoctl(StepperFd[Z], CRSTEPPER_SET_POS, &Pos);

    if (ret < 0)
    {
        printf("Reset Position Failed!\n");
        return 0;
    }
    /* Set Z Axis home flag.*/
    HomeSetFlag |= HOME_Z_FLAG;
    /* Disable EndStop.*/
    CrEndStopIoctl(EndStopFd[Z], CRENDSTOP_SET_DISABLE,0);
    return 1;
}
/**
 * 函数功能: 归零X轴
 * 函数参数: 无
 * 输入: 无
 * 输出: 是否归零成功
 * 返回值: 1:成功,0:未归零成功
 */
BOOL MoveHomeX(VOID)
{
    if (CrEndStopIoctl(EndStopFd[X], CRENDSTOP_SET_ENABLE, 0) < 0)
    {
        rt_kprintf("EndStop X Not Exist.\n");
        return 0;
    }
    struct Coordinate3d_t CurCoordinate = {0};
    struct Coordinate3d_t Target = {0};

    U8 EndStopStatus = 0xff;
    //判断限位开关是否已经被触发
    if(CrEndStopIoctl(EndStopFd[X],CRENDSTOP_GET_TRAGSTATUS,&EndStopStatus) < 0)
    {
        rt_kprintf("<error> X Axis Limit is Invalid!\n");
        return FALSE;
    }
    if( EndStopStatus == FALSE )
    {
        Target.E = CurCoordinate.E;
        Target.X = CurCoordinate.X - X_AXIS_MAX_SIZE;
        Target.Y = CurCoordinate.Y;
        Target.Z = CurCoordinate.Z;
        while( PlanLine(LineBuff, &CurCoordinate, &Target) < 0 )
        {
            CrM_Sleep(SAFE_DELAY_MS);
        }
        while ( !(IsMotionComplete(LineBuff, &AxisFifo)))
        {
            if (CrEndStopIoctl(EndStopFd[X], CRENDSTOP_GET_TRAGSTATUS, &EndStopStatus) >= 0
                && EndStopStatus == TRUE)
                break;
            CrM_Sleep(SAFE_DELAY_MS);
        }
    }
    /**
     * first probe.
     */
    if (CrEndStopIoctl(EndStopFd[X], CRENDSTOP_GET_TRAGSTATUS, &EndStopStatus) >= 0 
        && EndStopStatus == TRUE)
    {
        /* probe success,and stop move stepper.*/
        PlanLineBuffClear(LineBuff);
        SyncAlgorithm.DiscardCurrentLine();
        SteppersStopQuick();
        CrEndStopIoctl(EndStopFd[X], CRENDSTOP_SET_DISABLE, 0);
    }
    else
    {
        return FALSE;
    }
    /**
     * Second Probe.
     */
    Target.E = CurCoordinate.E;
    Target.X = CurCoordinate.X + HOME_X_OFFSET;
    Target.Y = CurCoordinate.Y;
    Target.Z = CurCoordinate.Z;
    while ( PlanLine(LineBuff, &CurCoordinate, &Target) < 0 )
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }
    
    while ( !(IsMotionComplete(LineBuff, &AxisFifo)))
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }

    Target.E = CurCoordinate.E;
    Target.X = CurCoordinate.X - HOME_X_OFFSET;
    Target.Y = CurCoordinate.Y;
    Target.Z = CurCoordinate.Z;
    while ( PlanLine(LineBuff, &CurCoordinate, &Target) < 0)
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }
    /* Waiting for Moving finished.*/
    CrEndStopIoctl(EndStopFd[X], CRENDSTOP_SET_ENABLE, 0);
    while ( !(IsMotionComplete(LineBuff, &AxisFifo)))
    {
        if (CrEndStopIoctl(EndStopFd[X], CRENDSTOP_GET_TRAGSTATUS, &EndStopStatus) >= 0
                && EndStopStatus == TRUE)
            break;
        CrM_Sleep(SAFE_DELAY_MS);
    }
    /*Set X Axis Posintion.*/
    S32 Pos = 0;
    S32 ret = CrStepperIoctl(StepperFd[X], CRSTEPPER_SET_POS, &Pos);

    if (ret < 0)
    {
        printf("Reset Position Failed!\n");
        return 0;
    }
    /* Set X Axis home flag.*/
    HomeSetFlag |= HOME_X_FLAG;
    /* Disable EndStop.*/
    CrEndStopIoctl(EndStopFd[X], CRENDSTOP_SET_DISABLE,0);

    return 1;
}
/**
 * 函数功能: 归零Y轴
 * 函数参数: 无
 * 输入: 无
 * 输出: 是否归零成功
 * 返回值: 1:成功,0:未归零成功
 */
BOOL MoveHomeY(VOID)
{
    if (CrEndStopIoctl(EndStopFd[Y], CRENDSTOP_SET_ENABLE,0) < 0)
    {
        rt_kprintf("EndStop Y Not Exist.\n");
        return 0;
    }
    
    struct Coordinate3d_t CurCoordinate = {0};
    struct Coordinate3d_t Target = {0};
    
    U8 EndStopStatus = 0xff;
    if (CrEndStopIoctl(EndStopFd[Y], CRENDSTOP_GET_TRAGSTATUS, &EndStopStatus) < 0)
    {
        rt_kprintf("<error> Y Axis Limit is Invalid!\n");
        return FALSE;
    }
    if ( EndStopStatus == FALSE)
    {
        Target.E = CurCoordinate.E;
        Target.X = CurCoordinate.X;
        Target.Y = CurCoordinate.Y - Y_AXIS_MAX_SIZE;
        Target.Z = CurCoordinate.Z;
        
        while ( PlanLine(LineBuff, &CurCoordinate, &Target) < 0 )
        {
            CrM_Sleep(SAFE_DELAY_MS);
        }
        while ( !(IsMotionComplete(LineBuff, &AxisFifo)))
        {
            if (CrEndStopIoctl(EndStopFd[Y], CRENDSTOP_GET_TRAGSTATUS, &EndStopStatus) >= 0
                && EndStopStatus == TRUE)
                break;
            CrM_Sleep(SAFE_DELAY_MS);
        }
    }
    /**
     * first probe.
     */
    if (CrEndStopIoctl(EndStopFd[Y],CRENDSTOP_GET_TRAGSTATUS,&EndStopStatus) >= 0 
        && EndStopStatus == TRUE)

    {
        /* probe success,and stop move stepper.*/
        PlanLineBuffClear(LineBuff);
        SyncAlgorithm.DiscardCurrentLine();
        SteppersStopQuick();
        CrEndStopIoctl(EndStopFd[Y], CRENDSTOP_SET_DISABLE, 0);
    }
    else
    {
        return FALSE;
    }
    /**
     * Secode Probe: Y Axis offset probe.
     */
    Target.E = CurCoordinate.E;
    Target.X = CurCoordinate.X;
    Target.Y = CurCoordinate.Y + HOME_Y_OFFSET;
    Target.Z = CurCoordinate.Z;
    while ( PlanLine(LineBuff, &CurCoordinate, &Target) < 0 )
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }
    while ( !(IsMotionComplete(LineBuff, &AxisFifo)))
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }

    /* Enable EndStop.*/
    CrEndStopIoctl(EndStopFd[Y], CRENDSTOP_SET_ENABLE,0);

    Target.E = CurCoordinate.E;
    Target.X = CurCoordinate.X;
    Target.Y = CurCoordinate.Y - HOME_Y_OFFSET;
    Target.Z = CurCoordinate.Z;
    while ( PlanLine(LineBuff, &CurCoordinate, &Target) < 0 )
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }
    
    /*Waiting for Moving finished.*/
    while ( !(IsMotionComplete(LineBuff, &AxisFifo)))
    {
        if (CrEndStopIoctl(EndStopFd[Y], CRENDSTOP_GET_TRAGSTATUS, &EndStopStatus) >= 0
                && EndStopStatus == TRUE)
                break;
        CrM_Sleep(SAFE_DELAY_MS);
    }
    /* Set Y Axis Posintion.*/
    S32 Pos = 0;
    S32 CurPos = -1;
    S32 ret = CrStepperIoctl(StepperFd[Y], CRSTEPPER_SET_POS, &Pos);
    CrStepperIoctl(StepperFd[Y], CRSTEPPER_GET_POS, &CurPos);
    
    if (ret < 0)
    {
        printf("Reset Position Failed!\n");
        return 0;
    }
    /* Set Y Axis home flag.*/
    HomeSetFlag |= HOME_Y_FLAG;
    /* Disable EndStop.*/
    CrEndStopIoctl(EndStopFd[Y], CRENDSTOP_SET_DISABLE, 0);
    return TRUE;
}
/**
 * 函数功能: 判断X,Y,Z轴是否都归零
 * 函数参数: 无
 * 输入: 无
 * 输出: X,Y,Z轴是否归零
 * 返回值: 1:已归零,0:未归零
 */
BOOL IsHome()
{
    return ((HomeSetFlag & HOME_Z_FLAG) && (HomeSetFlag & HOME_X_FLAG) && (HomeSetFlag & HOME_Y_FLAG));
}
/**
 * 函数功能: 判断X轴是否归零
 * 函数参数: 无
 * 输入: 无
 * 输出: X轴是否归零
 * 返回值: 1:已归零,0:未归零
 */
BOOL IsHomeX()
{
    return (HomeSetFlag & HOME_X_FLAG);
}
/**
 * 函数功能: 判断Y轴是否归零
 * 函数参数: 无
 * 输入: 无
 * 输出: Y轴是否归零
 * 返回值: 1:已归零,0:未归零
 */
BOOL IsHomeY()
{
    return (HomeSetFlag & HOME_Y_FLAG);
}
/**
 * 函数功能: 判断Z轴是否归零
 * 函数参数: 无
 * 输入: 无
 * 输出: Z轴是否归零
 * 返回值: 1:已归零,0:未归零
 */
BOOL IsHomeZ()
{
    return (HomeSetFlag & HOME_Z_FLAG);
}

/**
 * 函数功能: 设置电机属性
 * 函数参数: Attr: 电机属性结构体指针,Axis: 电机轴
 * 输入: Attr: 电机属性结构体指针,Axis: 电机轴
 * 输出: 无
 * 返回值: 小于0:失败,0:成功
 */
S32 StepperSetAttr(struct StepperAttr_t *Attr,enum Axis_t Axis)
{
    return CrStepperIoctl(StepperFd[Axis], CRSTEPPER_SET_ATTR, Attr);
}
/**
 * 函数功能: 获取电机属性
 * 函数参数: Attr: 电机属性结构体指针,Axis: 电机轴
 * 输入: Axis: 电机轴
 * 输出: Attr: 电机属性结构体指针
 * 返回值: 小于0:失败,0:成功
 */
S32 StepperGetAttr(struct StepperAttr_t *Attr,enum Axis_t Axis)
{
    return CrStepperIoctl(StepperFd[Axis], CRSTEPPER_GET_ATTR, Attr);
}
/**
 * 函数功能: 设置电机运动属性
 * 函数参数: Attr: 电机运动属性结构体指针,Axis: 电机轴
 * 输入: Attr: 电机运动属性结构体指针,Axis: 电机轴
 * 输出: 无
 * 返回值: 小于0:失败,0:成功
 */
S32 StepperSetMotionAttr(struct StepperMotionAttr_t *Attr,enum Axis_t Axis)
{
    return CrStepperIoctl(StepperFd[Axis], CRSTEPPER_SET_MOTION_ATTR, Attr);
}
/**
 * 函数功能: 获取电机运动属性
 * 函数参数: Attr: 电机运动属性结构体指针,Axis: 电机轴
 * 输入: Axis: 电机轴
 * 输出: Attr: 电机运动属性结构体指针
 * 返回值: 小于0:失败,0:成功
 */
S32 StepperGetMotionAttr(struct StepperMotionAttr_t *Attr,enum Axis_t Axis)
{
    return CrStepperIoctl(StepperFd[Axis], CRSTEPPER_GET_MOTION_ATTR, Attr);
}
/**
 * 函数功能: 设置电机当前步数
 * 函数参数: Pos: 坐标地址指针,Axis: 电机轴
 * 输入: Pos: 坐标地址指针
 * 输出: 无
 * 返回值: 小于0:失败,0:成功
 */
S32 StepperSetPos(struct StepperPos_t *Pos,enum Axis_t Axis)
{
    return CrStepperIoctl(StepperFd[Axis], CRSTEPPER_SET_POS, &Pos->CurPosition);
}
/**
 * 函数功能: 获取当前实际坐标
 * 函数参数: Pos:坐标地址指针
 * 输入: 无
 * 输出: Pos:坐标地址指针
 * 返回值: 小于0:失败,0:成功
 */
S32 StepperGetPos(struct StepperPos_t *Pos,enum Axis_t Axis)
{
    return CrStepperIoctl(StepperFd[Axis], CRSTEPPER_GET_POS, &Pos->CurPosition);
}
/**
 * 函数功能: 设置当前实际坐标
 * 函数参数: Pos:坐标地址指针
 * 输入: Pos:坐标地址指针
 * 输出: 无
 * 返回值: 小于0:失败,0:成功
 */
S32 StepperSetCurCoordinate(struct        Coordinate3d_t *Pos)
{
    /* Set X Axis coordinate.*/
    struct StepperAttr_t Attr = {0};
    StepperGetAttr(&Attr,X);
    S32 PosX = Pos->X * Attr.StepsPerMm;
    if (CrStepperIoctl(StepperFd[X], CRSTEPPER_SET_POS, &PosX) < 0)
    {
        return -1;
    }
    CurCoordination.X = (FLOAT)PosX / Attr.StepsPerMm;
    
    /* Set Y Axis coordinate.*/
    memset(&Attr,0,sizeof(struct StepperAttr_t));
    StepperGetAttr(&Attr,Y);
    S32 PosY = Pos->Y * Attr.StepsPerMm;
    if (CrStepperIoctl(StepperFd[Y], CRSTEPPER_SET_POS, &PosY) < 0)
    {
        return -1;
    }
    CurCoordination.Y = (FLOAT)PosY / Attr.StepsPerMm;
    
    /* Set Z Axis coordinate.*/
    memset(&Attr,0,sizeof(struct StepperAttr_t));
    StepperGetAttr(&Attr,Z);
    S32 PosZ = Pos->Z * Attr.StepsPerMm;
    if (CrStepperIoctl(StepperFd[Z], CRSTEPPER_SET_POS, &PosZ) < 0)
    {
        return -1;
    }
    CurCoordination.Z = (FLOAT)PosZ / Attr.StepsPerMm;

    /* Set E Axis coordinate.*/
    memset(&Attr,0,sizeof(struct StepperAttr_t));
    StepperGetAttr(&Attr,E);
    S32 PosE = Pos->E * Attr.StepsPerMm;
    if (CrStepperIoctl(StepperFd[E], CRSTEPPER_SET_POS, &PosE) < 0)
    {
        return -1;
    }
    CurCoordination.E = (FLOAT)PosE / Attr.StepsPerMm;
    return 0;
}
/**
 * 函数功能: 获取当前实际坐标
 * 函数参数: Pos:坐标地址指针
 * 输入: 无
 * 输出: Pos:坐标地址指针
 * 返回值: 小于0:失败,0:成功
 */
S32 StepperGetCurCoordinate(struct        Coordinate3d_t *Pos)
{
    S32 PosX = -1,PosY = -1,PosZ = -1,PosE = -1;
    if (CrStepperIoctl(StepperFd[X],CRSTEPPER_GET_POS,&PosX) < 0)
    {
        return -1;
    }
    if (CrStepperIoctl(StepperFd[Y],CRSTEPPER_GET_POS,&PosY) < 0)
    {
        return -1;
    }
    if (CrStepperIoctl(StepperFd[Z],CRSTEPPER_GET_POS,&PosZ) < 0)
    {
        return -1;
    }
    if (CrStepperIoctl(StepperFd[E],CRSTEPPER_GET_POS,&PosE) < 0)
    {
        return -1;
    }
    struct StepperAttr_t AttrX = {0};
    struct StepperAttr_t AttrY = {0};
    struct StepperAttr_t AttrZ = {0};
    struct StepperAttr_t AttrE = {0};
    
    StepperGetAttr(&AttrX,X);
    StepperGetAttr(&AttrY,Y);
    StepperGetAttr(&AttrZ,Z);
    StepperGetAttr(&AttrE,E);
    
    Pos->X = (FLOAT)PosX / AttrX.StepsPerMm;
    Pos->Y = (FLOAT)PosY / AttrY.StepsPerMm;
    Pos->Z = (FLOAT)PosX / AttrZ.StepsPerMm;
    Pos->E = (FLOAT)PosY / AttrE.StepsPerMm;
    return 0;
}
/**
 * 函数功能: 步进微调
 * 函数参数: Fd:电机轴描述符,Dir: 运动方向,Steps:微调步数
 * 输入: Fd:电机轴描述符,Dir: 运动方向,Steps:微调步数
 * 输出: 无
 * 返回值: 小于0:失败,0:成功
 */
S32 StepperBabyMove(S32 Fd, U8 Dir , U32 Steps)
{
    if (Fd < 0)
    {
        return -1;
    }
    S32 Pos = 0;
    /* Get position and interval.*/
    CrStepperIoctl(Fd,CRSTEPPER_GET_POS,&Pos);
    /* Positive direction.*/
    if( Dir > 0 )
    {
        if (CrStepperIoctl(Fd,CRSTEPPER_SET_POSITIVE_DIR,NULL) < 0)
        {
            return -1;
        }
        Pos ++;
    }
    /* Negative direction.*/
    else if (Dir < 0)
    {
        if (CrStepperIoctl(Fd,CRSTEPPER_SET_NEGATIVE_DIR,NULL) < 0)
        {
            return -1;
        }
        Pos --;
    }
    while(Steps --)
    {
        /* Move one step.*/
        if (CrStepperIoctl(Fd,CRSTEPPER_SET_MOVE_ONE_IMPLUSE,NULL) < 0)
        {
            return -1;
        }
    }

    return 0;
}

/**
 * 函数功能: 设置速度属性参数
 * 函数参数: FrAttr:速度参数地址指针
 * 输入: FrAttr:速度参数地址指针
 * 输出: 无
 * 返回值: 无
 */
VOID StepperSetFeedrateAttr(struct FeedrateAttr_t *Attr)
{
    FeedrateAttrSet(Attr);
}
/**
 * 函数功能: 获取速度属性参数
 * 函数参数: FrAttr:速度参数地址指针
 * 输入: FrAttr:速度参数地址指针
 * 输出: 无
 * 返回值: 无
 */
VOID StepperGetFeedrateAttr(struct FeedrateAttr_t *Attr)
{
    FeedrateAttrGet(Attr);
}
/**
 * 函数功能: 快速停止
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 0
 * 注: 电机可能会丢步
 */
S32 SteppersStopQuick()
{
    StepperTimerStop();
    PlanLineBuffClear(LineBuff);
    SyncAlgorithm.DiscardCurrentLine();
    
//    for (int i = X; i < AxisNum; i++)
    CommonRstFifo(&AxisFifo);
    //CurrentMoveBlock->Compelte = 1;
    //CurrentMoveBlock = NULL;
//    StepperTimerRun();
    return 0;
}
/**
 * 函数功能: 等待电机所有的直线运动缓存完成
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 0
 */
S32 SteppersStopSafely(VOID)
{
    while( !(IsMotionComplete(LineBuff, &AxisFifo)) )
    {
        rt_thread_delay(10);
    }
    return 0;
}
/**
 * 函数功能: 电机使能
 * 函数参数: Fd: 电机描述符
 * 输入: Fd: 电机描述符
 * 输出: 参考返回值
 * 返回值: 小于0:失败,0:成功
 */
S32 SteppersEnable(S32 Fd)
{
    /* All Steppers share a enable pin .*/
    if (CrStepperIoctl(Fd, CRSTEPPER_SET_ENABLE, 0) < 0)
    {
        return -1;
    }
    return 0;
}
/**
 * 函数功能: 判断直线运动缓存是否运动完成
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 直线运动缓存是否运动完成,0:未完成,1:完成
 */
BOOL CrBlockIsFinished()
{
    return (IsMotionComplete(LineBuff, &AxisFifo));
}
/**
 * 函数功能: 获取电机是否为使能状态
 * 函数参数: Fd:电机对应描述符
 * 输入: Fd:电机对应描述符
 * 输出: 参考返回值
 * 返回值: 小于0:获取失败,0:未使能,1:使能
 */
BOOL SteppersEnableStatus(S32 Fd)
{
    U8 Status = -1;
    if (CrStepperIoctl(Fd, CRSTEPPER_GET_ENABLE_STATUS, &Status) < 0)
    {
        return -1;
    }
    return Status;
}
/**
 * 函数功能: 关闭电机
 * 函数参数: Fd:电机对应描述符
 * 输入: Fd:电机对应描述符
 * 输出: 操作是否成功
 * 返回值: 小于0:失败,0:成功
 */
S32 SteppersDisable(S32 Fd)
{
    if (CrStepperIoctl( Fd, CRSTEPPER_SET_DISABLE, 0) < 0)
    {
        return -1;
    }
    return 0;
}
/**
 * 函数功能: X,Y,Z回零
 * 函数参数: 无
 * 输入: 无
 * 输出: 归零是否成功
 * 返回值: 0:失败,1:成功
 */
BOOL MoveHome(VOID)
{
    if (MoveHomeX() && MoveHomeY() && MoveHomeZ())
    {
        printf("Home Success!\n");
        return 1; 
    }
    else
    {
        printf("Home Failed!\n");
        return 0;
    }
}

