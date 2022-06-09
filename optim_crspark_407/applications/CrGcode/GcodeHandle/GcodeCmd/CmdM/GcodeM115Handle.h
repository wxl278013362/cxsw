#ifndef _GCODEM115HANDLE_H
#define _GCODEM115HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM115Parse(const S8 *CmdBuff, VOID *Arg);     //M115命令的解析函数
S32 GcodeM115Exec(VOID *ArgPtr, VOID *ReplyResult);   //M115命令的执行函数
S32 GcodeM115Reply(VOID *ReplyResult, S8 *Buff);      //M115命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
