/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     cx2470       the first version
 */

#ifndef _GCODEM18OrM84Handle_H
#define _GCODEM18OrM84Handle_H

#include "CrInc/CrType.h"

/*
 * M18, M84都是禁用电机的命令，区别在于M84多了设置超时时间的功能
 **/
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM18OrM84Parse(const S8 *CmdBuff, VOID *Arg);     //M18OrM84命令的解析函数
S32 GcodeM18OrM84Exec(VOID *ArgPtr, VOID *ReplyResult);   //M18OrM84命令的执行函数
S32 GcodeM18OrM84Reply(VOID *ReplyResult, S8 *Buff);      //M18OrM84命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /*_GCODEM18OrM84Handle_H*/

