/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-08     cx2470       the first version
 */
#ifndef _GCODEM209HANDLE_H_
#define _GCODEM209HANDLE_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif /*ifdef  __cplusplus */

S32 GcodeM209Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM209Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM209Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif /*ifdef  __cplusplus */

#endif /*ifndef _GCODEM209HANDLE_H_ */
