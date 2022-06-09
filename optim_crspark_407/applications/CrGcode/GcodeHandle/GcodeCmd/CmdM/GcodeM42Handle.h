#ifndef _GCODEM42HANDLE_H
#define _GCODEM42HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM42Parse(const S8 *CmdBuff, VOID *Arg);     //M42命令的解析函数
S32 GcodeM42Exec(VOID *ArgPtr, VOID *ReplyResult);   //M42命令的执行函数
S32 GcodeM42Reply(VOID *ReplyResult, S8 *Buff);      //M42命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
