/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-15     cx2470       the first version
 */
#include "PrintfSpeed.h"
#include "SpecialDisplay.h"
#include <stdio.h>
#include <string.h>
#include "CrInc/CrMsgType.h"
#include "CrInc/CrConfig.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"


extern struct ViewMotionMsg_t MotionAttr = {0};

static CrMsg_t PrintfSpeedRecv = NULL;
//static CrMsg_t PrintfSpeedSetRecv = NULL;
static CrMsgQueue_t PrintfSpeedSend = InValidMsgQueue();
static CrMsgQueue_t PrintfSpeedSetSend = InValidMsgQueue();

static S32 Tick = 0;
/*Wait after sending successfully  (ms)*/
#define OK_ACTION_SEND_SUCCESS_WAIT   30

VOID PrintfSpeedQueueInit()
{
    PrintfSpeedSetSend = ViewSendQueueOpen((S8 *)MOTION_ATTR_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    PrintfSpeedSend = ViewSendQueueOpen((S8 *)PARAM_MOTION_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
//    PrintfSpeedSetRecv = ViewRecvQueueOpen((S8 *)MOTION_ATTR_REPORT_QUEUE, sizeof(struct ViewMotionMsg_t));
    PrintfSpeedRecv = ViewRecvQueueOpen((S8 *)PARAM_MOTION_REPORT_QUEUE, sizeof(struct ViewMotionMsg_t));
    printf("PrintfSpeedSetSend = %p\n", PrintfSpeedSetSend);
}

static VOID UpdateFrParam()
{
    if ( !IsValidMsgQueue(PrintfSpeedSend) || !PrintfSpeedRecv )
        return ;

    CHECK_TIMEOUT(Tick, rt_tick_get());

    MotionAttr.Action = GET_MOTION_MSG;

    if (QueueSendMsg(PrintfSpeedSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1) != 0 )
        return ;

    CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);

    S8 Buff[sizeof(MotionAttr)] = {0};
    U32 MsgPri = 1;

    if (CrMsgRecv(PrintfSpeedRecv, (S8 *)Buff, sizeof(MotionAttr), &MsgPri) !=  sizeof(MotionAttr))
        return ;

    memcpy(&MotionAttr, Buff, sizeof(MotionAttr));
}

VOID* PrintfSpeedMenuUPAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.PrintRatio < 999 )
        (MotionAttr.MotionParam.PrintRatio)++;

    return (VOID *)&(MotionAttr.MotionParam.PrintRatio);
}

VOID* PrintfSpeedMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( MotionAttr.MotionParam.PrintRatio > 1 )
        (MotionAttr.MotionParam.PrintRatio)--;

    return (VOID *)&(MotionAttr.MotionParam.PrintRatio);
}

VOID* PrintfSpeedMenuOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )

        return NULL;

    S16 Fr = MotionAttr.MotionParam.PrintRatio;

    UpdateFrParam();

    MotionAttr.MotionParam.PrintRatio = Fr;
    MotionAttr.Action = SET_MOTION_MSG;

    S32 Ret = QueueSendMsg(PrintfSpeedSetSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1);
    S32 Result = QueueSendMsg(PrintfSpeedSend, (S8 *)&MotionAttr, sizeof(MotionAttr), 1);

    if ( Ret == 0 || Result == 0 )
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        CrBeepAppShortBeeps();
    }
    rt_kprintf("ret = %d, result = %d\n", Ret, Result);

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return ArgOut;
}

S32 PrintfSpeedMenuShow(VOID *Arg)
{
    UpdateFrParam();

    return SpecialContenDisplay(TUNE_ITEM_SPEED_NAME_ID, MotionAttr.MotionParam.PrintRatio, NULL);
}

S32 PrintfSpeedSpecialDisplay(VOID *Arg)
{
    UpdateFrParam();

    S8 Temp[5] = {0};
    snprintf(Temp, sizeof(Temp), "%d", MotionAttr.MotionParam.PrintRatio);
    Temp[strlen(Temp) + 1] = '\0';

    return SpecialMenuDisplay(Arg, Temp);
}

VOID ClearPrintfSpeedShowTime(VOID)
{
    Tick = 0;
}
