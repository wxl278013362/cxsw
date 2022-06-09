#include "CrGcode.h"
#include <string.h>
#include <stdio.h>
#include "Common/Include/CrSleep.h"
#include "Common/Include/CrMutex.h"
#include "Common/Include/CrTime.h"
#include "CrGcode/GcodeHandle/GcodeHandle.h"
#include "CrGcode/GcodeCmdGet/GcodeCmdGet.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrLog/CrLog.h"
#include "CrInc/CrConfig.h"

#define SPECIFIC_CMD_PRI   10

#define QUEUE_MAX_MSG            (8)
#define GCODE_REPLAY_BUFFF_SIZE  (20)
#define CMD_EXEC_ATTEMPTS        (3)   //GCODE 命令执行错误尝试次数



static S8* GcodeQueueName = NULL;
CrGcode* GcodeModel = NULL;

VOID *CrGcode::CmdThread(VOID* Arg)
{
    S32 Ret = 1;
    if ( !Arg )
    {
        CrPthreadExit((VOID*)&Ret);
    }

    CrGcode *Thread = (CrGcode *)Arg;

    while(1)
    {
        Thread->Exec();
        sched_yield();
    }

    delete Thread;

    CrPthreadExit((VOID*)&Ret);
}

S8 *CrGcode::GetThreadBuffAddr(S32 &BuffLen)
{
    BuffLen = sizeof(ThreadBuff);

    return ThreadBuff;
}

CrGcode::CrGcode(S8 *QueueName)
    : CrModel(),
      Waiting(0)
{
    GcodeQueueName = QueueName;
    CmdQueue = InValidMsgQueue();

    memset(ThreadBuff, THREAD_BYTE_INIT_VALUE, sizeof(ThreadBuff));

    if ( GcodeQueueName )
    {
        S32 QueueFlag = O_CREAT | O_RDWR;

        CrMsgQueueAttr_t Attr;
        Attr.mq_flags = O_NONBLOCK;
        Attr.mq_msgsize = QUEUE_MSG_SIZE;
        Attr.mq_maxmsg = QUEUE_MAX_MSG;

        CmdQueue = QueueOpen(GcodeQueueName, QueueFlag, 0x777, &Attr);
        if ( !CmdQueue )
        {
            rt_kprintf("CrGcode msg queue failed!\n");
        }

        CrPthreadAttr_t AttrThread;
        memset(&AttrThread, 0, sizeof(AttrThread));
        CrPthreadAttrInit(&AttrThread);
        AttrThread.stackaddr = ThreadBuff;
        AttrThread.stacksize = sizeof(ThreadBuff);
//        AttrThread.schedparam.sched_priority = RT_MAIN_THREAD_PRIORITY;

        CrPthreadAttrSetDetachState(&AttrThread, PTHREAD_CREATE_DETACHED);
        //pth01
        S32 Res = CrPthreadCreate(&GcodeThread, &AttrThread, CrGcode::CmdThread, this);
        if ( Res != 0 )
        {
            rt_kprintf("%s thread create fail, errno = %d\n", GcodeQueueName, errno);
            return ;
        }
        CrPthreadAttrDestroy(&AttrThread);
    }

    /********************* extened ************************/
    CmdBuffHead = CmdBuffTail = 0;
    CmdCount = 0;
    memset(CmdBuff,0,sizeof(CmdBuff));
}

CrGcode::~CrGcode()
{
    rt_kprintf("~CrGcode has be called\n");
    if ( IsValidMsgQueue(CmdQueue) )
    {
        if ( GcodeQueueName )
            CrMsgQueueUnlink(GcodeQueueName);
        CrMsgQueueClose(CmdQueue);

        CmdQueue = InValidMsgQueue();
    }
    if ( GcodeThread >= 0 )
    {
        VOID* Result = NULL;

        CrPthreadKill(GcodeThread, 10);
        CrPthreadJoin(GcodeThread, &Result);
    }
        GcodeThread = -1;
}

