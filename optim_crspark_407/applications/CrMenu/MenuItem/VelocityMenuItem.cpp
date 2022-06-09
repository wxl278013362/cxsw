/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-18     cx2470       the first version
 */
#include "VelocityMenuItem.h"
#include "../Show/ShowAction.h"
#include "SpecialDisplay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CrInc/CrMsgType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrLcdShowStruct.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

//速度与parammodel交互和运动的交互
static CrMsg_t ParamVelRecv = NULL;
static CrMsgQueue_t ParamVelSend = NULL;
static CrMsgQueue_t MotionVelSend = NULL;

extern struct ViewMotionMsg_t MotionAttr;
static S32 PrevTick = 0;

#define OK_ACTION_SEND_SUCCESS_WAIT   20

static S32 VelocityGetParm(struct ViewMotionMsg_t *Accele , struct ViewMotionMsg_t *OutAttr)
{
    if ( !Accele || !OutAttr || !IsValidMsgQueue(ParamVelSend) || !ParamVelRecv )
        return 0;
    if ( QueueSendMsg(ParamVelSend, (S8 *)Accele, sizeof(struct ViewMotionMsg_t), 1) != 0 )
        return 0;

    CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);

    U32 MsgPri = 1;
    S8 Buff[sizeof(struct ViewMotionMsg_t)] = {0};
    if ( CrMsgRecv(ParamVelRecv, Buff, sizeof(Buff), &MsgPri) < 0 )
        return 0;

    memcpy(OutAttr, Buff, sizeof(struct ViewMotionMsg_t));

    return 1;
}

static VOID UpdateVelocityParam()
{
    /*Check the interval between getting parameters */
    CHECK_TIMEOUT(PrevTick, rt_tick_get());

    struct ViewMotionMsg_t Velocity = {0};
    MotionAttr.Action = GET_MOTION_MSG;

    if( VelocityGetParm(&MotionAttr, &Velocity) > 0 )
        memcpy(&MotionAttr, &Velocity, sizeof(struct ViewMotionMsg_t));

}

/*set param and wait process*/
static VOID SendParamAndWait()
{
    if ( !IsValidMsgQueue(ParamVelSend) || !IsValidMsgQueue(MotionVelSend))
        return ;

    MotionAttr.Action = SET_MOTION_MSG;

    S32 Ret = QueueSendMsg(ParamVelSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1);
    S32 Result = QueueSendMsg(MotionVelSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1);

    if ( !Ret || !Result )
    {
        CrBeepAppShortBeeps(); /* 短鸣 */
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
    }

}

VOID VelocityQueueInit()
{
	ParamVelRecv = NULL;
	MotionVelSend = InValidMsgQueue();
	ParamVelSend = InValidMsgQueue();
    MotionVelSend = ViewSendQueueOpen((S8 *)MOTION_ATTR_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    ParamVelSend = ViewSendQueueOpen((S8 *)PARAM_MOTION_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    ParamVelRecv = ViewRecvQueueOpen((S8 *)PARAM_MOTION_REPORT_QUEUE, sizeof(struct ViewMotionMsg_t));
}
/*=========================================================
 *                  Vmax X Directory function
 *=========================================================*/
S32 VelocityVmaxXspecialDisplay(VOID *Arg)
{
    UpdateVelocityParam();

    S8 Temp[8] = {0};
    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.StepperXAttr.MaxVel);

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *VelocityVmaxXDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperXAttr.MaxVel > 1 )
        (MotionAttr.MotionParam.StepperXAttr.MaxVel)--;

    return (VOID *)&MotionAttr.MotionParam.StepperXAttr.MaxVel;
}

VOID *VelocityVmaxXUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperXAttr.MaxVel < 999 )
    (MotionAttr.MotionParam.StepperXAttr.MaxVel)++;

    return (VOID *)&MotionAttr.MotionParam.StepperXAttr.MaxVel;
}

VOID *VelocityVmaxXOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    SendParamAndWait();

    return ArgOut;
}

S32 VelocityVmaxXMenuShow(VOID *Arg)
{
    UpdateVelocityParam();

    return SpecialContenDisplay(VELOCITY_ITEM_VMAX_X_NAME_ID, MotionAttr.MotionParam.StepperXAttr.MaxVel, NULL);
}

/*=========================================================
*                  Vmax Y Directory function
*=========================================================*/
S32 VelocityVmaxYspecialDisplay(VOID *Arg)
{
    UpdateVelocityParam();

    S8 Temp[8] = {0};
    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.StepperYAttr.MaxVel);

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *VelocityVmaxYDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperYAttr.MaxVel > 1 )
        (MotionAttr.MotionParam.StepperYAttr.MaxVel)--;

    return (VOID *)&MotionAttr.MotionParam.StepperYAttr.MaxVel;
}

VOID *VelocityVmaxYUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperYAttr.MaxVel < 999 )
        (MotionAttr.MotionParam.StepperYAttr.MaxVel)++;

    return (VOID *)&MotionAttr.MotionParam.StepperYAttr.MaxVel;
}

