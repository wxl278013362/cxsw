/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     cx2470       the first version
 */
#ifndef _GCODEM114HANDLE_H_
#define _GCODEM114HANDLE_H_

#include "CrInc/CrType.h"

/*
 *
 **/
#ifdef __cplusplus
extern "C" {
#endif


S32 GcodeM114Parse(const S8 *CmdBuff, VOID *Arg);     //M114命令的解析函数
S32 GcodeM114Exec(VOID *ArgPtr, VOID *ReplyResult);   //M114命令的执行函数
S32 GcodeM114Reply(VOID *ReplyResult, S8 *Buff);      //M114命令的回复函数

#ifdef __cplusplus
}
#endif


#endif /* _GCODEM114HANDLE_H_ */
