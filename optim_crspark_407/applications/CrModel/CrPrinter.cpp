#include "CrPrinter.h"
#include <string.h>
#include <rtthread.h>
#include <stdio.h>

#include "Common/Include/CrTime.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "Common/Include/CrSleep.h"
#include "CrLog/CrLog.h"
#include "CrMotion.h"
#include "CrFans.h"

#define REPORT_PRINT_PROCESS_INTERVAL (1000)

S32 StartPrintTime = 0, PrintTime = 0, PauseTime = 0;
CrPrinter* Printer = NULL;

#define STOP_WAITING_HEATING_CMD  ("M108")
#define PAUSE_PRINT_CMD ("M25")   //暂停打印
#define RESUME_PRINT_CMD ("M24")  //恢复打印
#define STOP_PRINT_CMD ("M524")   //停止打印

#define MIN_KEEP_ACTIVE_TIME (600000)    //单位是毫秒，最小保持活动的时间是10分钟

U32 KeepActiveTime = KEEP_ACTIVE_TIME;
U64 LastCmdParse = 0;


//读取gcode指令来源测试文件(TEST_FILE)的宏
#define TEST_DEMO   (1)
//测试读取的gcode文件
#define TEST_FILE   /**/"/flowerpotsleek.gcode" /*"/2body10.5.gcode"*/ /*"/5huaping1.5h.gcode"*/ /*"/1boat-3.3h.gcode"*/ /*"/4rabbit1.1.gcode"*/ /*"/dog-2.5H.gcode"*/ /*"/fk20x20x20.gcode"*/
//测试读取gcode文件的描述符
S32 FileFd = -1;




/**
 * 函数功能: 在gcode文件中，读取一条gcode指令
 * 函数参数:Cmd:存储读取的指令 MaxLen:指令的最大长度(含空格等特殊字符)
 * 输入: 无
 * 输出: 读取到的指令Cmd，以及指令的长度
 * 返回值: 小于0:读取失败,等于0:文件已读完,大于0:读取到指令的长度
 * 注：该函数会使gcode文件的读取位置发生偏移
 */
S32 ReadCmdFromFile(S8 Cmd[], U32 MaxLen)
{
    S32 Res = 0;
    if ( Cmd == NULL )
    {
        printf("Cmd buffer is NULL!\n");
        return Res;
    }
    if (FileFd < 0)
    {
        FileFd = open(TEST_FILE, O_RDONLY);
    }
    if (FileFd < 0)
    {
        printf("<error>:FileFd = %d\n",FileFd);
        return Res;
    }
    char Byte;
    U32 i = 0;
    memset(Cmd, 0, MaxLen);
    while(MaxLen --)
    {
        if ( (Res = read(FileFd, &Byte, 1)) > 0 )
        {
            if(Byte == '\n' || Byte == '\r')
            {
                if (i < 1)//命令至少2个字符
                    continue;
                else
                {
                    Cmd[i] = '\0';//命令末尾结束符
                    return i;
                }
            }
            if(Byte == ';' || Byte == '#')
            {
                /* 跳过文件的注释行 */
                while(read(FileFd, &Byte, 1) > 0)
                {
                    if (Byte == '\n')
                    {
                        if(i >= 2)
                        {
                            return i;
                        }
                        else
                            break;
                    }
                }
                MaxLen ++;//防止跳过MaxLen行后,不继续读取有效指令
            }
            else
            {
                Cmd[i] = Byte;
                i++;
            }
        }
        else {
            close(FileFd);
            FileFd = -1;
            if (Res == 0)
            {
                rt_kprintf("read File:%s finish!\n",TEST_FILE);
                return 0;
            }
            else
            {
                rt_kprintf("read File error! err = %d \n",Res);
                return -1;
            }
        }
    }
    return i;
}
/**
 * 函数功能: 在gcode文件中，读取一条gcode指令(主要用于测试)
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 * 注：该函数会使gcode文件的读取位置发生偏移
 */
void ReadCmdTest()
{
    S8 CmdBuf[50]= {0};
    S32 Ret = ReadCmdFromFile(CmdBuf, 50);
    rt_kprintf("read result:%d Cmd:%s\n", Ret, CmdBuf);
}
/**
 * 函数功能: 在gcode文件中，读取十条gcode指令(主要用于测试)
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 * 注：该函数会使gcode文件的读取位置发生偏移
 */

