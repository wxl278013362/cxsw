/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-29     cx2470       the first version
 */
#ifndef _GCODEM30HANDLE_H_
#define _GCODEM30HANDLE_H_


#include "CrInc/CrType.h"

/*
 * M30 删除SD卡文件
 **/
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM30Parse(const S8 *CmdBuff, VOID *Arg);     //M30命令的解析函数
S32 GcodeM30Exec(VOID *ArgPtr, VOID *ReplyResult);   //M30命令的执行函数
S32 GcodeM30Reply(VOID *ReplyResult, S8 *Buff);      //M30命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM30HANDLE_H_ */
