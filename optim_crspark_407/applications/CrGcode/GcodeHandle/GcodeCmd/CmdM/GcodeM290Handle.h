#ifndef _CRGCODEM290HANDLE_H
#define _CRGCODEM290HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM290Parse(const S8 *CmdBuff, VOID *Arg); //M220命令的解析函数
S32 GcodeM290Exec(VOID *ArgPtr, VOID *ReplyResult);   //M220命令的执行函数
S32 GcodeM290Reply(VOID *ReplyResult, S8 *Buff);   //M220命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
