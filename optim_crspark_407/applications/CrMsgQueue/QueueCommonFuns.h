#ifndef _QUEUECOMMONFUNS_H
#define _QUEUECOMMONFUNS_H

#include "CrInc/CrType.h"
#include "CommonQueue/CrMsgQueue.h"
#include "SharedQueue/CrSharedMsgQueue.h"

#ifdef __cplusplus
extern "C" {
#endif

CrMsgQueue_t QueueOpen(const S8 *Name, S32 Flag, U32 Mode, CrMsgQueueAttr_t *Attr);
CrMsg_t SharedQueueOpen(const S8 *Name, S32 Flag, U32 Mode, CrMsgQueueAttr_t *Attr);
CrMsgQueue_t ViewSendQueueOpen(S8 *Name, S32 Size);
CrMsg_t ViewRecvQueueOpen(S8 *Name, S32 Size);
S32 QueueRecvMsg(CrMsgQueue_t MsgQ, S8 *Buff, S32 Len, S32* Pri);
S32 QueueSendMsg(CrMsgQueue_t MsgQ, S8 *Buff, S32 Len, S32 Pri);

#ifdef __cplusplus
}
#endif

#endif
