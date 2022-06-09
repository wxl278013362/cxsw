#include "MoveAxisMenuItem.h"
#include "CrView/CrShowView.h"
#include "../Show/LcmShow.h"
#include <rtthread.h>
#include "../Show/ShowAction.h"
#include "SpecialDisplay.h"
#include <stdlib.h>
#include <string.h>
#include "CrInc/CrMsgType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrLcdShowStruct.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

static CrMsgQueue_t MotionMoveSend = NULL;
static float   X_AixsDistance = 0.0f;
static float   Y_AixsDistance = 0.0f;
static float   Z_AixsDistance = 0.0f;
static float   ExtruderDistance = 0.0f;

//struct ViewStepperMoveMsg_t Move;   //保存移动的信息

//struct ViewStepperMoveMsg_t StepperMoveMsg;

static S32 FloatValueTranStr(S8 *Buff, float *Value)
{
    if ( !Buff || !Value )
        return 0;

    S32 IntValue = *Value;
    S32 SubValue = abs((*Value - IntValue) * 100);

    if ( (SubValue % 10) == 9 )
    {
        SubValue += (10 - (SubValue % 10));
        (SubValue == 100 ) ? ((*Value < 0) ? IntValue -= 1 : IntValue += 1) , SubValue = 0 : SubValue; //当SubValue 为99 时加一 会导致显示三位小数
    }

    sprintf(Buff, "%c%d.%02d", ((*Value < 0)?'-':' '), abs(IntValue), SubValue);

    return 1;
}

S32 MoveAxisMenuInit()
{
    //创建队列
    MotionMoveSend = InValidMsgQueue();
    MotionMoveSend = ViewSendQueueOpen((S8 *)MOTION_MOVE_OPT_QUEUE, sizeof(struct ViewStepperMoveMsg_t));
    
    return IsValidMsgQueue(MotionMoveSend) ? 1 : 0;
}

/*******************************X Aixs*********************************/
VOID* X_AixsMaxUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisX = MAX_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_X_AXIS;      //使能移动轴
    
    if( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg
        , sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        X_AixsDistance += MAX_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&X_AixsDistance);
}

VOID* X_AixsMaxUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend) || (X_AixsDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisX = 0 - MAX_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_X_AXIS;          //使能移动轴

    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg
        , sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        X_AixsDistance -= MAX_MOVE_UNIT;
//        if ( X_AixsDistance < 0 )
//        {
//            X_AixsDistance = 0;
//        }
    }

    return (VOID *)(&X_AixsDistance);
}

VOID* X_AixsMidUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisX = MID_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_X_AXIS;      //使能移动轴
    
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg
        , sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >=0 )
    {
        X_AixsDistance += MID_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&X_AixsDistance);
}

VOID* X_AixsMidUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend) || (X_AixsDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisX = 0 - MID_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_X_AXIS;          //使能移动轴

    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg
        , sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        X_AixsDistance -= MID_MOVE_UNIT;
        
//        if ( X_AixsDistance < 0 )
//        {
//            X_AixsDistance = 0;
//        }
    }

    return (VOID *)(&X_AixsDistance);
}

VOID* X_AixsMinUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisX = MIN_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_X_AXIS;      //使能移动轴
    
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg
        , sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        X_AixsDistance += MIN_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&X_AixsDistance);
}

VOID* X_AixsMinUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend) || (X_AixsDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisX = 0 - MIN_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_X_AXIS;          //使能移动轴

    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg, 
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        X_AixsDistance -= MIN_MOVE_UNIT;
//        if ( X_AixsDistance < 0 )
//        {
//            X_AixsDistance = 0;
//        }
    }
    return (VOID *)(&X_AixsDistance);
}

S32 X_AixsMoveMenuItemShow(VOID *Arg)
{
    S8 Temp[12] = {0};
    FloatValueTranStr(Temp, &X_AixsDistance);
    return SpecialContenDisplay(MOVE_AXIS_ITEM_MOVE_X_AXIS_NAME_ID, 0, Temp);
}

VOID* Y_AixsMaxUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg  = {0};
    StepperMoveMsg.MoveDisY = MAX_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Y_AXIS;      //使能移动轴
    
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg, 
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Y_AixsDistance += MAX_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&Y_AixsDistance);
}

VOID* Y_AixsMaxUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend) || (Y_AixsDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    
    StepperMoveMsg.MoveDisY = 0 - MAX_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Y_AXIS;          //使能移动轴

    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg, 
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Y_AixsDistance -= MAX_MOVE_UNIT;
//        if ( Y_AixsDistance < 0 )
//        {
//            Y_AixsDistance = 0;
//        }
    }

    return (VOID *)(&Y_AixsDistance);
}

