#ifndef _CRSHAREDMSGQUEUE_H
#define _CRSHAREDMSGQUEUE_H

#include "../CommonQueue/CrMsgQueue.h"
#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef VOID* CrMsg_t;
typedef CrMsgQueueAttr_t CrMsgAttr_t;

//无阻塞的消息队列
CrMsg_t CrMsgOpen(const S8 *Name, S32 Flag, S32 Mode, CrMsgAttr_t *Attr);   //使用缓冲区
VOID CrMsgClose(CrMsg_t Queue);

//接收一个消息并获取消息的优先级，MsgPri是优先级变量指针
S32 CrMsgRecv(CrMsg_t MsgQ, S8 *MsgBuff, S32 MsgLen, U32 *MsgPri);

//发送一个消息到到消息队列同时指定消息的优先级，MsgPri是优先级
S32 CrMsgSend(CrMsg_t MsgQ, const S8 *MsgBuff, S32 MsgLen, U32 MsgPri);

//队列描述是否有效
//S32 IsEncapMsgQueueValid(MsgQueue_t Queue);

//无效队列的值
//MsgQueue_t InValidEncapMsgQueue();

#ifdef __cplusplus
}
#endif


#endif
