/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     AboutPrinter.cpp
  Author :        Chenli
  Version:        1.0
  Description:    打印机信息
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          Chenli        2020-10-15     V1.0.0.1
****************************************************************************/
#ifndef __ABOUTPRINTER_MENUITEM_H__
#define __ABOUTPRINTER_MENUITEM_H__

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

 VOID* AboutPrinterUpAction(VOID *ArgOut, VOID *ArgIn);
 VOID* AboutPrinterDownAction(VOID *ArgOut, VOID *ArgIn);
 VOID* AboutPrinterOkAction(VOID *ArgOut, VOID *ArgIn);
 S32 AboutPrinterMenuShow(VOID *Arg);
 S32 CenterDisplayX(S8 *DisplayPtr);

#ifdef __cplusplus
}
#endif

#endif
