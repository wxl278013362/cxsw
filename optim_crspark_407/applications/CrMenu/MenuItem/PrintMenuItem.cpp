#include "PrintMenuItem.h"
#include "CrView/CrShowView.h"
#include "CrInc/CrMsgType.h"
#include "CrInc/CrConfig.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

static S32  PrintFlag = PRINT_READY;
static CrMsgQueue_t PrintOptSend;

#define PRINTMSG_PRI 1

S32 PrintOptMenuInit()
{
    //创建队列
    PrintOptSend = InValidMsgQueue();
    PrintOptSend = ViewSendQueueOpen((S8 *)PRINT_PRINTOPT_QUEUE, sizeof(struct ViewPrintCmdMsg_t));

    return IsValidMsgQueue(PrintOptSend) ? 1 : 0;
}

VOID* StartPrintAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(PrintOptSend)  )
        return NULL;

    S32 *Back = (S32 *)ArgOut;
    *Back = BACK_ROOT;
    
    ViewPrintCmdMsg_t  PrintCmdMsg;  
    PrintCmdMsg.Action = CMD_START;
    if ( 0 != QueueSendMsg(PrintOptSend, (S8 * )&PrintCmdMsg, sizeof(struct ViewPrintCmdMsg_t), PRINTMSG_PRI) )
        return (VOID *)&PrintFlag;

    CrM_Sleep(50);
    CrBeepAppShortBeeps(); /* 短鸣 */
    HideMask |= (MAINMENU_ITEM_PRINTFFROMTF_HID_MASK | MAINMENU_ITEM_PREPARE_HID_MASK\
            | MAINMENU_ITEM_RECOVER_PARAM_HID_MASK | MAINMENU_ITEM_LOAD_PARAM_HID_MASK);
    HideMask &= ~(MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK | MAINMENU_ITEM_STOP_PRINTF_HID_MASK | MAINMENU_ITEM_TUNE_HID_MASK);

    return (VOID *)&PrintFlag;
}

VOID* StopPrintAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(PrintOptSend) )
        return NULL;

    S32 *Back = (S32 *)ArgOut;
    *Back = BACK_ROOT;

    ViewPrintCmdMsg_t  PrintCmdMsg;  
    PrintCmdMsg.Action = CMD_STOP;

    if ( 0 != QueueSendMsg(PrintOptSend, (S8 * )&PrintCmdMsg, sizeof(struct ViewPrintCmdMsg_t), PRINTMSG_PRI) )
        return (VOID *)&PrintFlag;

    CrM_Sleep(50);

    CrBeepAppShortBeeps(); /* 短鸣 */
    HideMask |= (MAINMENU_ITEM_STOP_PRINTF_HID_MASK | MAINMENU_ITEM_RESUME_PRINTF_HID_MASK | MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK | MAINMENU_ITEM_TUNE_HID_MASK);
    HideMask &= ~MAINMENU_ITEM_PRINTFFROMTF_HID_MASK;
    HideMask &= ~MAINMENU_ITEM_PREPARE_HID_MASK;
    HideMask &= ~MAINMENU_ITEM_RECOVER_PARAM_HID_MASK;
    HideMask &= ~MAINMENU_ITEM_LOAD_PARAM_HID_MASK;

    return (VOID *)&PrintFlag;
}

VOID* PausePrintAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut || !IsValidMsgQueue(PrintOptSend) )
        return NULL;

    S32 *Back = (S32 *)ArgOut;
    *Back = BACK_ROOT;

    ViewPrintCmdMsg_t  PrintCmdMsg;  
    PrintCmdMsg.Action = CMD_PAUSE;
    if ( 0 != QueueSendMsg(PrintOptSend, (S8 * )&PrintCmdMsg, sizeof(struct ViewPrintCmdMsg_t), PRINTMSG_PRI) )
        return (VOID *)&PrintFlag;

    CrM_Sleep(50);
    CrBeepAppShortBeeps(); /* 短鸣 */
    HideMask |= MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK;
    HideMask &= ~MAINMENU_ITEM_RESUME_PRINTF_HID_MASK;

    return (VOID *)&PrintFlag;
}

VOID* ContinuePrintAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    S32 *Back = (S32 *)ArgOut;
    *Back = BACK_ROOT;

    ViewPrintCmdMsg_t  PrintCmdMsg;  
    PrintCmdMsg.Action = CMD_RESUME;
    if ( 0 != QueueSendMsg(PrintOptSend, (S8 * )&PrintCmdMsg, sizeof(struct ViewPrintCmdMsg_t), PRINTMSG_PRI) )
        return (VOID *)&PrintFlag;

    CrM_Sleep(50);
    CrBeepAppShortBeeps(); /* 短鸣 */
    HideMask &= ~MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK;
    HideMask |= MAINMENU_ITEM_RESUME_PRINTF_HID_MASK;

    return (VOID *)&PrintFlag;
}

/*
S32 GetPrintStatus()
{
    return PrintFlag;
}
*/


