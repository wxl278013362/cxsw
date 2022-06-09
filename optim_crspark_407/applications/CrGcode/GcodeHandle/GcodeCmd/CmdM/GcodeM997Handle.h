#ifndef _GCODEM997HANDLE_H_
#define _GCODEM997HANDLE_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM997Parse(const S8 *CmdBuff, VOID *Arg);     //M997命令的解析函数
S32 GcodeM997Exec(VOID *ArgPtr, VOID *ReplyResult);   //M997命令的执行函数
S32 GcodeM997Reply(VOID *ReplyResult, S8 *Buff);      //M997命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
