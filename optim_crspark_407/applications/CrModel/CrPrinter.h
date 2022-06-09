#ifndef _CRPRINTER_H
#define _CRPRINTER_H

#include "CrModel.h"
#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"
#include "CrMsgQueue/CommonQueue/CrMsgQueue.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "Common/Include/CrThread.h"
#include "Common/Include/CrMutex.h"
//#include "CrSdcard.h"   //出现交叉引用，编译不过
/*In file included from G:\RtThreadTest\Spark\applications\CrModel/CrPrinter.h:9:0,
                 from ../applications/CrModel/CrPrinter.cpp:1:
G:\RtThreadTest\Spark\applications\CrModel/CrSdcard.h:35:18: error: 'CrPrinter' has not been declared
*/
#include "CrGcode.h"
#include "CrHeaterManager.h"
#include "GcodeSource.h"

#ifndef UART_COUNT
#define UART_COUNT (1)
#endif

#define KEEP_ACTIVE_TIME  (600000)    //单位是毫秒，时间是10分钟


#define PAUSE_PRINT_RETRACT_LENGTH  (5)     //暂停时回抽的长度和恢复进料的长度一致,单位：毫米
#define PAUSE_PRINT_RISE_HIGH (1)   //暂停时Z轴的抬升高度

//从文件读取Gcode命令进行打印
class CrPrinter : public CrModel
{
public:
    CrPrinter(CrGcode *GcodeP, CrHeaterManager *NozzleP, CrHeaterManager *BedP);    //将GCode命令处理的model指针传到此函数中
    ~CrPrinter();

    VOID Exec();

    S32 StartPrint(GcodeSource *Source); //开始打印线程
    VOID StopPrint();          //停止打印线程

    VOID SetUart(GcodeSource *Uart);   //设置Uart

    S32  HasReachedTargetTemper();  //获取是否达到预设温度
    VOID SetPrintTime(S32 TimeElapsed);      //设置打逝去印时间
    VOID SetPrintStartTime(CrTimeSpec_t Time);//设置打印开始时间
    S32  SetPrintProcess(U32 ProgressV);     //设置打印进度
    S32  GetGcodeCmd(S8 *Cmd, S32 Len);   //从source中读出一条Gcode命令
    S32  PutGcodeCmd(S8 *Cmd, S32 Len);   //将命令发送给Gcode model
    enum PrintStatus_t  GetPrintStatus();  //获取打印状态（打印中，停止，暂停等）
    VOID SetPrintStatus(enum PrintStatus_t State) {Status = State;}  //获取打印状态（打印中，停止，暂停等）
    S8  *GetThreadBuff(S32 &Len);
    CrGcode *GetGcodeModel() {return Gcode;}
    GcodeSource *GetGcodeSource(){return PrintSource;}   //M27指令需要使用
    GcodeSource **GetUartArray(){return UartArray;}   //获取串口的地址

    //打印过程中的运动
    VOID PauseMotionInPrinting(S32 *NozzleTargetTemper, S32 *BedTargetTemper);  //暂停
    VOID ResumeMotionInPrinting(S32 *NozzleTargetTemper, S32 *BedTargetTemper);
    VOID StopMotionInPrinting();    //停止运动
    
    VOID PauseHandle();
    VOID ResumeHandle();
    VOID StopHandle();
    S32 GetNozTemp(){return NozzleTemp;}
    S32 GetBedTemp(){return BedTemp;}

    VOID StopWaitingHeating();
    BOOL IsManualPrintAction() {return ManualPrintAction;}
    VOID FinishPrintManualAction() { ManualPrintAction = 0;}
    S32 GetUsedTime() {return UsedTime;}

private:

    VOID GetTargetTemper(S32 *NozzleTargetTemper, S32 *BedTargetTemper);   //获取预热温度
    static VOID* ThreadHandle(VOID *Arg);       //打印线程的回调函数

    S32  PrintInfoMsgHandle(S8 *Buff, S32 Len);  //上报打印状态、打印时间和进度 (向root view上报)
    S32  PrintCmdMsgHandle(S8 *Buff, S32 Len);   //来自view的打印命令，如开始、暂停、停止

    VOID PrintInfoMsgOpt();
    VOID PrintCmdMsgOpt();

    VOID PauseOpt();
    VOID ResumeOpt();
    VOID StopOpt();

private : 
    CrPthread_t Thread;
    CrGcode *Gcode;
    CrHeaterManager *Nozzle;
    CrHeaterManager *Bed;
    GcodeSource *PrintSource;     //Gcode命令的来源
    GcodeSource *UartArray[UART_COUNT];   //串口指针
    enum PrintStatus_t Status;    //打印的状态
    S32 Progress;   // 0 - 100
    S32 UsedTime;   //已打印时间
    BOOL ManualPrintAction;   //手动操作暂停，继续和停止

    S32 NozzleTemp;
    S32 BedTemp;
    //加鎖
    CrPthreadMutex_t  Mutex;

    CrMsg_t      SendPrintInfoQueue;   //发送打印信息
    CrMsgQueue_t RecvPrintInfoReqQueue;  //
    CrMsgQueue_t PrintOptQueue;

    S8  ThreadBuff[2048 + 512];   //线程缓冲区地址
};

extern CrPrinter* Printer;

#ifdef TEST_PRINT_MODEL
VOID TestPrintModel();
#endif

#endif
