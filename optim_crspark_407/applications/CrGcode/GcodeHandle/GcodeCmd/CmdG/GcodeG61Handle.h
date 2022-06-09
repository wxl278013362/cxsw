#ifndef _GCODEG61HANDLE_H
#define _GCODEG61HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG61Parse(const S8 *CmdBuff, VOID *Arg); //G60命令的执行函数
S32 GcodeG61Exec(VOID *ArgPtr, VOID *ReplyResult);   //G60命令的执行函数
S32 GcodeG61Reply(VOID *ReplyResult, S8 *Buff);   //G60命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

