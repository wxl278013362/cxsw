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
#include "StepsMenuItem.h"
#include "../../CrConfig/CrParamConfig.h"
#include "SpecialDisplay.h"
#include <string.h>
#include "CrInc/CrConfig.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

//速度与parammodel交互和运动的交互
static CrMsg_t ParamStepRecv = NULL;
static CrMsgQueue_t ParamStepSend = NULL;
static CrMsgQueue_t MotionStepSend = NULL;

extern struct ViewMotionMsg_t MotionAttr;
static S32 PrevTick = 0;

#define OK_ACTION_SEND_SUCCESS_WAIT   20

VOID StepQueueInit()
{
    ParamStepSend = InValidMsgQueue();
    MotionStepSend = InValidMsgQueue();
    MotionStepSend = ViewSendQueueOpen((S8 *)MOTION_ATTR_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    ParamStepSend = ViewSendQueueOpen((S8 *)PARAM_MOTION_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    ParamStepRecv = ViewRecvQueueOpen((S8 *)PARAM_MOTION_REPORT_QUEUE, sizeof(struct ViewMotionMsg_t));
}

static S32 StepGetParm(struct ViewMotionMsg_t *Step , struct ViewMotionMsg_t *OutAttr)
{
    if ( !Step || !OutAttr || !IsValidMsgQueue(ParamStepSend) || !ParamStepRecv )
        return 0;

    S32 Ret = QueueSendMsg(ParamStepSend, (S8 *)Step, sizeof(struct ViewMotionMsg_t), 1);
    if ( Ret != 0 )
        return 0;

    CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
    U32 MsgPri = 1;

    S8 Buff[sizeof(struct ViewMotionMsg_t)] = {0};
    if ( CrMsgRecv(ParamStepRecv, Buff, sizeof(Buff), &MsgPri) < 0)
        return 0;

    memcpy(OutAttr, Buff, sizeof(struct ViewMotionMsg_t));

    return 1;
}

static VOID UpdateStepsParm()
{
    /*Check the interval between getting parameters */
    CHECK_TIMEOUT(PrevTick, rt_tick_get());

    struct ViewMotionMsg_t Accele = {0};
    MotionAttr.Action = GET_MOTION_MSG;

    if( StepGetParm(&MotionAttr, &Accele) > 0)
        memcpy(&MotionAttr, &Accele, sizeof(ViewMotionMsg_t));

}

static VOID SendDataAndWait()
{
    if ( !IsValidMsgQueue(ParamStepSend) ||  !IsValidMsgQueue(MotionStepSend))
        return ;

    MotionAttr.Action = SET_MOTION_MSG;

    S32 Ret = QueueSendMsg(ParamStepSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1);
    S32 Result = QueueSendMsg(MotionStepSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1);

    if ( !Ret || !Result )
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        CrBeepAppShortBeeps(); /* 短鸣 */
    }
}
static S32 FloatValueTranStr(S8 *Buff, float Value, U16 BufLen)
{
    if ( !Buff )
        return 0;

    S32 TempValue = Value * 10;
    S32 IntValue = TempValue / 10;
    S32 SubValue = TempValue % 10;
    snprintf(Buff, BufLen, "%d.%d", IntValue, SubValue);

    return 1;
}

/*=========================================================
 *                  X Steps Directory function
 *=========================================================*/
S32 XstepsSpecialDisplay(VOID *Arg)
{
    UpdateStepsParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperXAttr.StepsPerMm, sizeof(Temp));

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *XstepsDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperXAttr.StepsPerMm  > 5.0f )
        MotionAttr.MotionParam.StepperXAttr.StepsPerMm -= 0.1f ;

    return (VOID *)&MotionAttr.MotionParam.StepperXAttr.StepsPerMm;
}

VOID *XstepsUpAction(VOID *ArgOut, VOID *ArgIn)
{
    MotionAttr.MotionParam.StepperXAttr.StepsPerMm += 0.1f;
    return (VOID *)&MotionAttr.MotionParam.StepperXAttr.StepsPerMm;
}

VOID *XstepsOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return ArgOut;
}

