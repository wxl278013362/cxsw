#ifndef _CRGCODEM24HANDLE_H
#define _CRGCODEM24HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

VOID GcodeM24Init();
S32 GcodeM24Parse(const S8 *CmdBuff, VOID *Arg); //M104命令的解析函数
S32 GcodeM24Exec(VOID *ArgPtr, VOID *ReplyResult);   //M104命令的执行函数
S32 GcodeM24Reply(VOID *ReplyResult, S8 *Buff);   //M104命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