void ReadCmdManyTest()
{
int i = 10;
    while(i--)
    {
        S8 CmdBuf[50]= {0};
        S32 Ret = ReadCmdFromFile(CmdBuf, 50);
        rt_kprintf("read result:%d Cmd:%s\n", Ret, CmdBuf);
    }
}
MSH_CMD_EXPORT(ReadCmdManyTest,ReadCmdManyTest);
MSH_CMD_EXPORT(ReadCmdTest,ReadCmdTest);

/**
 * 函数功能: 负责循环获取gcode指令，并发送至gcode指令处理线程
 * 输入: Arg:CrPrinter的实例指针
 * 输出: 无
 * 返回值: 无
 */

VOID* CrPrinter::ThreadHandle(VOID *Arg)
{
    if ( !Arg )
        return NULL;

    CrPrinter *Printer = (CrPrinter *)Arg;
//    CrGcode *GcodeModel = Printer->GetGcodeModel();
//    if ( !GcodeModel )
//        return NULL;

    S8 KeepActive = false;  //关闭标识

    GcodeSource **UartArr =  Printer->GetUartArray();
    S8 UartCmdArr[50] = {0};

    U8  NeedClearCmdQueue = 0;
    while (1)
    {
        if ( Printer->GetPrintStatus() == PRINTING )   //开始打印
        {
            if ( StartPrintTime == 0 )
                StartPrintTime = rt_tick_get();   //开始打印的时间

            S8 CmdBuff[QUEUE_MSG_SIZE] = {0};
            NeedClearCmdQueue = 1;
            S32 Res = -1;
            
#if TEST_DEMO/*测试USB demo，通过USB读取U盘固定gcode文件里的命令*/
            Res = ReadCmdFromFile(CmdBuff, QUEUE_MSG_SIZE);
#else
            Res = Printer->GetGcodeCmd(CmdBuff, QUEUE_MSG_SIZE);
#endif

#if 0   //   暂时不用2021-11-16
            if ( Res <= -1)   //读失败
            {
                printf("Read Error \n");
                //LOG(">:Read Cmd Error \n");
                Printer->SetPrintStatus(STOP_PRINT);
                continue;
            }
            else if ( Res == 0 )  //读完了
            {
                Printer->StopPrint();
                Printer->SetPrintStatus(STOP_PRINT);
                PrintTime = 0;   //将停止时的时间清理
                StartPrintTime = 0;
                continue;
            }
            /* if gcode queue is full,free up CPU and waitting enqueue.*/
            if ( strcmp(CmdBuff, STOP_WAITING_HEATING_CMD) == 0 )
            {
                Printer->GetGcodeModel()->StopWaiting();
                continue;
            }

            Printer->PutGcodeCmd(CmdBuff, QUEUE_MSG_SIZE);
            S32 Time = (rt_tick_get() - StartPrintTime + PrintTime) / 1000;
            Printer->SetPrintTime(Time);
            //rt_kprintf("Read Gcode Cmd: %s\r\n", CmdBuff);
            //LOG(">:Get Gcode Cmd: %s\r\n", CmdBuff);
#else
            if ( Res <= -1)   //读失败
             {
                 printf("Read Error \n");
                 //LOG(">:Read Cmd Error \n");
                 Printer->SetPrintStatus(STOP_PRINT);
             }
             else if ( Res == 0 )  //读完了
             {
                 Printer->StopPrint();
                 Printer->SetPrintStatus(STOP_PRINT);
                 PrintTime = 0;   //将停止时的时间清理
                 StartPrintTime = 0;
             }
             else
             {
                 /* if gcode queue is full,free up CPU and waitting enqueue.*/
                 if ( strcmp(CmdBuff, STOP_WAITING_HEATING_CMD) == 0 )
                 {
                     Printer->GetGcodeModel()->StopWaiting();
                     //continue;
                 }
                 else
                 {
                     Printer->PutGcodeCmd(CmdBuff, QUEUE_MSG_SIZE);
                     S32 Time = (rt_tick_get() - StartPrintTime + PrintTime) / 1000;
                     Printer->SetPrintTime(Time);

                     KeepActive = true;
                     LastCmdParse = CrGetSystemTimeMilSecs();
                 }
             }
#endif
        }
        else if (Printer->GetPrintStatus() == PAUSE_PRINT)
        {
//            if ( !(Printer->IsManualPrintAction()) )   //是否时手动停止   暂时不用2021-11-16
//                continue ;

            if ( Printer->IsManualPrintAction() )
            {
                S8 Cmd[QUEUE_MSG_SIZE] = {0};
                strcpy(Cmd, PAUSE_PRINT_CMD);
                Cmd[strlen(PAUSE_PRINT_CMD)] = '\0';
                Printer->PutGcodeCmd(Cmd, sizeof(Cmd));
                Printer->FinishPrintManualAction();
                LastCmdParse = CrGetSystemTimeMilSecs();
            }
        }
        else if ( Printer->GetPrintStatus() == RESUME_PRINT )
        {
//            if ( !(Printer->IsManualPrintAction()) )   //是否时手动停止   暂时不用2021-11-16
//                continue ;
            if ( Printer->IsManualPrintAction() )
            {
                S8 Cmd[QUEUE_MSG_SIZE] = {0};
                strcpy(Cmd, RESUME_PRINT_CMD);
                Cmd[strlen(RESUME_PRINT_CMD)] = '\0';
                Printer->PutGcodeCmd((S8 *)Cmd, QUEUE_MSG_SIZE);
                Printer->FinishPrintManualAction();
                LastCmdParse = CrGetSystemTimeMilSecs();
                KeepActive = true;
            }
        }
        else if ( Printer->GetPrintStatus() == STOP_PRINT )
        {
            if ( NeedClearCmdQueue )   //是否需要清队列
            {
                Printer->GetGcodeModel()->CrCmdQueueClear();
                NeedClearCmdQueue = 0;
            }
            StartPrintTime = 0;
            PrintTime = 0;
//            if ( !(Printer->IsManualPrintAction()) )   //是否时手动停止   暂时不用2021-11-16
//                continue ;

            if ( Printer->IsManualPrintAction() )
            {
                //执行运动
                S8 Cmd[50] = {0};
                strcpy(Cmd, STOP_PRINT_CMD);
                Cmd[strlen(STOP_PRINT_CMD)] = '\0';
                Printer->PutGcodeCmd(Cmd,QUEUE_MSG_SIZE);
                Printer->FinishPrintManualAction();
                LastCmdParse = CrGetSystemTimeMilSecs();
                KeepActive = true;
            }
        }

        if ( CrGetSystemTimeMilSecs() > (LastCmdParse + KeepActiveTime) )
        {
            if ( KeepActive )
            {
                //关闭中断

                //关闭加热器和风扇
                S8 i = 0;
                for ( i = 0; i < PWM_FAN_COUNT ; ++i )
                {
                    if ( PwmFan[i] )
                    {
                        struct PwmControllerAttr_t Attr = PwmFan[i]->GetFanAttr();
                        Attr.Duty = 0;
                        PwmFan[i]->SetFanAttr(Attr);
                    }
                }

                if ( BedHeater )
                {
                    struct HeaterManagerAttr_t BedAttr = BedHeater->CrHeaterMagagerGetAttr();
                    BedAttr.TargetTemper = 0;
                    BedHeater->CrHeaterMagagerSetAttr(BedAttr);
                }

                for ( i = 0; i < HOTENDS ; ++i )
                {
                    if ( HotEndArr[i] )
                    {
                        struct HeaterManagerAttr_t HotAttr = HotEndArr[i]->CrHeaterMagagerGetAttr();
                        HotAttr.TargetTemper = 0;
                        HotEndArr[i]->CrHeaterMagagerSetAttr(HotAttr);
                    }
                }

                //释放电机
                if ( MotionModel )
                    MotionModel->MotionStop();

                KeepActive = false;
            }

        }
        //循环读取串口的内容进行处理
        if ( !UartArr[0] )
            continue;

        memset(UartCmdArr, 0, sizeof(UartCmdArr));
        if ( UartArr[0]->GetCmd(UartCmdArr, sizeof(UartCmdArr)) )
        {
            //rt_kprintf("recv uart cmd %s\n", UartCmdArr);
            if ( GcodeUart::VerifyCmd(UartCmdArr) )
            {
                S8 UartCmd[50] = {0};
                if ( GcodeUart::GetVerifiedGcodeCmd(UartCmdArr, UartCmd) )
                {
                    //rt_kprintf("LOGD:*****************recv uart cmd %s, source cmd %s\n", UartCmd, UartCmdArr);
                    Printer->PutGcodeCmd(UartCmd, strlen(UartCmd) + 1);   //注意要将字符串结束符也传进去
                    LastCmdParse = CrGetSystemTimeMilSecs();
                    KeepActive = true;
                }
            }
        }

        rt_thread_yield();
    }

    return NULL;
}


