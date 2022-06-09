#ifndef _CRMSGQUEUE_H
#define _CRMSGQUEUE_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"


#ifdef USING_OS_LINUX

//#include <sys/types.h>
//#include <sys/ipc.h>
//#include <sys/msg.h>   //SysV的消息队列

#include <mqueue.h>    //Posix的消息队列
typedef struct timespec CrTimeSpec_t;
typedef mqd_t CrMsgQueue_t;
typedef struct mq_attr CrMsgQueueAttr_t;
typedef struct sigevent CrSigEvent_t;

#endif

#ifdef USING_OS_RTTHREAD
#include <mqueue.h>
typedef struct timespec CrTimeSpec_t;
typedef mqd_t CrMsgQueue_t;
typedef struct mq_attr CrMsgQueueAttr_t;
typedef struct sigevent CrSigEvent_t;

#endif

#ifdef __cplusplus
extern "C" {
#endif

//mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);

// 打开一个消息队列
CrMsgQueue_t CrMsgQueueOpen(const S8 *Name, S32 Flag, U32 Mode, CrMsgQueueAttr_t *Attr);

//关闭消息队列
S32 CrMsgQueueClose(CrMsgQueue_t MsgQ);

//发送一个消息到到消息队列同时指定消息的优先级，MsgPri是优先级
S32 CrMsgQueueSend(CrMsgQueue_t MsgQ, const S8 *MsgBuff, S32 MsgLen, U32 MsgPri);

//超时模式 发送一个消息到到消息队列同时指定消息的优先级，MsgPri是优先级。abs_timeout 指向的结构指定了一个阻塞的软上限时间。这个上限通过从 Epoch 1970-01-01 00:00:00 +0000 (UTC) 开始的绝对的秒和纳秒数指定超时
S32 CrMsgQueueTimedSend(CrMsgQueue_t MsgQ, const S8 *MsgBuff, S32 MsgLen, U32 MsgPri, const CrTimeSpec_t *AbsTimeout);

//接收一个消息并获取消息的优先级，MsgPri是优先级变量指针
S32 CrMsgQueueRecv(CrMsgQueue_t MsgQ, S8 *MsgBuff, S32 MsgLen, U32 *MsgPri);

//超时模式  接收一个消息并获取消息的优先级，MsgPri是优先级变量指针。Timeout 是超时的描述。
S32 CrMsgQueueTimedRecv(CrMsgQueue_t MsgQ, S8 *MsgBuff, S32 MsgLen, U32 *MsgPri, const CrTimeSpec_t *Timeout);

//删除 消息队列
S32 CrMsgQueueUnlink(const S8 *Name);

//注册一个当消息到达时的通知
S32 CrMsgQueueNotify(CrMsgQueue_t MsgQ, const CrSigEvent_t *Notifucation);

//设置消息队列属性，并返回旧的队列属性
S32 CrMsgQueueSetAttr(CrMsgQueue_t MsgQ, const CrMsgQueueAttr_t *NewAttr, CrMsgQueueAttr_t *OldAttr);

//获取消息队列属性
S32 CrMsgQueueGetAttr(CrMsgQueue_t MsgQ, CrMsgQueueAttr_t *Attr);

//队列描述是否有效
S32 IsValidMsgQueue(CrMsgQueue_t MsgQ);

//无效队列的值
CrMsgQueue_t InValidMsgQueue();

#ifdef TEST_MSGQUEUE
VOID TestMsgQ(S8 *Name, S32 Flag, U32 Mode, CrMsgQueueAttr_t *Attr); //测试消息队列接口
#endif

#ifdef __cplusplus
}
#endif

#endif
