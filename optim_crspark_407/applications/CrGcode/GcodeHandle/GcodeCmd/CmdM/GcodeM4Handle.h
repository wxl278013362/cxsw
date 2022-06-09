#ifndef _CRGCODEM4HANDLE_H
#define _CRGCODEM4HANDLE_H

#include "CrInc/CrType.h"
#include "GcodeM3Handle.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM4Parse(const S8 *CmdBuff, VOID *Arg);       //M4命令的解析函数
S32 GcodeM4Exec(VOID *ArgPtr, VOID *ReplyResult);     //M4命令的执行函数
S32 GcodeM4Reply(VOID *ReplyResult, S8 *Buff);        //M4命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
