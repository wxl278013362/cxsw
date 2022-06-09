#ifndef _GCODEM75HANDLE_H
#define _GCODEM75HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM75Parse(const S8 *CmdBuff, VOID *Arg);     //M75命令的解析函数
S32 GcodeM75Exec(VOID *ArgPtr, VOID *ReplyResult);   //M75命令的执行函数
S32 GcodeM75Reply(VOID *ReplyResult, S8 *Buff);      //M75命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
