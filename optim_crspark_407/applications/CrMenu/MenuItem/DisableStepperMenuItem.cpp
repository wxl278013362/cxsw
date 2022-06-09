/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-16     cx2470       the first version
 */

#include "DisableStepperMenuitem.h"
#include "CrInc/CrMsgType.h"
#include "CrInc/CrConfig.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

static CrMsgQueue_t MoveSend = NULL;

#define STEPPERMENU_PRI 1

S32 DisableStepperMenuInit()
{
    //创建队列
    MoveSend = InValidMsgQueue();
    MoveSend = ViewSendQueueOpen((S8 *)MOTION_MOVE_OPT_QUEUE, sizeof(struct ViewStepperMoveMsg_t));

    return IsValidMsgQueue(MoveSend) ? 1 : 0;
}

VOID *DisableStepperMenuItemOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(MoveSend))
        return 0;

    CrBeepAppShortBeeps(); /* 短鸣 */
        
    S32 *Back = (S32 *)ArgOut;
    *Back = 0;
    
    struct ViewStepperMoveMsg_t DisableStepperMsg = {0};
    DisableStepperMsg.Enable = 2;

    QueueSendMsg(MoveSend, (S8 *)&DisableStepperMsg, sizeof(struct ViewStepperMoveMsg_t), STEPPERMENU_PRI);

    return ArgOut;
}
