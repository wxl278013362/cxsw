
#ifndef _STEPPERAPI_H_

#define _STEPPERAPI_H_

#include "CrMotionPlanner.h"
#include "CrStepper.h"

#ifdef __cplusplus
extern "C"
{
#endif

//enum Axis_t
//{
//    X,
//    Y,
//    Z,
//    E,
//    AxisNum,
//};

//X轴限位开关缓冲距离
#define HOME_X_OFFSET           2//  //2
//Y轴限位开关缓冲距离
#define HOME_Y_OFFSET           2//  //4
//Z轴限位开关缓冲距离
#define HOME_Z_OFFSET           2//  //2

//X轴最值
#define X_AXIS_MAX_SIZE         (220)
#define X_AXIS_MIN_SIZE         (0)

//Y轴最值
#define Y_AXIS_MAX_SIZE         (220)
#define Y_AXIS_MIN_SIZE         (0)

//Z轴最值
#define Z_AXIS_MAX_SIZE         (250)
#define Z_AXIS_MIN_SIZE         (0)


#define TRUE        (1)
#define FALSE       (0)
#define SAFE_DELAY_MS  (1)

VOID StepperInit(S32 Stepper_Fd[], S32 EndStop_Fd[], U32 AxisNum);
BOOL MoveHome();
BOOL MoveHomeX();
BOOL MoveHomeY();
BOOL MoveHomeZ();
BOOL IsHome();
BOOL IsHomeX();
BOOL IsHomeY();
BOOL IsHomeZ();


S32 StepperSetAttr(struct StepperAttr_t *Attr,enum Axis_t Axis);
S32 StepperGetAttr(struct StepperAttr_t *Attr,enum Axis_t Axis);

S32 StepperSetMotionAttr(struct StepperMotionAttr_t *Attr,enum Axis_t Axis);
S32 StepperGetMotionAttr(struct StepperMotionAttr_t *Attr,enum Axis_t Axis);

VOID StepperSetFeedrateAttr(struct FeedrateAttr_t *Attr);
VOID StepperGetFeedrateAttr(struct FeedrateAttr_t *Attr);

S32 StepperSetCurCoordinate(struct        Coordinate3d_t *Pos);
S32 StepperGetCurCoordinate(struct        Coordinate3d_t *Pos);


S32 StepperSetPos(struct StepperPos_t *Pos,enum Axis_t Axis);
S32 StepperGetPos(struct StepperPos_t *Pos,enum Axis_t Axis);

S32 SteppersStopQuick();
S32 SteppersStopSafely();
S32 SteppersEnable(S32 Fd);
S32 SteppersDisable(S32 Fd);
BOOL SteppersEnableStatus(S32 Fd);

BOOL CrBlockIsFinished();

/************************ Debug Log *************************************/
VOID PrintCoordinate(struct Coordinate3d_t *Pos);

#ifdef __cplusplus
}
#endif

#endif 
