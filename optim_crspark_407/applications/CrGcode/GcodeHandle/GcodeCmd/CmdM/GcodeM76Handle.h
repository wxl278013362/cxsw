#ifndef _GCODEM76HANDLE_H
#define _GCODEM76HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM76Parse(const S8 *CmdBuff, VOID *Arg);     //M76命令的解析函数
S32 GcodeM76Exec(VOID *ArgPtr, VOID *ReplyResult);   //M76命令的执行函数
S32 GcodeM76Reply(VOID *ReplyResult, S8 *Buff);      //M76命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
