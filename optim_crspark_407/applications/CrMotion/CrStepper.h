#ifndef _CRSTEPPER_H

#define _CRSTEPPER_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"

#ifdef __cplusplus
extern "C" {
#endif


#if 0
//对电机变量的进行初始化，但是不创建变量（Pin的名称参考Gpio文件中的Pin名称的命名方式）
S32  CrStepperCreate(struct Stepper_t *Stepper, S8 *Name, struct StepperBaseAttr_t *Attr, S8 *EnnPinName, S8 *DirPinName, S8 *EnablePinName);
S32  CrStepperDestory(struct Stepper_t *Stepper);

//电机属性
S32  CrStepperGetBaseAttr(struct Stepper_t *Stepper, struct StepperBaseAttr_t *Attr);
struct StepperMotionExpendAttr_t *CrStepperGetExpendAttr(struct Stepper_t *Stepper);
VOID CrStepperSetExpendAttr(struct Stepper_t *Stepper, struct StepperMotionExpendAttr_t *Attr);
VOID CrStepperSetExpendAttrAccel(struct Stepper_t *Stepper, S32 Accel);   //设置加速加速度
VOID CrStepperSetExpendAttrDecel(struct Stepper_t *Stepper, S32 Decel);   //设置减速加速度 （减速加速度没有范围限制）
VOID CrStepperSetExpendAttrJerk(struct Stepper_t *Stepper, S32 Jerk);    //设置
VOID CrStepperSetExpendAttrMaxVelocity(struct Stepper_t *Stepper, S32 Speed);
VOID CrStepperSetExpendAttrMinVelocity(struct Stepper_t *Stepper, S32 Speed);
VOID CrStepperSetCurPosition(struct Stepper_t *Stepper, S32 Pos);

//执行移动一步的运动过程
S32 CrStepperSetMoveOneStep(struct Stepper_t *Stepper);  //设置移动一步

//电机使能控制
S32  CrStepperSetDisable(struct Stepper_t *Stepper);     //电机失能
S32  CrStepperSetEnable(struct Stepper_t *Stepper);//电机使能
BOOL CrStepperIsEnable(struct Stepper_t *Stepper);    //是否可用
//电机方向控制
VOID CrStepperSetDir(struct Stepper_t *Stepper, S32 Dir);     //控制电机方向

/*
//设置电机的基础参数(注意这些基础参数的设置接口的提供是为了满足不同电机的特异性)
VOID CrStepperSetEnableLevel(struct Stepper_t *Stepper, S32 Level);           //设置使能电平
VOID CrStepperSetPositiveDirLevel(struct Stepper_t *Stepper, S32 Level);            //设置正方向电平
VOID CrStepperSetMicroStep(struct Stepper_t *Stepper, enum MicroStepIndex_e MicroStep);   //设置微步进
VOID CrStepperSetStepAngle(struct Stepper_t *Stepper, float Angle);    //设置步距角
VOID CrStepperSetPerimeter(struct Stepper_t *Stepper, float Perimeter);   //设置周长 （注意：分辨率由周长、步距角和微步进三者组合计算出来）
*/
#else

//电机名称
#define X_STEPPER_NAME                  "AXIS_X"
#define Y_STEPPER_NAME                  "AXIS_Y"
#define Z_STEPPER_NAME                  "AXIS_Z"
#define E_STEPPER_NAME                  "AXIS_E"

//控制命令
#define CRSTEPPER_SET_PINS               0x01  //设置打开的电机的引脚
#define CRSTEPPER_GET_ENABLE_LEVEL       0x02  //获取电机使能电平
#define CRSTEPPER_GET_POSITIVE_DIR_LEVEL 0x03  //获取电机正方向电平
#define CRSTEPPER_SET_ATTR               0x04  //设置电机属性
#define CRSTEPPER_GET_ATTR               0x05  //获取电机属性
#define CRSTEPPER_SET_MOTION_ATTR        0x06  //设置电机运动属性
#define CRSTEPPER_GET_MOTION_ATTR        0x07  //获取电机运动属性
#define CRSTEPPER_SET_POS                0x08  //设置位置
#define CRSTEPPER_GET_POS                0x09  //获取位置
#define CRSTEPPER_SET_POSITIVE_DIR       0x10  //设置电机正方向移动
#define CRSTEPPER_SET_NEGATIVE_DIR       0x11  //设置电机负方向移动
#define CRSTEPPER_SET_ENABLE             0x12  //电机使能
#define CRSTEPPER_SET_DISABLE            0x13  //电机失能
#define CRSTEPPER_SET_MOVE_ONE_IMPLUSE   0x14  //移动一个脉冲
#define CRSTEPPER_GET_NAME               0x15  //获取名字
#define CRSTEPPER_GET_ENABLE_STATUS      0x16  //获取电机使能状态
#define CRSTEPPER_SET_PIN_STATUS         0x17  //设置引脚状态

#if 0
//驱动属性
struct StepperPins_t
{
    /* pin part */
    S8 *EnnPinName;           //使能引脚    （名称使用）
    S8 *DirPinName;           //方向引脚
    S8 *StepPinName;          //步进引脚
    S8 *PdUartName;           //串口名称   （串口使能了后，寄存器就会被使用）

    U8 EnableLevel;           //高使能(1)or低使能(0)
    U8 PositiveDirLevel;      //正方向是高电平(1)or低电平(0)
};


//电机的基础属性
struct StepperAttr_t
{
    U32                        SubDivision; //细分 16 32 64 256
    U32                        StepPerMm;   //每毫米步进数
    FLOAT                      StepAngle;   //步距角
    FLOAT                      Perimeter;   //周长   uint:mm  （电机轴转动一周的长度,若为Z/E轴则表示Z/E轴导程）
    
};

//运动属性
struct StepperMotionAttr_t
{
    S32 Velocity;     //速度
    S32 MaxVelocity;  //最大速度
    S32 MinVelocity;  //最小速度
    S32 Acceleration; //加速度
    S32 Deceleration; //减速加速度
    S32 Jerk;         //Jerk，加速度导数.(加速度变化率)
};

struct StepperPos_t
{
    BOOL EndStopped;      //是否已经归零
    S32  CurPosition;     //当前绝对位置,实际长度
};
#endif
//步进电机
struct Stepper_t
{
    S8                               Name[12]; //电机名称
    struct StepperAttr_t             Attr;
    struct StepperPins_t             Pins;
    struct StepperPos_t              Pos;
    struct StepperMotionAttr_t       MotionAttr;
};

S32 CrStepperOpen(S8 *Name, S32 Flag, S32 Mode);
S32 CrStepperClose(S32 Fd);
S32 CrStepperIoctl(S32 Fd, S32 Cmd, VOID *Arg);

#endif

#ifdef __cplusplus
}
#endif

#endif
