/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-15     cx2470       the first version
 */
#include "FlowMenuItem.h"
#include "SpecialDisplay.h"
#include <stdio.h>
#include <string.h>
#include "CrInc/CrMsgType.h"
#include "CrInc/CrConfig.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

static struct ViewMotionMsg_t Folw = {0};

static CrMsg_t FlowRecv = NULL;
//static CrMsg_t PrintfSpeedSetRecv = NULL;
static CrMsgQueue_t FlowSend = InValidMsgQueue();
static CrMsgQueue_t FlowSetSend = InValidMsgQueue();

static S32 Tick = 0;
/*Wait after sending successfully  (ms)*/
#define OK_ACTION_SEND_SUCCESS_WAIT   20

/*interval time (ms)*/
#define UPDATE_INFO_TIMEOUT     2000

VOID FlowQueueInit()
{
    FlowSetSend = ViewSendQueueOpen((S8 *)MOTION_ATTR_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    FlowSend = ViewSendQueueOpen((S8 *)PARAM_MOTION_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
//    PrintfSpeedSetRecv = ViewRecvQueueOpen((S8 *)MOTION_ATTR_REPORT_QUEUE, sizeof(struct ViewMotionMsg_t));
    FlowRecv = ViewRecvQueueOpen((S8 *)PARAM_MOTION_REPORT_QUEUE, sizeof(struct ViewMotionMsg_t));
}

static VOID UpdateFlowParam()
{
    if ( !IsValidMsgQueue(FlowSend) || !FlowRecv )
        return ;

    CHECK_TIMEOUT(Tick, rt_tick_get());

    Folw.Action = GET_MOTION_MSG;

    if ( QueueSendMsg(FlowSend, (S8 *)&Folw, sizeof(Folw), 1) != 0 )
        return ;

    CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);

    S8 Buff[sizeof(Folw)] = {0};
    U32 MsgPri = 1;

    if (CrMsgRecv(FlowRecv, (S8 *)Buff, sizeof(Folw), &MsgPri) !=  sizeof(Folw))
        return ;

    memcpy(&Folw, Buff, sizeof(Folw));
}

VOID* FlowMenuUPAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( Folw.MotionParam.FlowRatio < 999 )
        (Folw.MotionParam.FlowRatio)++;


    return (VOID*)&(Folw.MotionParam.FlowRatio);

}

VOID* FlowMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( Folw.MotionParam.FlowRatio > 1 )
        (Folw.MotionParam.FlowRatio)--;


    return (VOID*)&(Folw.MotionParam.FlowRatio);

}

VOID* FlowMenuOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )

        return NULL;

    S16 FlowRatio = Folw.MotionParam.FlowRatio;

    UpdateFlowParam();

    Folw.MotionParam.FlowRatio = FlowRatio;
    Folw.Action = SET_MOTION_MSG;

    S32 Ret = QueueSendMsg(FlowSetSend, (S8 *)&Folw, sizeof(Folw), 1);
    S32 Result = QueueSendMsg(FlowSend, (S8 *)&Folw, sizeof(Folw), 1);

    if ( !Ret || !Result )
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        CrBeepAppShortBeeps(); /* 短鸣 */
    }

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return ArgOut;
}

S32 FlowMenuShow(VOID *Arg)
{
    UpdateFlowParam();

    return SpecialContenDisplay(TUNE_ITEM_FLOW_NAME_ID, Folw.MotionParam.FlowRatio, NULL);
}

S32 FlowSpecialDisplay(VOID *Arg)
{
    UpdateFlowParam();

    S8 Temp[5] = {0};
    snprintf(Temp, sizeof(Temp), "%d", Folw.MotionParam.FlowRatio);
    Temp[strlen(Temp) + 1] = '\0';

    return SpecialMenuDisplay(Arg, Temp);
}

VOID ClearFlowShowTime(VOID)
{
    Tick = 0;
}

