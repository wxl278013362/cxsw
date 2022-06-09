#ifndef _CRGCODEM16HANDLE_H
#define _CRGCODEM16HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM16Parse(const S8 *CmdBuff, VOID *Arg); //M16命令的解析函数
S32 GcodeM16Exec(VOID *ArgPtr, VOID *ReplyResult);   //M16命令的执行函数
S32 GcodeM16Reply(VOID *ReplyResult, S8 *Buff);   //M16命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
