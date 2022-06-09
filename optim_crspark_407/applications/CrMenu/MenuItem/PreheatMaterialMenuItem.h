/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-16     cx2470       the first version
 */
#ifndef _PREHEATMATERIALMENUITEM_H_
#define _PREHEATMATERIALMENUITEM_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C"
{
#endif

VOID TargetTemperQueueInit();

VOID *PreheatPLAMenuItemOKAction(VOID *ArgOut, VOID *ArgIn);
VOID *PreheatPLABedMenuItemOKAction(VOID *ArgOut, VOID *ArgIn);
VOID *PreheatPLANozMenuItemOKAction(VOID *ArgOut, VOID *ArgIn);
VOID *PreheatABSMenuItemOKAction(VOID *ArgOut, VOID *ArgIn);
VOID *PreheatABSBedMenuItemOKAction(VOID *ArgOut, VOID *ArgIn);
VOID *PreheatABSNozMenuItemOKAction(VOID *ArgOut, VOID *ArgIn);

/**====================================================================================
 *                           PLA材料预设温度菜单的特殊显示
 * ===================================================================================*/
S32  PLANozzleTempSpecialDisplay(VOID *Arg);
S32  PLABedTempSpecialDisplay(VOID *Arg);

VOID *PLAPreheatBedMenuDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *PLAPreheatBedMenuUPAction(VOID *ArgOut, VOID *ArgIn);
S32   PLAPreheatBedMenuShow(VOID *Arg);
VOID *PLAPreheatBedMenuOkAction(VOID *ArgOut, VOID *ArgIn);

VOID *PLAPreheatNozzleMenuUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *PLAPreheatNozzleMenuDownAction(VOID *ArgOut, VOID *ArgIn);
S32   PLAPreheatNozzleMenuShow(VOID *Arg);
VOID *PLAPreheatNozzleMenuOkAction(VOID *ArgOut, VOID *ArgIn);

/**====================================================================================
 *                           ABS材料预设温度菜单的特殊显示
 * ===================================================================================*/
S32   ABSNozzleTempSpecialDisplay(VOID *Arg);
S32   ABSBedTempSpecialDisplay(VOID *Arg);
VOID *ABSPreheatBedMenuDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *ABSPreheatBedMenuUPAction(VOID *ArgOut, VOID *ArgIn);
S32   ABSPreheatBedMenuShow(VOID *Arg);
VOID *ABSPreheatBedMenuOkAction(VOID *ArgOut, VOID *ArgIn);

VOID *ABSPreheatNozzleMenuOkAction(VOID *ArgOut, VOID *ArgIn);
VOID *ABSPreheatNozzleMenuUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *ABSPreheatNozzleMenuDownAction(VOID *ArgOut, VOID *ArgIn);
S32   ABSPreheatNozzleMenuShow(VOID *Arg);

/*
  *              材料风扇速度操作
 * */
S32   PLAFanSpeedSpecialDisplay(VOID *Arg);
S32   ABSFanSpeedSpecialDisplay(VOID *Arg);

VOID* PLAFansSpeedMenuUPAction(VOID *ArgOut, VOID *ArgIn);
VOID* PLAFansSpeedMenuDownAction(VOID *ArgOut, VOID *ArgIn);
VOID* PLAFansSpeedMenuOKAction(VOID *ArgOut, VOID *ArgIn);
S32   PLAFansSpeedMenuShow(VOID *Arg);

VOID* ABSFansSpeedMenuUPAction(VOID *ArgOut, VOID *ArgIn);
VOID* ABSFansSpeedMenuDownAction(VOID *ArgOut, VOID *ArgIn);
VOID* ABSFansSpeedMenuOKAction(VOID *ArgOut, VOID *ArgIn);
S32   ABSFansSpeedMenuShow(VOID *Arg);
VOID ClearPreheatShowTime(VOID);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATIONS_CRMENU_MENUITEM_PREHEATPLAMENUITEM_H_ */
