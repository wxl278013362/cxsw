/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-04     cx2375       the first version
 */
#ifndef APPLICATIONS_CRHEATER_PWM_PWMCALLBACK_H_
#define APPLICATIONS_CRHEATER_PWM_PWMCALLBACK_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

VOID HeartPwmRun(VOID *PData);
VOID PwmRunBed(VOID *PData);
#ifdef __cplusplus
}
#endif

#endif /* APPLICATIONS_CRHEATER_PWM_PWMCALLBACK_H_ */
