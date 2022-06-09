#ifndef _CRGCODEM166HANDLE_H
#define _CRGCODEM166HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM166Parse(const S8 *CmdBuff, VOID *Arg);   //M166命令的解析函数
S32 GcodeM166Exec(VOID *ArgPtr, VOID *ReplyResult); //M166命令的执行函数
S32 GcodeM166Reply(VOID *ReplyResult, S8 *Buff);    //M166命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
