#include "ParamMenuItem.h"
#include "AccelerationMenuItem.h"
#include "FilamentMenuItem.h"
#include "FlowMenuItem.h"
#include "JerkMenuItem.h"
#include "PreheatMaterialMenuitem.h"
#include "PrintfSpeed.h"
#include "StepsMenuItem.h"
#include "TemperatureMenuItem.h"
#include "VelocityMenuItem.h"
#include <stdio.h>
#include "CrInc/CrMsgType.h"
#include "CrInc/CrConfig.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

static CrMsgQueue_t ParamOptSend;

#define PARAMMSG_PRI 1

static VOID ClearShowTime(VOID)
{
    ClearAccleShowTime();
    ClearFilamentShowTime();
    ClearFlowShowTime();
    ClearJerkShowTime();
    ClearPreheatShowTime();
    ClearPrintfSpeedShowTime();
    ClearStepsShowTime();
    ClearTemptureShowTime();
    ClearVelocityShowTime();
}

S32 ParamOptMenuInit()
{
    //创建队列
    ParamOptSend = InValidMsgQueue();
    ParamOptSend = ViewSendQueueOpen(PARAM_OPT_SET_QUEUE, sizeof(struct ViewParamOptActionMsg_t));

    return IsValidMsgQueue(ParamOptSend) ? 1 : 0;
}

VOID* ParamSaveOkAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(ParamOptSend)  )
    {
        return (VOID*)NULL;
    }

    CrBeepAppLongBeeps(); /* 长鸣 */
    ClearShowTime();
    struct ViewParamOptActionMsg_t ParamOptMsg;
    ParamOptMsg.Action = SAVE_ALL_TO_FILE;
    QueueSendMsg(ParamOptSend, ( S8 * )&ParamOptMsg, sizeof(struct ViewParamOptActionMsg_t), PARAMMSG_PRI);

    return ArgOut;
}

VOID* ParamLoadOkAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(ParamOptSend)  )
    {
        return (VOID*)NULL;
    }    
    CrBeepAppLongBeeps(); /* 长鸣 */
    ClearShowTime();
    struct ViewParamOptActionMsg_t ParamOptMsg;
    ParamOptMsg.Action = LOAD_ALL_FORM_FILE;
    QueueSendMsg(ParamOptSend, ( S8 * )&ParamOptMsg, sizeof(struct ViewParamOptActionMsg_t), PARAMMSG_PRI);

    return ArgOut;
}

VOID* ParamRecoverOkAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(ParamOptSend)  )
    {
        return (VOID*)NULL;
    }
    CrBeepAppLongBeeps(); /* 长鸣 */
    ClearShowTime();
    struct ViewParamOptActionMsg_t ParamOptMsg;
    ParamOptMsg.Action = LOAD_ALL_DEFAULT_PARAM;
    QueueSendMsg(ParamOptSend, ( S8 * )&ParamOptMsg, sizeof(struct ViewParamOptActionMsg_t), PARAMMSG_PRI);

    return ArgOut;
}


VOID* ParamPlaSaveOkAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(ParamOptSend)  )
    {
        return (VOID*)NULL;
    }
    CrBeepAppLongBeeps(); /* 长鸣 */
    ClearShowTime();
    struct ViewParamOptActionMsg_t ParamOptMsg;
    ParamOptMsg.Action = SAVE_PLA_PARAM;
    QueueSendMsg(ParamOptSend, ( S8 * )&ParamOptMsg, sizeof(struct ViewParamOptActionMsg_t), PARAMMSG_PRI);

    return ArgOut;
}

VOID* ParamPlaLoadOkAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(ParamOptSend)  )
    {
        return (VOID*)NULL;
    }
    CrBeepAppLongBeeps(); /* 长鸣 */
    ClearShowTime();
    struct ViewParamOptActionMsg_t ParamOptMsg;
    ParamOptMsg.Action = LOAD_PLA_PARAM;
    QueueSendMsg(ParamOptSend, ( S8 * )&ParamOptMsg, sizeof(struct ViewParamOptActionMsg_t), PARAMMSG_PRI);
    return ArgOut;
}

VOID* ParamPlaRecoverOkAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(ParamOptSend)  )
    {
        return (VOID*)NULL;
    }
    CrBeepAppLongBeeps(); /* 长鸣 */
    ClearShowTime();
    struct ViewParamOptActionMsg_t ParamOptMsg;
    ParamOptMsg.Action = LOAD_PLA_DEFAULT_PARAM;
    QueueSendMsg(ParamOptSend, ( S8 * )&ParamOptMsg, sizeof(struct ViewParamOptActionMsg_t), PARAMMSG_PRI);

    return ArgOut;
}

VOID* ParamAbsSaveOkAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(ParamOptSend)  )
    {
        return (VOID*)NULL;
    }

    CrBeepAppLongBeeps(); /* 长鸣 */
    ClearShowTime();
    struct ViewParamOptActionMsg_t ParamOptMsg;
    ParamOptMsg.Action = SAVE_ABS_PARAM;
    QueueSendMsg(ParamOptSend, ( S8 * )&ParamOptMsg, sizeof(struct ViewParamOptActionMsg_t), PARAMMSG_PRI);

    return ArgOut;
}

VOID* ParamAbsLoadOkAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(ParamOptSend)  )
    {
        return (VOID*)NULL;
    }
    CrBeepAppLongBeeps(); /* 长鸣 */
    ClearShowTime();
    struct ViewParamOptActionMsg_t ParamOptMsg;
    ParamOptMsg.Action = LOAD_ABS_PARAM;
    QueueSendMsg(ParamOptSend, ( S8 * )&ParamOptMsg, sizeof(struct ViewParamOptActionMsg_t), PARAMMSG_PRI);

    return ArgOut;
}

VOID* ParamAbsRecoverOkAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(ParamOptSend)  )
    {
        return (VOID*)NULL;
    }
    CrBeepAppLongBeeps(); /* 长鸣 */
    ClearShowTime();

    struct ViewParamOptActionMsg_t ParamOptMsg;
    ParamOptMsg.Action = LOAD_ABS_DEFAULT_PARAM;
    QueueSendMsg(ParamOptSend, ( S8 * )&ParamOptMsg, sizeof(struct ViewParamOptActionMsg_t), PARAMMSG_PRI);

    return ArgOut;
}
