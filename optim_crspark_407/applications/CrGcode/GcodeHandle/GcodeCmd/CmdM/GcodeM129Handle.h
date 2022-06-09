#ifndef _CRGCODEM129HANDLE_H
#define _CRGCODEM129HANDLE_H

#include "GcodeM128Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

#if HAS_HEATER_1

S32 GcodeM129Parse(const S8 *CmdBuff, VOID *Arg);       //M128命令的解析函数
S32 GcodeM129Exec(VOID *ArgPtr, VOID *ReplyResult);     //M128令的执行函数
S32 GcodeM129Reply(VOID *ReplyResult, S8 *Buff);            //M128令的回复函数

#endif

#ifdef __cplusplus
}
#endif

#endif