CrPrinter::CrPrinter(CrGcode *GcodeP, CrHeaterManager *NozzleP, CrHeaterManager *BedP)
    : CrModel(),
      PrintSource(NULL),
      Status(STOP_PRINT),
      ManualPrintAction(0)

{
    Gcode = GcodeP;
    Nozzle = NozzleP;
    Bed = BedP;
    UsedTime = 0;
    Progress = 0;
    RecvPrintInfoReqQueue = InValidMsgQueue();
    PrintOptQueue = InValidMsgQueue();
    SendPrintInfoQueue = NULL;

    CrMsgQueueAttr_t Attr;
    Attr.mq_flags = O_NONBLOCK;
    Attr.mq_maxmsg = 1;
    Attr.mq_msgsize = sizeof(struct PrintInfoMsg_t);
    SendPrintInfoQueue = SharedQueueOpen(PRINT_INFO_REPORT_QUEUE, O_CREAT | O_RDWR, 0x0777, &Attr);
    Attr.mq_msgsize = sizeof(struct ViewPrintInfoMsg_t);
    RecvPrintInfoReqQueue = QueueOpen(PRINT_INFO_REQ_QUEUE, O_CREAT | O_RDWR, 0x0777, &Attr);

    Attr.mq_msgsize = sizeof(struct ViewPrintCmdMsg_t);
    PrintOptQueue = QueueOpen(PRINT_PRINTOPT_QUEUE, O_CREAT | O_RDWR, 0x0777, &Attr);

    S8 i = 0;
    for ( i = 0; i < UART_COUNT; ++i )
        UartArray[i] = NULL;

    CrPthreadAttr_t AttrThread;
    memset(&AttrThread, 0, sizeof(AttrThread));
    CrPthreadAttrInit(&AttrThread);
    memset(ThreadBuff, THREAD_BYTE_INIT_VALUE, sizeof(ThreadBuff));
    AttrThread.stackaddr = ThreadBuff;
    AttrThread.stacksize = sizeof(ThreadBuff);
    /* priority as same as main.*/
//    AttrThread.schedparam.sched_priority = RT_MAIN_THREAD_PRIORITY;
    /* pth02.*/
    CrPthreadAttrSetDetachState(&AttrThread, PTHREAD_CREATE_DETACHED);
    S32 Res = CrPthreadCreate(&Thread, &AttrThread, CrPrinter::ThreadHandle, this);

    CrPthreadAttrDestroy(&AttrThread);

    if ( Res != 0 )
    {
        rt_kprintf("CrPrinter model thread create fail, err = %d\n", errno);
    }

    /*Init mutex*/
    CrPthreadMutexInit(&Mutex, NULL);

}