VOID *VelocityVmaxYOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    SendParamAndWait();

    return ArgOut;
}

S32 VelocityVmaxYMenuShow(VOID *Arg)
{    
    UpdateVelocityParam();

    return SpecialContenDisplay(VELOCITY_ITEM_VMAX_Y_NAME_ID, MotionAttr.MotionParam.StepperYAttr.MaxVel, NULL);
}

/*=========================================================
*                  Vmax Z Directory function
*=========================================================*/
S32 VelocityVmaxZspecialDisplay(VOID *Arg)
{
    UpdateVelocityParam();

    S8 Temp[8] = {0};

    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.StepperZAttr.MaxVel);

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *VelocityVmaxZDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperZAttr.MaxVel > 1 )
        (MotionAttr.MotionParam.StepperZAttr.MaxVel)--;

    return (VOID *)&MotionAttr.MotionParam.StepperZAttr.MaxVel;
}

VOID *VelocityVmaxZUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperZAttr.MaxVel < 999 )
        (MotionAttr.MotionParam.StepperZAttr.MaxVel)++;

    return (VOID *)&MotionAttr.MotionParam.StepperZAttr.MaxVel;
}

VOID *VelocityVmaxZOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    SendParamAndWait();

    return ArgOut;
}

S32 VelocityVmaxZMenuShow(VOID *Arg)
{    
    UpdateVelocityParam();

    return SpecialContenDisplay(VELOCITY_ITEM_VMAX_Z_NAME_ID, MotionAttr.MotionParam.StepperZAttr.MaxVel, NULL);
}

/*=========================================================
*                  Vmax E Directory function
*=========================================================*/
S32 VelocityVmaxEspecialDisplay(VOID *Arg)
{    
    UpdateVelocityParam();

    S8 Temp[8] = {0};

    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.StepperEAttr.Param.MaxVel);

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *VelocityVmaxEDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperEAttr.Param.MaxVel > 1 )
        (MotionAttr.MotionParam.StepperEAttr.Param.MaxVel)--;

    return (VOID *)&MotionAttr.MotionParam.StepperEAttr.Param.MaxVel;
}

VOID *VelocityVmaxEUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperEAttr.Param.MaxVel < 999 )
        (MotionAttr.MotionParam.StepperEAttr.Param.MaxVel)++;

    return (VOID *)&MotionAttr.MotionParam.StepperEAttr.Param.MaxVel;
}

VOID *VelocityVmaxEOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    SendParamAndWait();

    return ArgOut;
}

S32 VelocityVmaxEMenuShow(VOID *Arg)
{    
    UpdateVelocityParam();

    return SpecialContenDisplay(VELOCITY_ITEM_VMAX_E_NAME_ID, MotionAttr.MotionParam.StepperEAttr.Param.MaxVel, NULL);
}

/*=========================================================
*                  Vmin Directory function
*=========================================================*/
S32 VelocityVminspecialDisplay(VOID *Arg)
{    
    UpdateVelocityParam();

    S8 Temp[8] = {0};

    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.VelMinXYZ);

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *VelocityVminDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.VelMinXYZ > 0 )
        (MotionAttr.MotionParam.VelMinXYZ)--;

    return (VOID *)&MotionAttr.MotionParam.VelMinXYZ;
}

VOID *VelocityVminUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.VelMinXYZ < 999 )
        (MotionAttr.MotionParam.VelMinXYZ)++;

    return (VOID *)&MotionAttr.MotionParam.VelMinXYZ;
}

VOID *VelocityVminOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    SendParamAndWait();

    return ArgOut;
}

S32 VelocityVminMenuShow(VOID *Arg)
{
    UpdateVelocityParam();

    return SpecialContenDisplay(VELOCITY_ITEM_VMIN_NAME_ID, MotionAttr.MotionParam.VelMinXYZ, NULL);
}

/*=========================================================
*                  VTrav Min Directory function
*=========================================================*/
S32 VelocityVTravMinspecialDisplay(VOID *Arg)
{
    UpdateVelocityParam();

    S8 Temp[8] = {0};

    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.TravelVelMinXYZ);

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *VelocityVTravMinDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.TravelVelMinXYZ > 0 )
        (MotionAttr.MotionParam.TravelVelMinXYZ)--;

    return (VOID *)&MotionAttr.MotionParam.TravelVelMinXYZ;
}

VOID *VelocityVTravMinUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.TravelVelMinXYZ < 999 )
        (MotionAttr.MotionParam.TravelVelMinXYZ)++;

    return (VOID *)&MotionAttr.MotionParam.TravelVelMinXYZ;
}

VOID *VelocityVTravMinOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    SendParamAndWait();

    return ArgOut;
}

S32 VelocityVTravMinMenuShow(VOID *Arg)
{
    UpdateVelocityParam();

    return SpecialContenDisplay(VELOCITY_ITEM_VTRAV_MIN_NAME_ID, MotionAttr.MotionParam.TravelVelMinXYZ, NULL);
}

VOID ClearVelocityShowTime(VOID)
{
    PrevTick = 0;
}