VOID CrGcode::Exec()            //将gcode命令收取，存储至内部队列
{
    if ( !IsValidMsgQueue(CmdQueue) || Waiting )   //在等待就不做任何事情
        return ;

    S32 Pri = SPECIFIC_CMD_PRI;
    S8 Buff[QUEUE_MSG_SIZE] = {0};

    while( !CrCmdQueueIsFull() )
    {
        if ( RecvMsg(CmdQueue, Buff, QUEUE_MSG_SIZE, &Pri) <= 0 )
            break ;

        memcpy(CmdBuff + CmdBuffTail, Buff, QUEUE_MSG_SIZE);
        /* offset tail index.*/
        CmdBuffTail = NextCmdIndex(CmdBuffTail);
        CmdCount ++;
    }

    /* handle gcode cmd.*/
    S32 ret = -1;
    while ( !CrCmdQueueIsEmpty() )
    {
        if ( Waiting )
            break;

        GcodeHandleStatus = GCODE_EXEC_PROCESS;
        ret = CrGcodehandle((S8 *)(CmdBuff + CmdBuffHead), strlen((S8 *)(CmdBuff + CmdBuffHead)));
        if (ret == GCODE_EXEC_OK)
        {
            memset(CmdBuff + CmdBuffHead, 0, QUEUE_MSG_SIZE);
            CmdBuffHead = NextCmdIndex(CmdBuffHead);
            if ( CmdCount > 0 )
                CmdCount --;
            GcodeHandleStatus = GCODE_EXEC_OK;
        }
        else if (ret == GCODE_EXEC_WAIT)
        {
            /* pause gcode handle until condition fulfilled.*/
            GcodeHandleStatus = GCODE_EXEC_WAIT;
            CrM_Sleep(5);
            break;
        }
//        if ( ret == GCODE_CMD_ERROR )
//        {
//            memset(CmdBuff + CmdBuffHead, 0, QUEUE_MSG_SIZE);
//            CmdBuffHead = NextCmdIndex(CmdBuffHead);
//            if ( CmdCount > 0 )
//                CmdCount --;
//            break;
//        }
        else if ( ret == CrErr)
        {
            /* gcode cmd handle failed.*/
            //rt_kprintf("Gcode Cmd:%s CrErr,next Cmd\n",(S8 *)(CmdBuff + CmdBuffHead));
            //LOG("Gcode Cmd:%s CrErr,next Cmd\n",(S8 *)(CmdBuff + CmdBuffHead));
            memset(CmdBuff + CmdBuffHead, 0, QUEUE_MSG_SIZE);
            CmdBuffHead = NextCmdIndex(CmdBuffHead);
            if ( CmdCount > 0 )
                CmdCount --;
            GcodeHandleStatus = GCODE_EXEC_OK;
            break;
        }
        else
        {
            //rt_kprintf("Gcode Cmd:%s Not found,next Cmd\n",(S8 *)(CmdBuff + CmdBuffHead));
            //LOG("Gcode Cmd:%s Not found,next Cmd\n",(S8 *)(CmdBuff + CmdBuffHead));
            memset(CmdBuff + CmdBuffHead, 0, QUEUE_MSG_SIZE);
            CmdBuffHead = NextCmdIndex(CmdBuffHead);
            if ( CmdCount > 0 )
                CmdCount --;
            GcodeHandleStatus = GCODE_EXEC_OK;
            break;
        }
//
//        if(CmdCount < 5)  //
//        {
//            break;              //当数量小于3时，出去等待接收数据
//        }
    }

//    if ( GcodeHandleStatus ==  GCODE_EXEC_OK )
//    {
//        if ( !strstr(CmdBuff + CmdBuffHead, "M105" ) )
//            rt_kprintf("%s\n", MSG_OK);
//        GcodeHandleStatus = GCODE_EXEC_PROCESS;
//    }

    return;
}

S32 CrGcode::CrGcodehandle(S8 *Cmd, S32 Len)
{
    S8 Replay[GCODE_REPLAY_BUFFF_SIZE] = {0};
    //rt_kprintf("LOGD: cmd is %s\n", Cmd);
    S8 Res = GcodeHandle(Cmd, Len, Replay);

    //if ( strlen(Replay) <= 0 )
    if ( (Res != GCODE_CMD_ERROR) && (Res != GCODE_NOT_SUPPOR) && (strlen(Replay) <= 0) && (Res != GCODE_EXEC_WAIT) )
#if 1
    {
extern U32 StuffLineOKFlag;

        if(Res == GCODE_EXEC_OK && StuffLineOKFlag == 1)
        {
            StuffLineOKFlag = 0;
        }
        else
        {
            rt_kprintf("%s\n", MSG_OK);
        }
    }
#else
//        rt_kprintf("%s\n", MSG_OK); //20211227预处理完即可认为入队成功，在函数StuffLineBuff后回复OK。
#endif
    //rt_kprintf("LOGD: cmd is %s\n", Cmd);

    return Res;
}