CrPrinter::~CrPrinter()
{
    StopPrint();
    CrPthreadMutexDestroy(&Mutex);
}

VOID CrPrinter::StopPrint()
{
    CrPthreadMutexLock(&Mutex);

    if(PrintSource != NULL)
    {
        delete PrintSource;
        PrintSource = NULL;
    }
    CrPthreadMutexUnLock(&Mutex);

    struct HeaterManagerAttr_t BedAttr = Bed->CrHeaterMagagerGetAttr();
    BedAttr.TargetTemper = 0.0;
    Bed->CrHeaterMagagerSetAttr(BedAttr);
    struct HeaterManagerAttr_t NozzleAttr = Nozzle->CrHeaterMagagerGetAttr();
    NozzleAttr.TargetTemper = 0.0f;
    Nozzle->CrHeaterMagagerSetAttr(NozzleAttr);
    Progress = 0;
    UsedTime = 0;
}

enum PrintStatus_t CrPrinter::GetPrintStatus()
{
    return Status;
}

S32 CrPrinter::StartPrint(GcodeSource *Source)
{
    CrPthreadMutexLock(&Mutex);

    if ( !Source || PrintSource )
    {
        CrPthreadMutexUnLock(&Mutex);
        return 0;
    }
    rt_kprintf("Start...\n");
    PrintSource = Source;
    CrPthreadMutexUnLock(&Mutex);
    Status = PRINTING;
    Progress = 0;
    UsedTime = 0;
    return 1;
}

