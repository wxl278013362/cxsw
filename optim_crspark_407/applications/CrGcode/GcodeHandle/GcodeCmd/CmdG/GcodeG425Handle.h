#ifndef _GCODEG425Handle_H
#define _GCODEG425Handle_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG425Parse(const S8 *CmdBuff, VOID *Arg); //G425命令的解析函数
S32 GcodeG425Exec(VOID *ArgPtr, VOID *ReplyResult);   //G425命令的执行函数
S32 GcodeG425Reply(VOID *ReplyResult, S8 *Buff);   //G425命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

