/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-29     cx2470       the first version
 */
#ifndef APPLICATIONS_GCODE_GCODECMD_CMDM_GCODEM109HANDLE_H_
#define APPLICATIONS_GCODE_GCODECMD_CMDM_GCODEM109HANDLE_H_

#include "CrInc/CrType.h"

/*
 **/
#ifdef __cplusplus
extern "C" {
#endif
VOID GcodeM109Init();
S32 GcodeM109Parse(const S8 *CmdBuff, VOID *Arg);     //M109命令的解析函数
S32 GcodeM109Exec(VOID *ArgPtr, VOID *ReplyResult);   //M109命令的执行函数
S32 GcodeM109Reply(VOID *ReplyResult, S8 *Buff);      //M109命令的回复函数
    
#ifdef __cplusplus
}
#endif


#endif /* APPLICATIONS_GCODE_GCODECMD_CMDM_GCODEM109HANDLE_H_ */
