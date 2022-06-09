#ifndef _CRGCODEM26HANDLE_H
#define _CRGCODEM26HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM26Parse(const S8 *CmdBuff, VOID *Arg); //M104命令的解析函数
S32 GcodeM26Exec(VOID *ArgPtr, VOID *ReplyResult);   //M104命令的执行函数
S32 GcodeM26Reply(VOID *ReplyResult, S8 *Buff);   //M104命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
