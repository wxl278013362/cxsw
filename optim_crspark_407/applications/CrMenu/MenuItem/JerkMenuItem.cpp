/*
#include <Menu.h-bak>
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-19     cx2470       the first version
 */
#include "JerkMenuItem.h"
#include "SpecialDisplay.h"
#include <stdio.h>
#include <string.h>
#include "CrInc/CrMsgType.h"
#include "CrInc/CrConfig.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

//速度与parammodel交互和运动的交互
static CrMsg_t ParamJerkRecv = NULL;
static CrMsgQueue_t ParamJerkSend = NULL;
static CrMsgQueue_t MotionJerkSend = NULL;

#define OK_ACTION_SEND_SUCCESS_WAIT   30

extern struct ViewMotionMsg_t MotionAttr;
static S32 PrevTick = 0;

VOID JerkQueueInit()
{
    ParamJerkSend = InValidMsgQueue();
    MotionJerkSend = InValidMsgQueue();
    MotionJerkSend = ViewSendQueueOpen((S8 *)MOTION_ATTR_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    ParamJerkSend = ViewSendQueueOpen((S8 *)PARAM_MOTION_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    ParamJerkRecv = ViewRecvQueueOpen((S8 *)PARAM_MOTION_REPORT_QUEUE, sizeof(struct ViewMotionMsg_t));
}

static S32  JerkGetParm(struct ViewMotionMsg_t *Jerk , struct ViewMotionMsg_t *OutAttr)
{
    if ( !Jerk || !OutAttr || !IsValidMsgQueue(ParamJerkSend) || !ParamJerkRecv )
        return 0;

    if ( QueueSendMsg(ParamJerkSend, (S8 *)Jerk, sizeof(struct ViewMotionMsg_t), 1) != 0 )
        return 0;

    CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);

    U32 MsgPri = 1;
    S8 Buff[sizeof(struct ViewMotionMsg_t)] = {0};
    if ( CrMsgRecv(ParamJerkRecv, Buff, sizeof(Buff), &MsgPri) < 0)
        return 0;

    memcpy(OutAttr, Buff, sizeof(struct ViewMotionMsg_t));

    return 1;
}

static S32 FloatValueTranStr(S8 *Buff, float Value, U16 BufLen)
{
    if ( !Buff )
        return 0;

    S32 IntValue = Value;
    S32 SubValue = (Value - IntValue) * 100;
    snprintf(Buff, BufLen, "+%03d.%02d", IntValue, SubValue);

    return 1;
}

static VOID UpdateJerkParm()
{
    /*Check the interval between getting parameters */
    CHECK_TIMEOUT(PrevTick, rt_tick_get());

    struct ViewMotionMsg_t Accele = {0};
    MotionAttr.Action = GET_MOTION_MSG;

    if( JerkGetParm(&MotionAttr, &Accele) > 0)
        memcpy(&MotionAttr, &Accele, sizeof(ViewMotionMsg_t));

}

static VOID SendDataAndWait()
{
    if (!IsValidMsgQueue(ParamJerkSend) || !IsValidMsgQueue(MotionJerkSend) )
        return ;

    MotionAttr.Action = SET_MOTION_MSG;

    S32 Ret = QueueSendMsg(ParamJerkSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1);
    S32 Result = QueueSendMsg(MotionJerkSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1);

    if ( !Ret || !Result )
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        CrBeepAppShortBeeps(); /* 短鸣 */
    }
}
/*=========================================================
*                   VX JerkDirectory function
*=========================================================*/
S32 VxJerkspecialDisplay(VOID *Arg)
{
    UpdateJerkParm();

    S8 Temp[7] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperXAttr.Jerk, sizeof(Temp));

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *VxJerkDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperXAttr.Jerk > 1 )
        MotionAttr.MotionParam.StepperXAttr.Jerk -= 1;

    return (VOID *)&MotionAttr.MotionParam.StepperXAttr.Jerk;
}

VOID *VxJerkUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperXAttr.Jerk < 990 )
        MotionAttr.MotionParam.StepperXAttr.Jerk += 1;

    return (VOID *)&MotionAttr.MotionParam.StepperXAttr.Jerk;
}

VOID *VxJerkOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return ArgOut;
}

S32 VxJerkMenuShow(VOID *Arg)
{
    UpdateJerkParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperXAttr.Jerk, sizeof(Temp));

    return SpecialContenDisplay(JERK_ITEM_X_NAME_ID, 0, Temp);

}
/*=========================================================
*                   VY Jerk Directory function
*=========================================================*/
S32   VyJerkspecialDisplay(VOID *Arg)
{
    UpdateJerkParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperYAttr.Jerk, sizeof(Temp));

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *VyJerkDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperYAttr.Jerk > 1 )
        MotionAttr.MotionParam.StepperYAttr.Jerk -= 1;

    return (VOID *)&MotionAttr.MotionParam.StepperYAttr.Jerk;
}

VOID *VyJerkUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperYAttr.Jerk < 990 )
        MotionAttr.MotionParam.StepperYAttr.Jerk += 1;

    return (VOID *)&MotionAttr.MotionParam.StepperYAttr.Jerk;
}

VOID *VyJerkOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return ArgOut;
}

S32   VyJerkMenuShow(VOID *Arg)
{
    UpdateJerkParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperYAttr.Jerk, sizeof(Temp));

    return SpecialContenDisplay(JERK_ITEM_Y_NAME_ID, 0, Temp);
}
/*=========================================================
*                   VZ JerkDirectory function
*=========================================================*/
S32   VzJerkspecialDisplay(VOID *Arg)
{
    UpdateJerkParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperZAttr.Jerk, sizeof(Temp));

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *VzJerkDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperZAttr.Jerk > 0.10f )
        MotionAttr.MotionParam.StepperZAttr.Jerk -= 0.01f;

    return (VOID *)&MotionAttr.MotionParam.StepperZAttr.Jerk;
}

VOID *VzJerkUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperZAttr.Jerk < 990.00f )
        MotionAttr.MotionParam.StepperZAttr.Jerk += 0.01f;

    return (VOID *)&MotionAttr.MotionParam.StepperZAttr.Jerk;
}

VOID *VzJerkOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return ArgOut;
}

S32   VzJerkMenuShow(VOID *Arg)
{
    UpdateJerkParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperZAttr.Jerk, sizeof(Temp));

    return SpecialContenDisplay(JERK_ITEM_Z_NAME_ID, 0, Temp);
}
/*=========================================================
*                   VE Jerk Directory function
*=========================================================*/
S32   VeJerkspecialDisplay(VOID *Arg)
{
    UpdateJerkParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperEAttr.Param.Jerk, sizeof(Temp));

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *VeJerkDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperEAttr.Param.Jerk > 0.10f )
        MotionAttr.MotionParam.StepperEAttr.Param.Jerk -= 0.01f;

    return (VOID *)&MotionAttr.MotionParam.StepperEAttr.Param.Jerk;
}

VOID *VeJerkUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperEAttr.Param.Jerk < 990.00f )
        MotionAttr.MotionParam.StepperEAttr.Param.Jerk += 0.01f;

    return (VOID *)&MotionAttr.MotionParam.StepperEAttr.Param.Jerk;
}

VOID *VeJerkOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return ArgOut;
}

S32   VeJerkMenuShow(VOID *Arg)
{
    UpdateJerkParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperEAttr.Param.Jerk, sizeof(Temp));

    return SpecialContenDisplay(JERK_ITEM_E_NAME_ID, 0, Temp);
}

VOID ClearJerkShowTime(VOID)
{
    PrevTick = 0;
}
