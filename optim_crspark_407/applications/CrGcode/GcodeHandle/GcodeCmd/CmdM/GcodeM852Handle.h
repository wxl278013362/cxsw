#ifndef _CRGCODEM852HANDLE_H
#define _CRGCODEM852HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM852Parse(const S8 *CmdBuff, VOID *Arg);       //M852命令的解析函数
S32 GcodeM852Exec(VOID *ArgPtr, VOID *ReplyResult);     //M852命令的执行函数
S32 GcodeM852Reply(VOID *ReplyResult, S8 *Buff);        //M852命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