VOID* Y_AixsMidUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisY = MID_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Y_AXIS;      //使能移动轴
    
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg, 
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Y_AixsDistance += MID_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&Y_AixsDistance);
}

VOID* Y_AixsMidUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend) || (Y_AixsDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    
    StepperMoveMsg.MoveDisY = 0 - MID_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Y_AXIS;          //使能移动轴

    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg, 
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Y_AixsDistance -= MID_MOVE_UNIT;
//        if ( Y_AixsDistance < 0 )
//        {
//            Y_AixsDistance = 0;
//        }
    }

    return (VOID *)(&Y_AixsDistance);
}

VOID* Y_AixsMinUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisY = MIN_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Y_AXIS;      //使能移动轴
    
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Y_AixsDistance += MIN_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&Y_AixsDistance);
}

VOID* Y_AixsMinUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend) || (Y_AixsDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisY = 0 - MIN_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Y_AXIS;          //使能移动轴

    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Y_AixsDistance -= MIN_MOVE_UNIT;
//        if ( Y_AixsDistance < 0 )
//        {
//            Y_AixsDistance = 0;
//        }
    }

    return (VOID *)(&Y_AixsDistance);
}

S32 Y_AixsMoveMenuItemShow(VOID *Arg)
{
    S8 Temp[12] = {0};
    FloatValueTranStr(Temp, &Y_AixsDistance);
    return SpecialContenDisplay(MOVE_AXIS_ITEM_MOVE_Y_AXIS_NAME_ID, 0, Temp);
}

VOID* Z_AixsMaxUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisZ = MAX_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Z_AXIS;      //使能移动轴
    
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg, 
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Z_AixsDistance += MAX_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&Z_AixsDistance);
}

VOID* Z_AixsMaxUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend) || (Z_AixsDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisZ = 0 - MAX_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Z_AXIS;          //使能移动轴

    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Z_AixsDistance -= MAX_MOVE_UNIT;
//        if ( Z_AixsDistance < 0 )
//        {
//            Z_AixsDistance = 0;
//        }
    }

    return (VOID *)(&Z_AixsDistance);
}

VOID* Z_AixsMidUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisZ = MID_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Z_AXIS;      //使能移动轴
    
    if( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Z_AixsDistance += MID_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&Z_AixsDistance);
}

VOID* Z_AixsMidUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend) || (Z_AixsDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisZ = 0 - MID_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Z_AXIS;          //使能移动轴

    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg, 
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Z_AixsDistance -= MID_MOVE_UNIT;
//        if ( Z_AixsDistance < 0 )
//        {
//            Z_AixsDistance = 0;
//        }
    }

    return (VOID *)(&Z_AixsDistance);
}

VOID* Z_AixsMinUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisZ = MIN_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Z_AXIS;      //使能移动轴
    
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Z_AixsDistance += MIN_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&Z_AixsDistance);
}

VOID* Z_AixsMinUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend) || (Z_AixsDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisZ = 0 - MIN_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Z_AXIS;          //使能移动轴
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Z_AixsDistance -= MIN_MOVE_UNIT;
//        if ( Z_AixsDistance < 0 )
//        {
//            Z_AixsDistance = 0;
//        }
    }

    return (VOID *)(&Z_AixsDistance);
}

VOID* Z_AixsLeastUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisZ = Z_MIN_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Z_AXIS;      //使能移动轴

    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Z_AixsDistance += Z_MIN_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&Z_AixsDistance);
}

VOID* Z_AixsLeastUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend) || (Z_AixsDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisZ = 0 - Z_MIN_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_Z_AXIS;          //使能移动轴
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        Z_AixsDistance -= Z_MIN_MOVE_UNIT;
//        if ( Z_AixsDistance < 0 )
//        {
//            Z_AixsDistance = 0;
//        }
    }

    return (VOID *)(&Z_AixsDistance);
}

