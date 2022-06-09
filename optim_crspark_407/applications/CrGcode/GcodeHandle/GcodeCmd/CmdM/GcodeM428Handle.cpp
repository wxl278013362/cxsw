#include "GcodeM428Handle.h"
#include <stdio.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrPrinter.h"
#include "CrModel/CrGcode.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrInc/CrMsgType.h"
#include "CrInc/CrMsgStruct.h"

static CrMsg_t LineMsgQueue = NULL;

#define CMD_M428 ("M428")
extern struct ShowPrintInfo_t ShowInfo;

VOID GcodeM428Init()
{
    LineMsgQueue = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));

    return ;
}

S32 GcodeM428Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    if ( strncmp(CmdBuff, CMD_M428, strlen(CMD_M428)) != 0)
        return 0;

    rt_kprintf("%s\n", CmdBuff);

    return 1;
}

VOID UnHomedErr()
{
    if ( ShowInfo.HomeX && ShowInfo.HomeY && ShowInfo.HomeZ )
        return ;

    struct MsgLine_t LineMsg = {0};
    LineMsg.MsgType = SPECIAL_MSG;

    sprintf(LineMsg.Info, "Home %s%s%s First", ShowInfo.HomeX ? "" : "X",\
                ShowInfo.HomeY ? "": "Y", ShowInfo.HomeZ ? "": "Z" );

    if ( LineMsgQueue )
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);

    LineMsg.MsgType = SPECIAL_MSG;
    strcpy(LineMsg.Info, "");
    LineMsg.Finished = 1;
    if ( LineMsgQueue )
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
}

S32 GcodeM428Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    UnHomedErr();

    /*FIXME:增加原点设置*/

    return 1;
}

S32 GcodeM428Reply(VOID *ReplyResult, S8 *Buff)
{

    return 0;
}
