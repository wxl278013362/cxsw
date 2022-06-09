/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-15     cx2470       the first version
 */
#ifndef _PRINTFSPEED_H_
#define _PRINTFSPEED_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C"
{
#endif
VOID PrintfSpeedQueueInit();
VOID* PrintfSpeedMenuUPAction(VOID *ArgOut, VOID *ArgIn);
VOID* PrintfSpeedMenuDownAction(VOID *ArgOut, VOID *ArgIn);
VOID* PrintfSpeedMenuOKAction(VOID *ArgOut, VOID *ArgIn);
S32   PrintfSpeedMenuShow(VOID *Arg);
S32 PrintfSpeedSpecialDisplay(VOID *Arg);
VOID ClearPrintfSpeedShowTime(VOID);

#ifdef __cplusplus
}
#endif
#endif /* APPLICATIONS_CRMENU_MENUITEM_PRINTFSPEED_H_ */
