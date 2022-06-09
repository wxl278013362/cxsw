/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-25     cx2470       the first version
 */
#ifndef _CRSLEEP_H_
#define _CRSLEEP_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C"
{
#endif

VOID CrM_Sleep(S32 Ms);

VOID CrU_Sleep(S32 Us);

#ifdef __cplusplus
}
#endif
#endif /* APPLICATIONS_CRSLEEP_CRSLEEP_H_ */
