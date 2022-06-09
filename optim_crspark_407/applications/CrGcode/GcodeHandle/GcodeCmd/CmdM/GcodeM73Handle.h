#ifndef _GCODEM73HANDLE_H
#define _GCODEM73HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

VOID GcodeM73Init(VOID *Model);

S32 GcodeM73Parse(const S8 *CmdBuff, VOID *Arg);     //M710命令的解析函数
S32 GcodeM73Exec(VOID *ArgPtr, VOID *ReplyResult);   //M710命令的执行函数
S32 GcodeM73Reply(VOID *ReplyResult, S8 *Buff);      //M710命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