VOID CrPrinter::Exec()
{
    PrintInfoMsgOpt();
    PrintCmdMsgOpt();
    return ;
}

VOID CrPrinter::PrintInfoMsgOpt()
{
    if ( !IsValidMsgQueue(PrintOptQueue) )
        return ;

    S8 Buf[sizeof(struct ViewPrintCmdMsg_t)] = {0};
    S32 Prime = 1;

    if ( RecvMsg(PrintOptQueue, Buf, sizeof(Buf), &Prime) <= 0)
        return ;

    PrintCmdMsgHandle(Buf, sizeof(Buf));

    struct PrintInfoMsg_t Info;
    Info.Status = Status;
    Info.Progress = Progress;
    Info.TimeElapsed = UsedTime;
	if ( SendPrintInfoQueue )
        CrMsgSend(SendPrintInfoQueue, (S8 *)&Info, sizeof(Info), 1);

    return ;
}

VOID CrPrinter::PrintCmdMsgOpt()
{
    if ( !IsValidMsgQueue(RecvPrintInfoReqQueue) || !SendPrintInfoQueue )
        return ;

    S8 Buf[sizeof(struct ViewPrintInfoMsg_t)] = {0};
    S32 Prime = 0;
    if ( RecvMsg(RecvPrintInfoReqQueue, Buf, sizeof(Buf), &Prime) <= 0 )
        return ;

    PrintInfoMsgHandle(Buf, sizeof(Buf));

    struct PrintInfoMsg_t Info;
    Info.Status = Status;
    Info.Progress = Progress;
    Info.TimeElapsed = UsedTime;
    CrMsgSend(SendPrintInfoQueue, (S8 *)&Info, sizeof(Info), 1);

    return ;
}

S32 CrPrinter::HasReachedTargetTemper()
{
    if ( !Nozzle || !Bed ) 
        return 0;

    if ( Nozzle->ReachedTargetTemper() && Bed->ReachedTargetTemper() )
        return 1;

    return 0;
}

VOID CrPrinter::SetPrintTime(S32 TimeElapsed)
{
    UsedTime = TimeElapsed;

    return ;
}

S32 CrPrinter::SetPrintProcess(U32 ProgressV)
{
    Progress = ProgressV;

    return 1;
}

S32 CrPrinter::GetGcodeCmd(S8 *Cmd, S32 Len)
{
    CrPthreadMutexLock(&Mutex);

    if ( !Cmd || !PrintSource )
    {
        CrPthreadMutexUnLock(&Mutex);
        return -1;
    }

    S32 Res = PrintSource->GetCmd(Cmd, Len);  //返回-1表示读失败(参数引起)， 0表示GcodeCmd已经读取完毕，大于0表示读取成功）
    CrPthreadMutexUnLock(&Mutex);
    if( Res <= -1)   //读错了
    {
        return Res;
    }
    else if ( Res == 0 )  //读完了
    {
        StopPrint();
        SetPrintProcess(100);
        return 0;
    }

    CrPthreadMutexLock(&Mutex);
    if ( PrintSource )
        SetPrintProcess(PrintSource->GetReadPercent() * 100);
    CrPthreadMutexUnLock(&Mutex);

    return Res;
}

S32 CrPrinter::PutGcodeCmd(S8 *Cmd, S32 Len)
{
    if ( !Cmd || (Len <= 0) || !Gcode )
        return 0;

    Gcode->PutGcodeCmd(Cmd, Len, 1);

    return 1;
}

S8  *CrPrinter::GetThreadBuff(S32 &Len)
{
    Len = sizeof(ThreadBuff);
    return ThreadBuff;
}

