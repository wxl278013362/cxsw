/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-20     cx2470       the first version
 */
#ifndef _FILAMENTMENUITEM_H_
#define _FILAMENTMENUITEM_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C"
{
#endif

VOID FilamentQueueInit();

VOID *FilamentMenuOKAction(VOID *ArgOut, VOID *ArgIn);    //是否使用容积
S32   FilamentMenuDisplay(VOID *Arg);

S32   FilamentDisplay(VOID *Arg);
VOID *FilamentDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *FilamentUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *FilamentOKAction(VOID *ArgOut, VOID *ArgIn);    //长丝的直径
S32   FilamentMenuShow(VOID *Arg);
VOID ClearFilamentShowTime(VOID);

#ifdef __cplusplus
}
#endif

#endif /* _FILAMENTMENUITEM_H_ */
