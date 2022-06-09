/*
#include <Menu.h-bak>
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-18     cx2470       the first version
 */
#include "AccelerationMenuItem.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrMsgQueue/CommonQueue/CrMsgQueue.h"
#include "SpecialDisplay.h"
#include <stdio.h>
#include <string.h>
#include "CrInc/CrMsgType.h"
#include "CrInc/CrConfig.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

//速度与parammodel交互和运动的交互
static CrMsg_t ParamAccelRecv = NULL;
static CrMsgQueue_t ParamAccelSend = NULL;
static CrMsgQueue_t MotionAccelSend = NULL;
static S32 PrevTick = 0;

#define OK_ACTION_SEND_SUCCESS_WAIT   20

extern struct ViewMotionMsg_t MotionAttr;

static S32 AcceleGetParm(struct ViewMotionMsg_t *Accele, struct ViewMotionMsg_t *OutAttr)
{
    if ( !Accele || !OutAttr || !IsValidMsgQueue(ParamAccelSend) || !ParamAccelRecv )
        return 0;

    if ( QueueSendMsg(ParamAccelSend, (S8 *)Accele, sizeof(struct ViewMotionMsg_t), 1) != 0 )
        return 0;

    CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);

    U32 MsgPri = 1;
    S8 Buff[sizeof(struct ViewMotionMsg_t)] = {0};
    if ( CrMsgRecv(ParamAccelRecv, Buff, sizeof(Buff), &MsgPri) != sizeof(Buff) )
        return 0;

    memcpy(OutAttr, Buff, sizeof(struct ViewMotionMsg_t));

    return 1;
}

static VOID UpdateAccleParm()
{
    S32 Tick = rt_tick_get();
    /*Check the interval between getting parameters */
    CHECK_TIMEOUT(PrevTick, Tick);

    struct ViewMotionMsg_t Accele = {0};
    MotionAttr.Action = GET_MOTION_MSG;

    if( AcceleGetParm(&MotionAttr, &Accele) > 0)
        memcpy(&MotionAttr, &Accele, sizeof(ViewMotionMsg_t));
}

static VOID SendDataAndWait()
{
    if ( !IsValidMsgQueue(ParamAccelSend) || !IsValidMsgQueue(MotionAccelSend) )
        return ;

    MotionAttr.Action = SET_MOTION_MSG;

    S32 Ret = QueueSendMsg(ParamAccelSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1);
    S32 Result = QueueSendMsg(MotionAccelSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1);

    if ( !Ret || !Result )
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        CrBeepAppShortBeeps(); /* 短鸣 */
    }
}

VOID AccelQueueInit()
{
    ParamAccelSend = InValidMsgQueue();
    MotionAccelSend = InValidMsgQueue();
    MotionAccelSend = ViewSendQueueOpen((S8 *)MOTION_ATTR_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    ParamAccelSend = ViewSendQueueOpen((S8 *)PARAM_MOTION_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    ParamAccelRecv = ViewRecvQueueOpen((S8 *)PARAM_MOTION_REPORT_QUEUE, sizeof(struct ViewMotionMsg_t));
}
/*=========================================================
 *                  Accel Directory function
 *=========================================================*/
S32 AccelespecialDisplay(VOID *Arg)
{
    UpdateAccleParm();

    S8 Temp[6] = {0};
    snprintf(Temp, sizeof(Temp),"%d", MotionAttr.MotionParam.AccelXYZ);

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *AcceleDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.AccelXYZ > 25 )
        MotionAttr.MotionParam.AccelXYZ -= 25;

    return (VOID *)&MotionAttr.MotionParam.AccelXYZ;
}

VOID *AcceleUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.AccelXYZ < 500 )
        MotionAttr.MotionParam.AccelXYZ += 25;

    return (VOID *)&MotionAttr.MotionParam.AccelXYZ;
}

VOID *AcceleOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

     return &MotionAttr;
}

S32 AcceleMenuShow(VOID *Arg)
{
    UpdateAccleParm();

    return SpecialContenDisplay(ACCELERATION_ITEM_ACCEL_NAME_ID, MotionAttr.MotionParam.AccelXYZ, NULL);
}

/*=========================================================
*                  ARetract Directory function
*=========================================================*/
S32 AcceleARetractspecialDisplay(VOID *Arg)
{
    UpdateAccleParm();

    S8 Temp[6] = {0};
    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.StepperEAttr.RetractAccel);
    Temp[strlen(Temp) + 1] = '\0';
    return SpecialMenuDisplay(Arg, Temp);
}

VOID *AcceleARetractDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperEAttr.RetractAccel > 100 )
        (MotionAttr.MotionParam.StepperEAttr.RetractAccel)--;

    return (VOID *)&MotionAttr.MotionParam.StepperEAttr.RetractAccel;
}

VOID *AcceleARetractUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperEAttr.RetractAccel < 500 )
        (MotionAttr.MotionParam.StepperEAttr.RetractAccel)++;

    return (VOID *)&MotionAttr.MotionParam.StepperEAttr.RetractAccel;
}

VOID *AcceleARetractOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

     return &MotionAttr;
}

S32 AcceleARetractMenuShow(VOID *Arg)
{
    UpdateAccleParm();

    return SpecialContenDisplay(ACCELERATION_ITEM_A_RETRACT_NAME_ID, MotionAttr.MotionParam.StepperEAttr.RetractAccel, NULL);
}

/*=========================================================
*                  ATravel Directory function
*=========================================================*/
S32 AcceleATravelspecialDisplay(VOID *Arg)
{
    UpdateAccleParm();

    S8 Temp[6] = {0};
    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.TravelAccelXYZ);

    return SpecialMenuDisplay(Arg, Temp);
}
VOID *AcceleATravelDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.TravelAccelXYZ > 25 )
        MotionAttr.MotionParam.TravelAccelXYZ -= 25;

    return (VOID *)&MotionAttr.MotionParam.TravelAccelXYZ;
}

