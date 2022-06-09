#ifndef _CRGCODECMDHANDLEMODEL_H
#define _CRGCODECMDHANDLEMODEL_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrMsgQueue/CommonQueue/CrMsgQueue.h"
#include "Common/Include/CrThread.h"
#include "CrModel.h"
//#include "CrMotion.h"

#define QUEUE_MSG_NUM            16
#define QUEUE_MSG_SIZE           50


//只进行Gcode命令的接受处理，不负责Gcode命令的获取
class CrGcode : public CrModel
{
public:
    CrGcode(S8 *QueueName);
    ~CrGcode();

//    S32  StartRecvCmd();
//    S32  StopRecvCmd();
    S32  PutGcodeCmd(S8 *Cmd, S32 CmdLen, S32 Priority = 1);  //Priority用于快速命令，如暂停、停止等
                                                       //数字越大，优先级越高
    VOID Exec();//将gcode命令收取，存储至内部队列
    VOID CrCmdQueueClear();
    VOID StopWaiting();   //停止等待
    VOID StartWaiting();  //开始等待
    BOOL IsWaiting() { return Waiting;}     //是否在等待

    S8 *GetThreadBuffAddr(S32 &BuffLen);

protected:
    static VOID *CmdThread(VOID* Arg);

    S32  CrGcodehandle(S8 *Cmd, S32 Len);     //从内部队列取出gcode命令，解析，执行，回复

private:
    CrMsgQueue_t CmdQueue;
    CrPthread_t GcodeThread;
    S8  ThreadBuff[2048]; //+1024
    S8  Waiting;    //等待状态
    /*********************** extened by TingFengXuan ***********************/
    S32 GcodeHandleStatus; /**/
    S32 CmdBuffHead;
    S32 CmdBuffTail;
    U8 CmdCount;
    S8  CmdBuff[QUEUE_MSG_NUM][QUEUE_MSG_SIZE];
    S32 CrCmdEnqueue(S8 *Buff,S32 Len);
    S32 CrCmdDequeue();
    BOOL CrCmdQueueIsEmpty();
    BOOL CrCmdQueueIsFull();
    S32 NextCmdIndex(S32 Index);
};

extern CrGcode* GcodeModel;

#endif
