#include "GcodeM27Handle.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrPrinter.h"
#include "CrModel/CrGcode.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "Common/Include/CrSleep.h"
#include "Common/Include/CrTimer.h"
#include <stdlib.h>
#include <stdio.h>

#define CMD_M27 ("M27")

S32 GcodeM27Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S8 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};

    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;
    
    if ( strcmp(Cmd, CMD_M27) != 0 )  //传进来的命令不是G0命令
        return 0;

    S8 Offset = strlen(CMD_M27) + sizeof(S8); //加1是越过分隔符
    struct ArgM27_t *CmdArg = (struct ArgM27_t *)Arg;

    S8 Opt[20] = {0};
    S8 OptLen = 0;

     while ( Offset < Len )
    {
        memset(Opt, 0, sizeof(Opt));
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
            break;    //获取参数失败

        OptLen = strlen(Opt);

        switch ( Opt[0] )
        {
            case OPT_C:
                CmdArg->HasC = true;
            break;

            case OPT_S:

                CmdArg->HasS = true;

                if (OptLen > 1)
                    CmdArg->S = atoi(Opt + 1);
            break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

VOID PrintFileName()
{
//    CrMsgQueue_t PrintFileSend = ViewSendQueueOpen((S8 *)SD_PRINTFILE_REQ_QUEUE, sizeof(struct ViewReqPrintFileMsg_t));
//    CrMsg_t PrintFileRecv = ViewRecvQueueOpen((S8 *)SD_PRINTFILE_REPORT_QUEUE, sizeof(struct SdCardPrintFileMsg_t));
//
//    struct ViewReqPrintFileMsg_t File = {1};
//
//    QueueSendMsg(PrintFileSend, ( S8 * )&File, sizeof(struct ViewReqPrintFileMsg_t), 1);
//
//    CrM_Sleep(60);
//
//    rt_kprintf("Current file: ");
//
//    S8 Buff[sizeof(struct SdCardPrintFileMsg_t)] = {0};
//    U32 MsgPri = 0;
//
//    if ( CrMsgRecv(PrintFileRecv, Buff, sizeof(Buff), &MsgPri) > 0 )
//    {
//        struct SdCardPrintFileMsg_t *PrintFileBuff = (struct SdCardPrintFileMsg_t *)Buff;
//        if ( strlen (PrintFileBuff->Name) > 0 )
//        {
//            rt_kprintf("%s\n", PrintFileBuff->Name);
//        }
//        else
//            rt_kprintf("(no file)\n");
//    }
//    else
//    {
//        rt_kprintf("(no file)\n");
//    }
}

VOID TimerCallback(VOID *Args)
{
    S8 * FileName = NULL;
    if ( Printer )
        FileName = Printer->GetGcodeSource()->GetFileName();

    printf("Current file: %s\n", FileName ? FileName : "(no file)");
}

S32 GcodeM27Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM27_t *Param = (struct ArgM27_t *)ArgPtr;

    if ( Param->HasC )
        TimerCallback(NULL);            //打印当前已经打开的文件名

    if ( Param->HasS )
    {
        static CrTimer_t Timer = NULL;                  //定时器指针

        if ( Param->S > 0 )
        {
            CrSignalEvent_t SigEvent = {0};

            SigEvent.sigev_notify = 0;
            SigEvent.sigev_notify_attributes = NULL;
            SigEvent.sigev_signo = 0;
            SigEvent.sigev_notify_function = (VOID (*) (union SigVal_t))TimerCallback;
            SigEvent.sigev_value = {0};

            CrClockid_t ClockId = 0;

            struct CrTimerAttr_t CrTimerInfo = {0};
            strncpy(CrTimerInfo.Name, "Filereport", sizeof(CrTimerInfo.Name));
            CrTimerInfo.HardwareTimer = 0;
            CrTimerInfo.Mode = RT_TIMER_FLAG_PERIODIC;

            if ( !Timer )
            {
                Timer = &CrTimerInfo;
                CrTimerCreate(ClockId, &SigEvent, &Timer);
            }

            if ( Timer )
            {
                 CrTimerSpec_t OldTimerSpac = {0};
                 CrTimerSpec_t NewTimerSpac = {0};
                 NewTimerSpac.it_interval.tv_sec = Param->S;
                 CrTimerSetTime(Timer, 1, &NewTimerSpac, &OldTimerSpac);
            }
        }
        else
        {
            CrTimerDelete(Timer);
        }
    }

    return 1;
}

S32 GcodeM27Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
