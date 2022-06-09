/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-29     cx2470       the first version
 */
#ifndef _GCODEM109HANDLE_H_
#define _GCODEM109HANDLE_H_

#include "CrInc/CrType.h"

/*
 **/
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM110Parse(const S8 *CmdBuff, VOID *Arg);     //M110命令的解析函数
S32 GcodeM110Exec(VOID *ArgPtr, VOID *ReplyResult);   //M110命令的执行函数
S32 GcodeM110Reply(VOID *ReplyResult, S8 *Buff);      //M110命令的回复函数
    
#ifdef __cplusplus
}
#endif


#endif /* _GCODEM109HANDLE_H_ */
