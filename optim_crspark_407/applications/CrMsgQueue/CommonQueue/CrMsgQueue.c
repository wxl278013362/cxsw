#include "CrMsgQueue.h"


CrMsgQueue_t CrMsgQueueOpen(const S8 *Name, S32 Flag, U32 Mode, CrMsgQueueAttr_t *Attr)
{
    return mq_open(Name, Flag, Mode, Attr);
}

S32 CrMsgQueueClose(CrMsgQueue_t MsgQ)
{
    return mq_close(MsgQ);
}

S32 CrMsgQueueSend(CrMsgQueue_t MsgQ, const S8 *MsgBuff, S32 MsgLen, U32 MsgPri)
{
    return mq_send(MsgQ, MsgBuff, MsgLen, MsgPri);
}

S32 CrMsgQueueTimedSend(CrMsgQueue_t MsgQ, const S8 *MsgBuff, S32 MsgLen, U32 MsgPri, const CrTimeSpec_t *AbsTimeout)
{
    return mq_timedsend(MsgQ, MsgBuff, MsgLen, MsgPri, AbsTimeout);
}

S32 CrMsgQueueRecv(CrMsgQueue_t MsgQ, S8 *MsgBuff, S32 MsgLen, U32 *MsgPri)
{
    return mq_receive(MsgQ, MsgBuff, MsgLen, MsgPri);
}

S32 CrMsgQueueTimedRecv(CrMsgQueue_t MsgQ, S8 *MsgBuff, S32 MsgLen, U32 *MsgPri, const CrTimeSpec_t *Timeout)
{
    return mq_timedreceive(MsgQ, MsgBuff, MsgLen, MsgPri, Timeout);
}

S32 CrMsgQueueUnlink(const S8 *Name)
{
    return mq_unlink(Name);
}

S32 CrMsgQueueNotify(CrMsgQueue_t MsgQ, const CrSigEvent_t *Notifucation)
{
    return mq_notify(MsgQ, Notifucation);
}

S32 CrMsgQueueSetAttr(CrMsgQueue_t MsgQ, const CrMsgQueueAttr_t *NewAttr, CrMsgQueueAttr_t *OldAttr)
{
    return mq_setattr(MsgQ, NewAttr, OldAttr);
}

S32 CrMsgQueueGetAttr(CrMsgQueue_t MsgQ, CrMsgQueueAttr_t *Attr)
{
    return mq_getattr(MsgQ, Attr);
}

S32 IsValidMsgQueue(CrMsgQueue_t MsgQ)
{
    return MsgQ != InValidMsgQueue() ? 1 : 0;
}

CrMsgQueue_t InValidMsgQueue()
{
    return NULL;
}


#ifdef TEST_MSGQUEUE

#include <rtthread.h>
#include <string.h>
#include <time.h>

