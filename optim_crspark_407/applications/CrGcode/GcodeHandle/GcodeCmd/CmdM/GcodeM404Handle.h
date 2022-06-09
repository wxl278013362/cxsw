#ifndef _CRGCODEM404HANDLE_H
#define _CRGCODEM404HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM404Parse(const S8 *CmdBuff, VOID *Arg);       //M404命令的解析函数
S32 GcodeM404Exec(VOID *ArgPtr, VOID *ReplyResult);     //M404命令的执行函数
S32 GcodeM404Reply(VOID *ReplyResult, S8 *Buff);        //M404命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
