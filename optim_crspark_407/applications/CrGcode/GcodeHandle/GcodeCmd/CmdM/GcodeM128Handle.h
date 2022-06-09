#ifndef _CRGCODEM128HANDLE_H
#define _CRGCODEM128HANDLE_H

#include "GcodeM126Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

#if HAS_HEATER_1

S32 GcodeM128Parse(const S8 *CmdBuff, VOID *Arg);       //M128命令的解析函数
S32 GcodeM128Exec(VOID *ArgPtr, VOID *ReplyResult);     //M128令的执行函数
S32 GcodeM128Reply(VOID *ReplyResult, S8 *Buff);            //M128令的回复函数

extern U8 baricuda_e_to_p_pressure;

#endif

#ifdef __cplusplus
}
#endif

#endif

