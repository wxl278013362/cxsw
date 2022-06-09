#ifndef _CRGCODEM524HANDLE_H
#define _CRGCODEM524HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

//VOID GcodeM524Init(VOID *Print);
S32 GcodeM524Parse(const S8 *CmdBuff, VOID *Arg); //M524命令的解析函数
S32 GcodeM524Exec(VOID *ArgPtr, VOID *ReplyResult);   //M524命令的执行函数
S32 GcodeM524Reply(VOID *ReplyResult, S8 *Buff);   //M524命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
