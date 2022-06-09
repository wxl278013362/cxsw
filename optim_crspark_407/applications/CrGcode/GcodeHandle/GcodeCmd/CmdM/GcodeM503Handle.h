#ifndef _CRGCODEM503HANDLE_H
#define _CRGCODEM503HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif


S32 GcodeM503Parse(const S8 *CmdBuff, VOID *Arg); //M503命令的解析函数
S32 GcodeM503Exec(VOID *ArgPtr, VOID *ReplyResult);   //M503命令的执行函数
S32 GcodeM503Reply(VOID *ReplyResult, S8 *Buff);   //M503命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
