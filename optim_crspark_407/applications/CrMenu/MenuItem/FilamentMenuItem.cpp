/*
#include <MenuSetting.h-bak>
#include <MenuDefaultAction.h-bak>
#include <Menu.h-bak>
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-20     cx2470       the first version
 */
#include "FilamentMenuItem.h"
#include "../Action/MenuDefaultAction.h"
#include "../language/Language.h"
#include "CrMsgQueue/CommonQueue/CrMsgQueue.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "../Show/ShowAction.h"
#include "SpecialDisplay.h"
#include <stdio.h>
#include "CrInc/CrMsgType.h"
#include "CrInc/CrConfig.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

//速度与parammodel交互和运动的交互
static CrMsg_t ParamFilamentRecv = NULL;
static CrMsgQueue_t ParamFilamentSend = NULL;
static CrMsgQueue_t MotionFilamentSend = NULL;
#define OK_ACTION_SEND_SUCCESS_WAIT   50

extern struct ViewMotionMsg_t  MotionAttr;
static S32 PrevTick = 0;

VOID FilamentQueueInit()
{
    ParamFilamentSend = InValidMsgQueue();
    MotionFilamentSend = InValidMsgQueue();
    MotionFilamentSend = ViewSendQueueOpen((S8 *)MOTION_ATTR_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    ParamFilamentSend = ViewSendQueueOpen((S8 *)PARAM_MOTION_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    ParamFilamentRecv = ViewRecvQueueOpen((S8 *)PARAM_MOTION_REPORT_QUEUE, sizeof(struct ViewMotionMsg_t));
}

static S32 FilamentGetParm(struct ViewMotionMsg_t *Accele , struct ViewMotionMsg_t *OutAttr)
{
    if ( !Accele ||  !OutAttr || !IsValidMsgQueue(ParamFilamentSend) || !ParamFilamentRecv )
        return 0;

    if ( QueueSendMsg(ParamFilamentSend, (S8 *)Accele, sizeof(struct ViewMotionMsg_t), 1) != 0 )
        return 0;

    CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);

    U32 MsgPri = 1;
    S8 Buff[sizeof(struct ViewMotionMsg_t)] = {0};
    if ( CrMsgRecv(ParamFilamentRecv, Buff, sizeof(Buff), &MsgPri) != sizeof(Buff) )
        return 0;

    memcpy(OutAttr, Buff, sizeof(struct ViewMotionMsg_t));

    return 1;
}

static VOID UpdateFilamentParam()
{
    /*Check the interval between getting parameters */
    CHECK_TIMEOUT(PrevTick, rt_tick_get());

    struct ViewMotionMsg_t Filament = {0};
    MotionAttr.Action = GET_MOTION_MSG;

    if( FilamentGetParm(&MotionAttr, &Filament) > 0 )
        memcpy(&MotionAttr, &Filament, sizeof(struct ViewMotionMsg_t));

    if ( MotionAttr.Diameter < 1.50f )
        MotionAttr.Diameter = 1.50f;

}
/*set param and wait process*/
static VOID SendParamAndWait()
{
    MotionAttr.Action = SET_MOTION_MSG;

    S32 Ret = -1, Result = -1;

    if ( ParamFilamentSend )
        Ret = QueueSendMsg(ParamFilamentSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1);
    if ( MotionFilamentSend )
        Result = QueueSendMsg(MotionFilamentSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1);

    if ( 0 == Ret || 0 == Result )
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        CrBeepAppShortBeeps(); /* 短鸣 */
        ClearFilamentShowTime();
    }
}

static S32 FloatValueTranStr(S8 *Buff, float Value, U16 BufLen)
{
    if ( !Buff )
        return 0;

    S32 TempValue = Value * 1000;
    S32 IntValue = TempValue / 1000;
    S32 SubValue = TempValue % 1000;
    snprintf(Buff, BufLen, "%d.%03d", IntValue, SubValue);

    return 1;
}

S32 FilamentDisplay(VOID *Arg)
{
    UpdateFilamentParam();

    S8 Temp[9] = {0};
    FloatValueTranStr(Temp, MotionAttr.Diameter, sizeof(Temp));

    return SpecialMenuDisplay(Arg, Temp);
}

S32 FilamentMenuDisplay(VOID *Arg)
{
    UpdateFilamentParam();

    S8 String[10] = {0};

    if ( MotionAttr.FilamentPatternAction == 2 )
    {
        Languages::FindMenuItemNameById(OFF_NAME_ID, String, 10);
        HideMask |= FILAMENT_ITEM_FIL_DIA_HID_MASK;
    }
    else
    {
        HideMask &= ~FILAMENT_ITEM_FIL_DIA_HID_MASK;
        Languages::FindMenuItemNameById(ON_NAME_ID, String, 10);
    }

    return SpecialMenuDisplay(Arg, String);
}

VOID *FilamentDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.Diameter > 1.501f )
        MotionAttr.Diameter -= 0.001f;

    return (VOID *)&MotionAttr.Diameter;
}

VOID *FilamentUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.Diameter < 3.250f )
        MotionAttr.Diameter += 0.001f;

    return (VOID *)&MotionAttr.Diameter;
}

VOID *FilamentMenuOKAction(VOID *ArgOut, VOID *ArgIn)
{
    UpdateFilamentParam();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = 0;
    if ( MotionAttr.FilamentPatternAction == 1 )
    {
        HideMask |= FILAMENT_ITEM_FIL_DIA_HID_MASK;
        MotionAttr.FilamentPatternAction = 2;
    }
    else
    {
        HideMask &= ~FILAMENT_ITEM_FIL_DIA_HID_MASK;
        MotionAttr.FilamentPatternAction = 1;
    }

    SendParamAndWait();

    return ArgOut;
}

VOID *FilamentOKAction(VOID *ArgOut, VOID *ArgIn)
{
    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;
    
    SendParamAndWait();

    return ArgOut;
}

S32 FilamentMenuShow(VOID *Arg)
{
    UpdateFilamentParam();

    S8 Temp[6] = {0};
    FloatValueTranStr(Temp, MotionAttr.Diameter, sizeof(Temp));

    return SpecialContenDisplay(FILAMENT_ITEM_FIL_DIA_NAME_ID, MotionAttr.Diameter, Temp);
}

VOID ClearFilamentShowTime(VOID)
{

    if ( MotionAttr.FilamentPatternAction == 2 )
    {
        HideMask |= FILAMENT_ITEM_FIL_DIA_HID_MASK;
    }
    else
    {
        HideMask &= ~FILAMENT_ITEM_FIL_DIA_HID_MASK;
    }

    PrevTick = 0;
}
