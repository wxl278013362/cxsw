#ifndef _PRINTMENUITEM_H
#define _PRINTMENUITEM_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

//打印标志
#define PRINT_READY (0x00)      //预备
#define PRINT_START (0x01)      //开始
#define PRINT_PAUSE (0x02)      //暂停
#define PRINT_CONTINUE (0x04)   //继续
#define PRINT_STOP (0x08)       //停止

//extern S32  PrintFlag;
S32 PrintOptMenuInit();

VOID* StartPrintAction(VOID *ArgOut, VOID *ArgIn);   //开始打印

VOID* StopPrintAction(VOID *ArgOut, VOID *ArgIn);   //停止打印
VOID* PausePrintAction(VOID *ArgOut, VOID *ArgIn);   //暂停打印
VOID* ContinuePrintAction(VOID *ArgOut, VOID *ArgIn);   //继续打印

//S32   GetPrintStatus();  //获取打印状态


#ifdef __cplusplus
}
#endif
#endif
