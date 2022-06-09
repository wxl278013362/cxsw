#ifndef _CRGCODEM127HANDLE_H
#define _CRGCODEM127HANDLE_H

#include "CrInc/CrType.h"
#include "GcodeM126Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

#if HAS_HEATER_0

S32 GcodeM127Parse(const S8 *CmdBuff, VOID *Arg);       //M127命令的解析函数
S32 GcodeM127Exec(VOID *ArgPtr, VOID *ReplyResult);     //M127令的执行函数
S32 GcodeM127Reply(VOID *ReplyResult, S8 *Buff);            //M127令的回复函数

#endif

#ifdef __cplusplus
}
#endif

#endif


