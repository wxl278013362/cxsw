/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     cx2470       the first version
 */
#ifndef _GCODEM82HANDLE_H_
#define _GCODEM82HANDLE_H_

#include "CrInc/CrType.h"

/*
  * 设置挤出头当前坐标模式为绝对坐标模式(M82)，默认为绝对坐标模式
 **/
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM82Parse(const S8 *CmdBuff, VOID *Arg);     //M82命令的解析函数
S32 GcodeM82Exec(VOID *ArgPtr, VOID *ReplyResult);   //M82命令的执行函数
S32 GcodeM82Reply(VOID *ReplyResult, S8 *Buff);      //M82命令的回复函数

#ifdef __cplusplus
}
#endif



#endif /* _GCODEM82HANDLE_H_ */
