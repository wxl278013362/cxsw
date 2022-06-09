#include "CrSharedMsgQueue.h"
#include "Common/Include/CrTime.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "Map.h"
#include "Common/Include/CrMutex.h"

struct SharedMsgQueue_t
{

    S8   UsingBuff; //使用缓冲区
    S8   *DataBuff; //数据缓冲区，存放最新得的数据（一个消息的长度的缓冲区）
    S32  DataLen;
    S8   *Name;
    CrMsgQueue_t     Queue;
    CrPthreadMutex_t *Mutex;
};

//无阻塞的消息队列
CrMsg_t CrMsgOpen(const S8 *Name, S32 Flag, S32 Mode, CrMsgAttr_t *Attr)
{
    if ( !Name || !Attr)
        return NULL;

//    Attr->mq_flags = O_NONBLOCK;
    Flag |= O_NONBLOCK;
    CrMsgQueue_t Queue = CrMsgQueueOpen(Name, Flag, Mode, Attr);
    //printf("create queue fd = %d\n", (int)Queue);
    if ( !IsValidMsgQueue(Queue) )
        return NULL;

    struct QueueBuffAttr_t BuffAttr;
    if ( !CrQueueBuffInit((S8 *)Name, Attr->mq_msgsize, &BuffAttr) )
    {
        CrMsgQueueClose(Queue);
         return NULL;
    }

    struct SharedMsgQueue_t *EncapQueue = (struct SharedMsgQueue_t *)malloc(sizeof(struct SharedMsgQueue_t));
    if ( !EncapQueue )
    {
        CrMsgQueueClose(Queue);
        CrQueueBuffUnInit(BuffAttr.QueueName);
        return NULL;
    }

    EncapQueue->DataBuff = BuffAttr.Data;
    EncapQueue->DataLen = BuffAttr.DataLen;
    EncapQueue->UsingBuff = 1;
    EncapQueue->Queue = Queue;
    EncapQueue->Mutex = BuffAttr.Mutex;
    EncapQueue->Name = BuffAttr.QueueName;

    return (CrMsg_t)EncapQueue;
}

VOID CrMsgClose(CrMsg_t Queue)
{
    if ( !Queue )
        return;

    struct SharedMsgQueue_t *EncapQueue = (struct SharedMsgQueue_t *)Queue;
    CrMsgQueueClose(EncapQueue->Queue);
    CrQueueBuffUnInit(EncapQueue->Name);

    EncapQueue->Queue = InValidMsgQueue();
    EncapQueue->DataBuff = NULL;
    EncapQueue->DataLen = 0;
    EncapQueue->UsingBuff = 0;
    EncapQueue->Mutex = NULL;
   
    EncapQueue->Name = NULL;

    return;
}

//接收一个消息并获取消息的优先级，MsgPri是优先级变量指针
S32 CrMsgRecv(CrMsg_t MsgQ, S8 *MsgBuff, S32 MsgLen, U32 *MsgPri)
{
    if ( !MsgQ || !MsgBuff || !MsgPri)
        return -1;
    
    struct SharedMsgQueue_t *Queue = (struct SharedMsgQueue_t *)MsgQ;
    if ( !IsValidMsgQueue(Queue->Queue) )
        return -1;

    CrTimeSpec_t  Time;
    CrGetCurrentSystemTime(&Time);
    Time.tv_nsec += 200000;  //将时间延迟200us
    S32 Res = CrMsgQueueTimedRecv(Queue->Queue, MsgBuff, MsgLen, MsgPri, &Time);
    if ( Res < 0 )
    {
        //接收失败，将上次接收的内容拷贝给消息缓冲区
//        rt_kprintf("queue usingbuffflag = %d, buff = %p, len = %d\n", Queue->UsingBuff, Queue->DataBuff, Queue->DataLen);
        if ( Queue->UsingBuff && Queue->DataBuff )
        {
            CrPthreadMutexLock(Queue->Mutex);
            if ( Queue->DataLen < MsgLen )
                MsgLen = Queue->DataLen;

            memcpy(MsgBuff, Queue->DataBuff, MsgLen);
            CrPthreadMutexUnLock(Queue->Mutex);
        }
        else
        {
            return -1;
        }
    }
    else
    {
        //接收消息成功，将内容进行备份
        if ( Queue->UsingBuff && Queue->DataBuff )
        {
            CrPthreadMutexLock(Queue->Mutex);
            S32 Size = MsgLen;
            if ( Queue->DataLen < MsgLen )
                Size = Queue->DataLen;

            memcpy(Queue->DataBuff, MsgBuff, Size);
            CrPthreadMutexUnLock(Queue->Mutex);
        }
    }


    return MsgLen;
}

//发送一个消息到到消息队列同时指定消息的优先级，MsgPri是优先级
S32 CrMsgSend(CrMsg_t MsgQ, const S8 *MsgBuff, S32 MsgLen, U32 MsgPri)
{
    if ( !MsgQ || !MsgBuff )
        return -1;

    struct SharedMsgQueue_t *Queue = (struct SharedMsgQueue_t *)MsgQ;
    if ( !IsValidMsgQueue(Queue->Queue) )
        return -1;

//    printf("start send data : %s\n", MsgBuff);
    CrTimeSpec_t  Time;
    CrGetCurrentSystemTime(&Time);
    Time.tv_nsec += 200000;  //将时间延迟200us
    S32 Res = CrMsgQueueTimedSend(Queue->Queue, MsgBuff, MsgLen, MsgPri, &Time);
//    printf("send msg result : %d\n", Res);
    if ( Res < 0 )
    {
//        printf("send data errno = %d, errstr : %s, send failed data : %s\n", errno, strerror(errno), MsgBuff);
        if ( !(Queue->UsingBuff) || !(Queue->DataBuff) )    //是否可以缓冲，如果不能缓冲就返回
            return -1;
        
        //发送失败，将队列的内容进行一次出队操作
        S8 Data[Queue->DataLen];
        U32 Pri = 0;
        Res = CrMsgRecv(MsgQ, Data, Queue->DataLen, &Pri);  //内容自动备份
        if ( Res < 0 )
            return -1;

//        printf("recv data success data : %s\n", Data);
        CrGetCurrentSystemTime(&Time);
        Time.tv_nsec += 200000;  //将时间延迟200us
        S32 Res = CrMsgQueueTimedSend(Queue->Queue, MsgBuff, MsgLen, MsgPri, &Time);
//        printf("send data Res  = %d\n", Res);
        if ( Res < 0 )
            return -1;
    }

    return MsgLen;
}

//队列描述是否有效
//S32 IsEncapMsgQueueValid(MsgQueue_t Queue);

//无效队列的值
//MsgQueue_t InValidEncapMsgQueue();