VOID *AcceleATravelUpAction(VOID *ArgOut, VOID *ArgIn)
{

    if ( MotionAttr.MotionParam.TravelAccelXYZ < 500 )
        MotionAttr.MotionParam.TravelAccelXYZ += 25;

    return (VOID *)&MotionAttr.MotionParam.TravelAccelXYZ;
}

VOID *AcceleATravelOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

     return &MotionAttr;
}

S32   AcceleATravelMenuShow(VOID *Arg)
{
    UpdateAccleParm();

    return SpecialContenDisplay(ACCELERATION_ITEM_A_TRAVEL_NAME_ID, MotionAttr.MotionParam.TravelAccelXYZ, NULL);
}

/*=========================================================
*                  AmaxX Directory function
*=========================================================*/
S32 AcceleAmaxXspecialDisplay(VOID *Arg)
{
    UpdateAccleParm();

    S8 Temp[7] = {0};
    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.StepperXAttr.MaxAccel);

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *AcceleAmaxXDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperXAttr.MaxAccel > 100 )
        MotionAttr.MotionParam.StepperXAttr.MaxAccel -= 25;

    return (VOID *)&MotionAttr.MotionParam.StepperXAttr.MaxAccel;
}

VOID *AcceleAmaxXUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperXAttr.MaxAccel < 65535 )
        MotionAttr.MotionParam.StepperXAttr.MaxAccel += 25;

    return (VOID *)&MotionAttr.MotionParam.StepperXAttr.MaxAccel;
}

VOID *AcceleAmaxXOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

     return &MotionAttr;
}

S32 AcceleAmaxXMenuShow(VOID *Arg)
{
    UpdateAccleParm();

    return SpecialContenDisplay(ACCELERATION_ITEM_AMAX_X_NAME_ID, MotionAttr.MotionParam.StepperXAttr.MaxAccel, NULL);
}

/*=========================================================
*                  AmaxY Directory function
*=========================================================*/
S32 AcceleAmaxYspecialDisplay(VOID *Arg)
{
    UpdateAccleParm();

    S8 Temp[9] = {0};
    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.StepperYAttr.MaxAccel);

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *AcceleAmaxYDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperYAttr.MaxAccel > 100 )
        MotionAttr.MotionParam.StepperYAttr.MaxAccel -= 25;

    return (VOID *)&MotionAttr.MotionParam.StepperYAttr.MaxAccel;
}

VOID *AcceleAmaxYUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperYAttr.MaxAccel < 65535 )
        MotionAttr.MotionParam.StepperYAttr.MaxAccel += 25;

    return (VOID *)&MotionAttr.MotionParam.StepperYAttr.MaxAccel;
}

VOID *AcceleAmaxYOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

     return &MotionAttr;
}

S32  AcceleAmaxYMenuShow(VOID *Arg)
{
    UpdateAccleParm();

    return SpecialContenDisplay(ACCELERATION_ITEM_AMAX_Y_NAME_ID, MotionAttr.MotionParam.StepperYAttr.MaxAccel, NULL);
}

/*=========================================================
*                  AmaxZ Directory function
*=========================================================*/
S32 AcceleAmaxZspecialDisplay(VOID *Arg)
{
    UpdateAccleParm();

    S8 Temp[6] = {0};
    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.StepperZAttr.MaxAccel);

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *AcceleAmaxZDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperZAttr.MaxAccel >= 25 )
        MotionAttr.MotionParam.StepperZAttr.MaxAccel -= 25;

    return (VOID *)&MotionAttr.MotionParam.StepperZAttr.MaxAccel;
}

VOID *AcceleAmaxZUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperZAttr.MaxAccel < 65535 )
        MotionAttr.MotionParam.StepperZAttr.MaxAccel += 25;

    return (VOID *)&MotionAttr.MotionParam.StepperZAttr.MaxAccel;
}

VOID *AcceleAmaxZOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

     return &MotionAttr;
}

S32   AcceleAmaxZMenuShow(VOID *Arg)
{
    UpdateAccleParm();

    return SpecialContenDisplay(ACCELERATION_ITEM_AMAX_Z_NAME_ID, MotionAttr.MotionParam.StepperZAttr.MaxAccel, NULL);
}

/*=========================================================
*                  AmaxE Directory function
*=========================================================*/
S32 AcceleAmaxEspecialDisplay(VOID *Arg)
{
    UpdateAccleParm();

    S8 Temp[6] = {0};
    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.StepperEAttr.Param.MaxAccel);

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *AcceleAmaxEDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperEAttr.Param.MaxAccel > 100 )
        MotionAttr.MotionParam.StepperEAttr.Param.MaxAccel -= 25;

    return (VOID *)&MotionAttr.MotionParam.StepperEAttr.Param.MaxAccel;
}

VOID *AcceleAmaxEUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperEAttr.Param.MaxAccel < 65535 )
        MotionAttr.MotionParam.StepperEAttr.Param.MaxAccel += 25;

    return (VOID *)&MotionAttr.MotionParam.StepperEAttr.Param.MaxAccel;
}

VOID *AcceleAmaxEOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

     return &MotionAttr;
}

S32   AcceleAmaxEMenuShow(VOID *Arg)
{
    UpdateAccleParm();

    return SpecialContenDisplay(ACCELERATION_ITEM_AMAX_E_NAME_ID, MotionAttr.MotionParam.StepperEAttr.Param.MaxAccel, NULL);
}

VOID ClearAccleShowTime(VOID)
{
    PrevTick = 0;
}