S32 XstepsMenuShow(VOID *Arg)
{
    UpdateStepsParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperXAttr.StepsPerMm, sizeof(Temp));

    return SpecialContenDisplay(STEPS_ITEM_X_NAME_ID, MotionAttr.MotionParam.StepperXAttr.StepsPerMm, Temp);
}

/*=========================================================
 *                  Y Steps Directory function
 *=========================================================*/
S32 YstepsSpecialDisplay(VOID *Arg)
{
    UpdateStepsParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperYAttr.StepsPerMm, sizeof(Temp));

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *YstepsDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperYAttr.StepsPerMm > 5.0f )
        MotionAttr.MotionParam.StepperYAttr.StepsPerMm -= 0.1f;

    return (VOID *)&MotionAttr.MotionParam.StepperYAttr.StepsPerMm;
}

VOID *YstepsUpAction(VOID *ArgOut, VOID *ArgIn)
{
    MotionAttr.MotionParam.StepperYAttr.StepsPerMm += 0.1f;

    return (VOID *)&MotionAttr.MotionParam.StepperYAttr.StepsPerMm;
}

VOID *YstepsOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return ArgOut;
}

S32 YstepsMenuShow(VOID *Arg)
{
    UpdateStepsParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperYAttr.StepsPerMm, sizeof(Temp));

    return SpecialContenDisplay(STEPS_ITEM_Y_NAME_ID, MotionAttr.MotionParam.StepperYAttr.StepsPerMm, Temp);
}

/*=========================================================
 *                  Z Steps Directory function
 *=========================================================*/
S32 ZstepsSpecialDisplay(VOID *Arg)
{
    UpdateStepsParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperZAttr.StepsPerMm, sizeof(Temp));

    return SpecialMenuDisplay(Arg, Temp);
}

VOID *ZstepsDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperZAttr.StepsPerMm > 5.0f )
        MotionAttr.MotionParam.StepperZAttr.StepsPerMm -= 0.1f;

    return (VOID *)&MotionAttr.MotionParam.StepperZAttr.StepsPerMm;
}

VOID *ZstepsUpAction(VOID *ArgOut, VOID *ArgIn)
{
    MotionAttr.MotionParam.StepperZAttr.StepsPerMm += 0.1f;

    return (VOID *)&MotionAttr.MotionParam.StepperZAttr.StepsPerMm;
}

VOID *ZstepsOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return ArgOut;
}

S32 ZstepsMenuShow(VOID *Arg)
{
    UpdateStepsParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperZAttr.StepsPerMm, sizeof(Temp));

    return SpecialContenDisplay(STEPS_ITEM_Z_NAME_ID, MotionAttr.MotionParam.StepperZAttr.StepsPerMm, Temp);
}

/*=========================================================
 *                  E Steps Directory function
 *=========================================================*/
S32 EstepsSpecialDisplay(VOID *Arg)
{
    UpdateStepsParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperEAttr.Param.StepsPerMm, sizeof(Temp));

    return SpecialMenuDisplay(Arg, Temp);
}
VOID *EstepsDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.StepperEAttr.Param.StepsPerMm > 5.0f )
        MotionAttr.MotionParam.StepperEAttr.Param.StepsPerMm -= 0.1f;

    return (VOID *)&MotionAttr.MotionParam.StepperEAttr.Param.StepsPerMm;
}

VOID *EstepsUpAction(VOID *ArgOut, VOID *ArgIn)
{
    MotionAttr.MotionParam.StepperEAttr.Param.StepsPerMm += 0.1f;

    return (VOID *)&MotionAttr.MotionParam.StepperEAttr.Param.StepsPerMm;
}
VOID *EstepsOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    SendDataAndWait();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return ArgOut;
}

S32 EstepsMenuShow(VOID *Arg)
{
    UpdateStepsParm();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.MotionParam.StepperEAttr.Param.StepsPerMm, sizeof(Temp));

    return SpecialContenDisplay(STEPS_ITEM_E_NAME_ID, MotionAttr.MotionParam.StepperEAttr.Param.StepsPerMm, Temp);
}

VOID ClearStepsShowTime(VOID)
{
    PrevTick = 0;
}
