#ifndef _CRGCODEM504HANDLE_H
#define _CRGCODEM504HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM504Parse(const S8 *CmdBuff, VOID *Arg);   //M504命令的解析函数
S32 GcodeM504Exec(VOID *ArgPtr, VOID *ReplyResult); //M504命令的执行函数
S32 GcodeM504Reply(VOID *ReplyResult, S8 *Buff);    //M504命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
