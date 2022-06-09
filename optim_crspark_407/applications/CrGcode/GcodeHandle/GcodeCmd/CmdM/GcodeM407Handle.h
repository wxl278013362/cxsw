#ifndef _CRGCODEM407HANDLE_H
#define _CRGCODEM407HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM407Parse(const S8 *CmdBuff, VOID *Arg);       //M407命令的解析函数
S32 GcodeM407Exec(VOID *ArgPtr, VOID *ReplyResult);     //M407命令的执行函数
S32 GcodeM407Reply(VOID *ReplyResult, S8 *Buff);        //M407命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
