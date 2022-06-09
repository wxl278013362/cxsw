#ifndef _GCODEM81HANDLE_H
#define _GCODEM81HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM81Parse(const S8 *CmdBuff, VOID *Arg);     //M81命令的解析函数
S32 GcodeM81Exec(VOID *ArgPtr, VOID *ReplyResult);   //M81命令的执行函数
S32 GcodeM81Reply(VOID *ReplyResult, S8 *Buff);      //M81命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
