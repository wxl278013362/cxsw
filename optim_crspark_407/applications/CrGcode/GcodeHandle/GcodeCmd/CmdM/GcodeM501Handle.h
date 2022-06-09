#ifndef _CRGCODEM501HANDLE_H
#define _CRGCODEM501HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif


S32 GcodeM501Parse(const S8 *CmdBuff, VOID *Arg); //M503命令的解析函数
S32 GcodeM501Exec(VOID *ArgPtr, VOID *ReplyResult);   //M503命令的执行函数
S32 GcodeM501Reply(VOID *ReplyResult, S8 *Buff);   //M503命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
