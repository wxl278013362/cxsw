#ifndef _GCODEG60HANDLE_H
#define _GCODEG60HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG60Parse(const S8 *CmdBuff, VOID *Arg); //G60命令的执行函数
S32 GcodeG60Exec(VOID *ArgPtr, VOID *ReplyResult);   //G60命令的执行函数
S32 GcodeG60Reply(VOID *ReplyResult, S8 *Buff);   //G60命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

