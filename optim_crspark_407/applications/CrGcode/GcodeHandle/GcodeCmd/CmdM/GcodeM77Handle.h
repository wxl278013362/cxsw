#ifndef _GCODEM77HANDLE_H
#define _GCODEM77HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM77Parse(const S8 *CmdBuff, VOID *Arg);     //M77命令的解析函数
S32 GcodeM77Exec(VOID *ArgPtr, VOID *ReplyResult);   //M77命令的执行函数
S32 GcodeM77Reply(VOID *ReplyResult, S8 *Buff);      //M77命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
