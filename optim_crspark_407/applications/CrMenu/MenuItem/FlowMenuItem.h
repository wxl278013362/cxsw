/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-15     cx2470       the first version
 */
#ifndef _FLOWMENUITEM_H_
#define _FLOWMENUITEM_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C"
{
#endif
VOID FlowQueueInit();
VOID* FlowMenuUPAction(VOID *ArgOut, VOID *ArgIn);
VOID* FlowMenuDownAction(VOID *ArgOut, VOID *ArgIn);
VOID* FlowMenuOKAction(VOID *ArgOut, VOID *ArgIn);
S32 FlowMenuShow(VOID *Arg);
S32 FlowSpecialDisplay(VOID *Arg);
VOID ClearFlowShowTime(VOID);
#ifdef __cplusplus
}
#endif

#endif /* APPLICATIONS_CRMENU_MENUITEM_FLOWMENUITEM_H_ */
