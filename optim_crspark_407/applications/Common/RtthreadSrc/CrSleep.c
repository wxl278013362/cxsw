/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-25     cx2470       the first version
 */
#include "../Include/CrSleep.h"
#include "rtthread.h"
#include <rthw.h>

VOID CrM_Sleep(S32 Ms)
{
    rt_tick_t Ret = rt_tick_get();
    rt_thread_delay_until(&Ret, Ms);
}


VOID CrU_Sleep(S32 Us)
{
    rt_hw_us_delay(Us);
}
