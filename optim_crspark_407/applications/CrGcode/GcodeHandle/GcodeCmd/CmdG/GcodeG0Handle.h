#ifndef _GCODEG0HANDLE_H
#define _GCODEG0HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG0Parse(const S8 *CmdBuff, VOID *Arg); //G0命令的执行函数
S32 GcodeG0Exec(VOID *ArgPtr, VOID *ReplyResult);   //G0命令的执行函数
S32 GcodeG0Reply(VOID *ReplyResult, S8 *Buff);   //G0命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

