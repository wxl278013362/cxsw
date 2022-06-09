/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-29     cx2470       the first version
 */
#ifndef APPLICATIONS_GCODE_GCODECMD_CMDM_GCODEM190HANDLE_H_
#define APPLICATIONS_GCODE_GCODECMD_CMDM_GCODEM190HANDLE_H_

#include "CrInc/CrType.h"

/*
 **/
#ifdef __cplusplus
extern "C" {
#endif

VOID GcodeM190Init();

S32 GcodeM190Parse(const S8 *CmdBuff, VOID *Arg);     //M146命令的解析函数
S32 GcodeM190Exec(VOID *ArgPtr, VOID *ReplyResult);   //M146命令的执行函数
S32 GcodeM190Reply(VOID *ReplyResult, S8 *Buff);      //M146命令的回复函数

#ifdef __cplusplus
}
#endif



#endif /* APPLICATIONS_GCODE_GCODECMD_CMDM_GCODEM190HANDLE_H_ */