/* modify by Charley*/
S32 Z_AixsLeastMoveMenuItemShow(VOID *Arg)
{
    S8 Temp[12] = {0};

    S32 IntValue = Z_AixsDistance;
    S32 SubValue = abs((Z_AixsDistance - IntValue) * 1000);

    if ( (SubValue % 100) == 9 )
    {
        SubValue += (100 - (SubValue % 100));
        (SubValue == 1000 ) ? ((Z_AixsDistance < 0) ? IntValue -= 1 : IntValue += 1) , SubValue = 0 : SubValue; //当SubValue 为99 时加一 会导致显示三位小数
    }

    sprintf(Temp, "%c%d.%03d", ((Z_AixsDistance < 0)?'-':' '), abs(IntValue), SubValue);

    return SpecialContenDisplay(MOVE_AXIS_ITEM_MOVE_Z_AXIS_NAME_ID, 0, Temp);
}

S32 Z_AixsMoveMenuItemShow(VOID *Arg)
{
    S8 Temp[12] = {0};
    FloatValueTranStr(Temp, &Z_AixsDistance);
    return SpecialContenDisplay(MOVE_AXIS_ITEM_MOVE_Z_AXIS_NAME_ID, 0, Temp);
}

VOID* ExtruderMaxUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisE = MAX_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_E_AXIS;      //使能移动轴
    
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        ExtruderDistance += MAX_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&ExtruderDistance);
}

VOID* ExtruderMidUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisE = MID_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_E_AXIS;      //使能移动轴
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        ExtruderDistance += MID_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&ExtruderDistance);
}

VOID* ExtruderMinUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisE = MIN_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_E_AXIS;      //使能移动轴
    
    //发送信号
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        ExtruderDistance += MIN_MOVE_UNIT;            //设置增量
    }

    return (VOID *)(&ExtruderDistance);
}

VOID* ExtruderMaxUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend) || (ExtruderDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) || (ExtruderDistance <= 0) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.MoveDisE = 0 - MAX_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_E_AXIS;          //使能移动轴

    //发送信号
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        ExtruderDistance -= MAX_MOVE_UNIT;
//        if ( ExtruderDistance < 0 )
//        {
//            ExtruderDistance = 0;
//        }
    }

    return (VOID *)(&ExtruderDistance);
}

VOID* ExtruderMidUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend)  || (ExtruderDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    
    StepperMoveMsg.MoveDisE = 0 - MID_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_E_AXIS;          //使能移动轴

    //发送信号
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
            sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        ExtruderDistance -= MID_MOVE_UNIT;
//        if ( ExtruderDistance < 0 )
//        {
//            ExtruderDistance = 0;
//        }
    }

    return (VOID *)(&ExtruderDistance);
}

VOID* ExtruderMinUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
//    if ( !IsValidMsgQueue(MotionMoveSend) || (ExtruderDistance <= 0) )
    if ( !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    
    StepperMoveMsg.MoveDisE = 0 - MIN_MOVE_UNIT;    //设置偏移量
    StepperMoveMsg.AxisType = MOVE_E_AXIS;          //使能移动轴

    //发送信号
    if ( QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg,
        sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI) >= 0 )
    {
        ExtruderDistance -= MIN_MOVE_UNIT;
//        if ( ExtruderDistance < 0 )
//        {
//            ExtruderDistance = 0;
//        }
    }

    return (VOID *)(&ExtruderDistance);
}

S32 ExtruderMoveMenuItemShow(VOID *Arg)
{
    S8 Temp[12] = {0};
    FloatValueTranStr(Temp, &ExtruderDistance);
    return SpecialContenDisplay(MOVE_AXIS_ITEM_EXTRUDER_NAME_ID, 0, Temp);
}

VOID* GoHomeMenuItemOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(MotionMoveSend) )
        return NULL;

    CrBeepAppShortBeeps(); /* 短鸣 */
    
    S32 *Back = (S32 *)ArgOut;
    *Back = 0;

    X_AixsDistance = 0;
    Y_AixsDistance = 0;
    Z_AixsDistance = 0;
    
    struct ViewStepperMoveMsg_t StepperMoveMsg = {0};
    StepperMoveMsg.AutoHome = 1;                    //归零使能
    //发送信号
    QueueSendMsg(MotionMoveSend, ( S8 * )&StepperMoveMsg, sizeof(struct ViewStepperMoveMsg_t), MOVEAXISMSG_PRI);
    
    return ArgOut;
}

#if 0
S32 SetAxisPosition(float X_Pos, float Y_Pos, float Z_Pos, float E_Pos)
{
    if ( (X_Pos < 0) || (Y_Pos < 0) || (Z_Pos < 0) || (E_Pos < 0) )
        return 0;

    X_AixsDistance = X_Pos;
    Y_AixsDistance = Y_Pos;
    Z_AixsDistance = Z_Pos;
    ExtruderDistance = E_Pos;

    return 1;
}
#endif
