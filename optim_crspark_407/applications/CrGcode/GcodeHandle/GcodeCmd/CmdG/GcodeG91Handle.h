#ifndef _GCODEG91Handle_H
#define _GCODEG91Handle_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG91Parse(const S8 *CmdBuff, VOID *Arg); //G91命令的解析函数
S32 GcodeG91Exec(VOID *ArgPtr, VOID *ReplyResult);   //G91命令的执行函数
S32 GcodeG91Reply(VOID *ReplyResult, S8 *Buff);   //G91命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

