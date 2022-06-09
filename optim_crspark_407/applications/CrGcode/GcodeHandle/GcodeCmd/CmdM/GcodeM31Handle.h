#ifndef _CRGCODEM31HANDLE_H
#define _CRGCODEM31HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM31Parse(const S8 *CmdBuff, VOID *Arg); //M104命令的解析函数
S32 GcodeM31Exec(VOID *ArgPtr, VOID *ReplyResult);   //M104命令的执行函数
S32 GcodeM31Reply(VOID *ReplyResult, S8 *Buff);   //M104命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
