#include "QueueCommonFuns.h"
#include "Common/Include/CrTime.h"

CrMsgQueue_t QueueOpen(const S8 *Name, S32 Flag, U32 Mode, CrMsgQueueAttr_t *Attr)
{
    if ( !Attr || !Name )
        return NULL;

    CrMsgQueue_t Queue = InValidMsgQueue();
    if ( Flag & O_CREAT )
    {
        S32 TemperFlag = Flag & (~O_CREAT);
        TemperFlag |= O_EXCL;
        Queue = CrMsgQueueOpen(Name, TemperFlag, Mode, Attr);
        if ( Queue == InValidMsgQueue() )
        {
            Queue = CrMsgQueueOpen(Name, Flag, Mode, Attr);
        }
    }
    else
    {
        Queue = CrMsgQueueOpen(Name, Flag, Mode, Attr);
        if ( Queue == InValidMsgQueue() )
        {
            Flag &= ~O_EXCL;   //去掉创建属性
            Flag |= O_CREAT;
            Queue = CrMsgQueueOpen(Name, Flag, Mode, Attr);
        }
    }

    return Queue;
}

CrMsg_t SharedQueueOpen(const S8 *Name, S32 Flag, U32 Mode, CrMsgQueueAttr_t *Attr)
{
    if ( !Attr || !Name )
        return NULL;

    CrMsg_t Queue = NULL;
    if ( Flag & O_CREAT )
    {
        //添加存在性判断
        S32 TemperFlag = Flag & (~O_CREAT);
        TemperFlag |= O_EXCL;
        Queue = CrMsgOpen(Name, TemperFlag, Mode, Attr);
        if ( Queue == NULL )
        {
            Queue = CrMsgOpen(Name, Flag, Mode, Attr);
        }
    }
    else
    {
        Queue = CrMsgOpen(Name, Flag, Mode, Attr);
        if ( Queue == NULL )
        {
            Flag &= ~O_EXCL;   //去掉创建属性
            Flag |= O_CREAT;
            Queue = CrMsgOpen(Name, Flag, Mode, Attr);
        }
    }

    return Queue;
}

CrMsgQueue_t ViewSendQueueOpen(S8 *Name, S32 Size)
{
    CrMsgQueue_t Queue = NULL;
    if ( !Name || Size <= 0 )
        return Queue;

    CrMsgQueueAttr_t Attr;
    Attr.mq_flags = O_NONBLOCK;
    Attr.mq_maxmsg = 1;
    Attr.mq_msgsize = Size;
    Queue = QueueOpen(Name, O_CREAT | O_RDWR, 0x0777, &Attr);

    return Queue;
}

CrMsg_t ViewRecvQueueOpen(S8 *Name, S32 Size)
{
    if ( !Name || Size <= 0 )
        return NULL;

    CrMsgQueueAttr_t Attr;
    Attr.mq_flags = O_NONBLOCK;
    Attr.mq_maxmsg = 1;
    Attr.mq_msgsize = Size;
    return SharedQueueOpen(Name, O_CREAT | O_RDWR, 0x0777, &Attr);
}

S32 QueueSendMsg(CrMsgQueue_t MsgQ, S8 *Buff, S32 Len, S32 Pri)
{
    if ( !IsValidMsgQueue(MsgQ) || !Buff || (Len <= 0) )
        return -1;

    CrTimeSpec_t Time;
    CrGetCurrentSystemTime(&Time);
    //Time.tv_sec += 1;
    Time.tv_nsec += 1000;
    S32 Res = CrMsgQueueTimedSend(MsgQ, Buff, Len, Pri, &Time);
//    S32 Res = CrMsgQueueSend(MsgQ, Buff, Len, Pri);

    return Res;
}


S32 QueueRecvMsg(CrMsgQueue_t MsgQ, S8 *Buff, S32 Len, S32* Pri)
{
    if ( !IsValidMsgQueue(MsgQ) || !Buff || (Len <= 0) || !Pri)
        return -1;

    CrTimeSpec_t Time;
    CrGetCurrentSystemTime(&Time);
    Time.tv_nsec += 10000;
    S32 Res = CrMsgQueueTimedRecv(MsgQ, Buff, Len, (U32 *)Pri, &Time);

    return Res;
}
