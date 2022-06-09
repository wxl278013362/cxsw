/*
#include <TemperatureMenuItem.h>
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-16     cx2470       the first version
 */
#ifndef _TEMPERATURE_H
#define _TEMPERATURE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C"
{
#endif

VOID TemperQueueInit();
//VOID InitPreheatTemperatureMenu(float BedTemp, float NozzleTemp);

VOID* PreheatBedMenuUpAction(VOID *ArgOut, VOID *ArgIn);
VOID* PreheatBedMenuDownAction(VOID *ArgOut, VOID *ArgIn);
S32 PreheatBedMenuShow(VOID *Arg);

VOID* PreheatNozzleMenuUpAction(VOID *ArgOut, VOID *ArgIn);
VOID* PreheatNozzleMenuDownAction(VOID *ArgOut, VOID *ArgIn);
S32 PreheatNozzleMenuShow(VOID *Arg);

VOID* PreheatBedMenuOKAction(VOID *ArgOut, VOID *ArgIn);
VOID* PreheatNozzleMenuOKAction(VOID *ArgOut, VOID *ArgIn);


/**====================================================================================
 *                           系统当前目标温度菜单的特殊显示
 * ===================================================================================*/
S32 NozzleTempSpecialDisplay(VOID *Arg);
S32 BedTempSpecialDisplay(VOID *Arg);

VOID* FansSpeedMenuUPAction(VOID *ArgOut, VOID *ArgIn);
VOID* FansSpeedMenuDownAction(VOID *ArgOut, VOID *ArgIn);
VOID* FansSpeedMenuOKAction(VOID *ArgOut, VOID *ArgIn);
S32   FansSpeedMenuShow(VOID *Arg);
S32   FanSpeedSpecialDisplay(VOID *Arg);

VOID ClearTemptureShowTime(VOID);

#ifdef __cplusplus
}
#endif

#endif