S32 CrGcode::PutGcodeCmd(S8 *Cmd, S32 CmdLen, S32 Priority)  //Priority用于快速命令，如暂停、停止等
{                                               //数字越大，优先级越高
    if ( !Cmd || (CmdLen <= 0) || !IsValidMsgQueue(CmdQueue) )
        return 0;

    S32 Ret = -1;

    //S64 Time = 0;
    do
    {
        Ret = SendMsg(CmdQueue, Cmd, CmdLen, Priority);

        if ( Ret != 0 )
        {
//            if ( CrGetSystemTimeMilSecs() - Time > 10 )
//            {
//                printf("echo:%s\n", STR_BUSY_PROCESSING);
//                Time = CrGetSystemTimeMilSecs();
//            }

            rt_thread_yield();
        }

    }while( Ret != 0 );

    return Ret ;
}

S32 CrGcode::NextCmdIndex(S32 Index)
{
    Index ++;
    if(Index >= QUEUE_MSG_NUM)
    {
        Index = 0;
    }
    return Index;
}

S32 CrGcode::CrCmdEnqueue(S8 *Buff,S32 Len)
{
    if ( CrCmdQueueIsFull() )
    {
        return -1;
    }
    return 0;
}

S32 CrGcode::CrCmdDequeue()
{
    if (CrCmdQueueIsEmpty())
    {
        return -1;
    }
}

BOOL CrGcode::CrCmdQueueIsEmpty()
{
    return ( (CmdBuffHead == CmdBuffTail) && (CmdCount == 0) );
}
BOOL CrGcode::CrCmdQueueIsFull()
{
    return ( CmdBuffHead == CmdBuffTail && CmdCount == QUEUE_MSG_NUM);
}

VOID CrGcode::CrCmdQueueClear()
{
    S8 Buff[QUEUE_MSG_SIZE] = {0};
    S8 Pri = 1;
    U8 Count = 0;
    while ( 1 )
    {
        if ( RecvMsg(CmdQueue, Buff, QUEUE_MSG_SIZE, (S32 *)&Pri) != QUEUE_MSG_SIZE )
        {
            if ( ++Count > 10 )
               break;
            CrM_Sleep(50);
        }
    }

    while ( GcodeHandleStatus == GCODE_EXEC_PROCESS)
        CrM_Sleep(10);

    CmdBuffHead = CmdBuffTail;
	CmdCount = 0;

	memset(CmdBuff, 0, QUEUE_MSG_NUM * QUEUE_MSG_SIZE);

    return;
}

VOID CrGcode::StopWaiting()
{
    Waiting = 0;
}

VOID CrGcode::StartWaiting()
{
    Waiting = 1;
}

#ifdef TEST_CRGCODE
VOID GcodeModeTest()
{
    S32 Fd = GcodeFileOpen(( S8 * )"/ccc.gcode");
    if ( Fd <= 0 )
    {
        printf("Open file error\n");
        return ;
    }
    CrGcode CrGcode((S8 *)"CrGcode");

    S8 Buf[QUEUE_MSG_SIZE] = {0};

    for ( ; ; )
    {
        memset(Buf, 0, QUEUE_MSG_SIZE);
        S32 Ret = GcodeFileGetCmd(Fd, Buf);
        if ( Ret == 0 )
        {
            printf("Read ERR!\n");
            break;
        }
        else if ( Ret == -1 )
        {
            printf("File End!\n");
            CrGcode.~CrGcode();
            break;
        }
        else
        {
            //   printf("SD Recv Cmd :%s, %d\n", Buf, strlen(Buf));
        }

        CrGcode.PutGcodeCmd(Buf, QUEUE_MSG_SIZE, 1);
        CrM_Sleep(10);
    }

    GcodeFileClose(Fd);
}

#endif
//MSH_CMD_EXPORT(GcodeModeTest, Test)