S32 CrPrinter::PrintInfoMsgHandle(S8 *Buff, S32 Len)
{
    if ( !Buff || (Len < (S32)sizeof(struct ViewPrintInfoMsg_t)) )
        return 0;

    struct ViewPrintInfoMsg_t *Info = (struct ViewPrintInfoMsg_t *)Buff;
    if ( Info->ReqProcess == 0 )
        return 0;

    return 1;
}


S32 CrPrinter::PrintCmdMsgHandle(S8 *Buff, S32 Len)
{
    if ( !Buff || (Len < (S32)sizeof(struct ViewPrintCmdMsg_t)) )
        return 0;

    struct ViewPrintCmdMsg_t *Cmd = (struct ViewPrintCmdMsg_t *)Buff;

    switch ( Cmd->Action )
    {
        case CMD_PAUSE:
        {
            PrintTime += rt_tick_get() - StartPrintTime;
            PauseOpt();
            ManualPrintAction = 1;
            GetGcodeModel()->StopWaiting();
            break;
        }
        case CMD_STOP:
        {
            StopOpt();
            StartPrintTime = 0;  //将要停止打印的模型的时间清零
            PrintTime = 0;
            ManualPrintAction = 1;
            GetGcodeModel()->StopWaiting();
            break;
        }
        case CMD_RESUME:
        {   
            StartPrintTime = rt_tick_get();
            ResumeOpt();
            ManualPrintAction = 1;
            GetGcodeModel()->StopWaiting();
            break;
        }
        case CMD_START:
        {
            Status = PRINTING;
            StartPrintTime = rt_tick_get();
            PrintTime = 0;
            GetGcodeModel()->StopWaiting();
            break;
        }
    }

    return 1;
}

VOID CrPrinter::GetTargetTemper(S32 *NozzleTargetTemper, S32 *BedTargetTemper)
{
    if ( !NozzleTargetTemper || !BedTargetTemper || !Nozzle || !Bed)
        return ;

    struct HeaterManagerAttr_t BedAttr = Bed->CrHeaterMagagerGetAttr();
    struct HeaterManagerAttr_t NozAttr = Nozzle->CrHeaterMagagerGetAttr();
    *NozzleTargetTemper = NozAttr.TargetTemper;
    if ( (NozAttr.TargetTemper - *NozzleTargetTemper) >= 0.5f )
        *NozzleTargetTemper += 1;

    *BedTargetTemper = BedAttr.TargetTemper;
    if ( (BedAttr.TargetTemper - *BedTargetTemper) >= 0.5f )
        *BedTargetTemper += 1;

    return ;
}

VOID CrPrinter::PauseHandle()
{
    printf("%s\n", __FUNCTION__);
    GetTargetTemper(&NozzleTemp, &BedTemp);

    if ( Nozzle )
    {
        struct HeaterManagerAttr_t Attr = Nozzle->CrHeaterMagagerGetAttr();
        Attr.TargetTemper = 0;
        Nozzle->CrHeaterMagagerSetAttr(Attr);
    }

    if ( Bed )
    {
        struct HeaterManagerAttr_t Attr = Bed->CrHeaterMagagerGetAttr();
        Attr.TargetTemper = 0;
        Bed->CrHeaterMagagerSetAttr(Attr);
    }
}

VOID CrPrinter::ResumeHandle()
{
    Status = PRINTING;

//    SetPrintStatus(PRINTING);
}

VOID CrPrinter::StopHandle()
{
//    Status = STOP_PRINT;

    StopPrint();
}

VOID CrPrinter::StopWaitingHeating()
{
    if ( Gcode )
        Gcode->StopWaiting();
}

VOID CrPrinter::PauseOpt()
{
    Status = PAUSE_PRINT;
}

VOID CrPrinter::ResumeOpt()
{
    Status = RESUME_PRINT;
}

VOID CrPrinter::StopOpt()
{
    Status = STOP_PRINT;
}

VOID CrPrinter::SetUart(GcodeSource *Uart)
{
    if ( !Uart )
        return ;

    S8 i = 0;
    for ( i = 0; i < UART_COUNT; ++i )
        if ( !UartArray[i] )
            break;

    if ( i < UART_COUNT )
        UartArray[i] = Uart;
}
