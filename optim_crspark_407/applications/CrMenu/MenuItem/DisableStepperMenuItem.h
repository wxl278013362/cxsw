/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-16     cx2470       the first version
 */
#ifndef APPLICATIONS_CRMENU_MENUITEM_DISABLESTEPPERMENUITEM_H_
#define APPLICATIONS_CRMENU_MENUITEM_DISABLESTEPPERMENUITEM_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C"
{
#endif

S32 DisableStepperMenuInit();
VOID *DisableStepperMenuItemOKAction(VOID *ArgOut, VOID *ArgIn);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATIONS_CRMENU_MENUITEM_DISABLESTEPPERMENUITEM_H_ */