struct Info_t
{
    S8  Name[10];
    S32 Type;
    S32 Cmd;
};
VOID TestMsgQ(S8 *Name, S32 Flag, U32 Mode, CrMsgQueueAttr_t *Attr)
{
   CrMsgQueue_t MsgQ = CrMsgQueueOpen(Name, Flag, Mode, Attr);
   if ( MsgQ ==  NULL)
   {
       rt_kprintf("open msg queue : %s failed!\n\r", Name);
       return;
    }

    rt_kprintf("open msg queue : %s success!\n\r", Name);
    S8  MsgBuff[20] = "hello world";
    S32 Size = CrMsgQueueSend(MsgQ, MsgBuff, 14, 1);
    if ( Size < 0 )
    {
        rt_kprintf("send msg : %s failed!\n\r", MsgBuff);
    }
    else
    {
       rt_kprintf("send msg : %s success, size : %d!\n\r", MsgBuff, Size);
    }

    S8  MsgBuff1[20] = "message is aaa";
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    rt_kprintf("%.24s %ld Nanoseconds\n", ctime(&ts.tv_sec), ts.tv_nsec);

    CrTimeSpec_t Time;
//    Time.tv_sec = 2147483648000;
    Time.tv_sec = 10 + ts.tv_sec;
    Time.tv_nsec = 10;
    rt_kprintf("sec = %d, current sec = %d, errorno = %d\n", Time.tv_sec, ts.tv_sec, rt_get_errno());
    if ( CrMsgQueueTimedSend(MsgQ, MsgBuff1, 13, 1, &Time) < 0 )
    {
        rt_kprintf("send msg : %s failed or timeout, errorno = %d , sec = %d!\n\r", MsgBuff1, rt_get_errno(), Time.tv_sec);
    }
    else
    {
       rt_kprintf("send msg : %s success!\n\r", MsgBuff1);
    }

    CrMsgQueueAttr MsgAttr;
    if ( CrMsgQueueGetAttr(MsgQ, &MsgAttr) < 0 )
    {
       rt_kprintf("get msg queue attr failed!\n\r");
    }
    else
    {
        rt_kprintf("get msg queue attr : flag = %d, maxmsg = %d, msgsize = %d curmsg = %d!\n\r"
                    , MsgAttr.mq_flags, MsgAttr.mq_maxmsg, MsgAttr.mq_msgsize, MsgAttr.mq_curmsgs);
    }

    S32 Data = 100000;
    if ( CrMsgQueueSend(MsgQ, &Data, sizeof(S32), 1) >= 0 )
    {
        rt_kprintf("send data : %d!\n\r", Data);
    }

    struct Info_t Info;
    strcpy(&Info, "hello");
    Info.Type = 10;
    Info.Cmd = 1000;
    if ( CrMsgQueueSend(MsgQ, &Info, sizeof(Info), 1) >= 0 )
    {
        rt_kprintf("send struct name : %s, type : %d, Cmd : %d!\n\r", Info.Name, Info.Type, Info.Cmd);
    }

    S8  RecvBuff[20] = {0};
    U32 MsgPri;
    Size = CrMsgQueueRecv(MsgQ, RecvBuff, 19, &MsgPri);
    if ( Size < 0 )
    {
       rt_kprintf("recv msg : %s failed!\n\r", RecvBuff);
    }
    else
    {
       rt_kprintf("recv msg : %s success, size = %d!\n\r", RecvBuff, Size);
    }

    memset(RecvBuff, 0, sizeof(RecvBuff));
    Size = CrMsgQueueRecv(MsgQ, RecvBuff, 19, &MsgPri);
    if ( Size < 0 )
    {
       rt_kprintf("recv msg : %s failed!\n\r", RecvBuff);
    }
    else
    {
       rt_kprintf("recv msg : %s success, size = %d!\n\r", RecvBuff, Size);
    }

    S32 RecvData = 0;
    if ( CrMsgQueueRecv(MsgQ, &RecvData, sizeof(RecvData), &MsgPri) >= 0 )
    {
        rt_kprintf("recv data : %d !\n\r", RecvData);
    }

    memset(&Info, 0, sizeof(Info));
    if ( CrMsgQueueRecv(MsgQ, &Info, sizeof(Info), &MsgPri) >= 0 )
    {
        rt_kprintf("recv struct name : %s, type : %d, Cmd : %d!\n\r", Info.Name, Info.Type, Info.Cmd);
    }

#if 0
    S8  RecvBuff1[20] = {0};
    Time.tv_sec = 2;
    if ( CrMsgQueueTimedRecv(MsgQ, RecvBuff1, 19, &MsgPri, &Time) < 0 )
    {
           rt_kprintf("recv msg : %s failed or timeout, errno = %d!\n\r", RecvBuff1, rt_get_errno());
    }
    else
    {
            rt_kprintf("recv msg : %s success!\n\r", RecvBuff1);
    }

    CrMsgQueueAttr NewAttr, OldAttr;
    NewAttr.mq_flags = 0;
    NewAttr.mq_maxmsg = 10;
    NewAttr.mq_msgsize = 100;
    NewAttr.mq_curmsgs = 0;
    if ( CrMsgQueueSetAttr(MsgQ, &NewAttr, &OldAttr) < 0 )
    {
       rt_kprintf("set msg queue attr failed, errorno = %d !\n\r", rt_get_errno());
    }
    else
    {
       rt_kprintf("msg queue old attr : flag = %d, maxmsg = %d, msgsize = %d curmsg = %d!\n\r"
               , NewAttr.mq_flags, NewAttr.mq_maxmsg, NewAttr.mq_msgsize, NewAttr.mq_curmsgs);
    }
#endif

    if ( CrMsgQueueUnlink(Name) < 0 )
    {
       rt_kprintf("unlink msg queue %s failed!\n\r", Name);
    }
    else
    {
       rt_kprintf("unlink msg queue %s success!\n\r", Name);
    }

    if ( CrMsgQueueClose(MsgQ) < 0 )
    {
       rt_kprintf("close msg queue failed!\n\r");
    }
    else
    {
        rt_kprintf("close msg queue success!\n\r");
    }
}
//RTM_EXPORT(TestMsgQ);

#endif
