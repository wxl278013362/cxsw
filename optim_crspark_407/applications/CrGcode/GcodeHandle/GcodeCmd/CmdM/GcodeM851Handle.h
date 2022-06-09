#ifndef _CRGCODEM851HANDLE_H
#define _CRGCODEM851HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM851Parse(const S8 *CmdBuff, VOID *Arg);       //M851命令的解析函数
S32 GcodeM851Exec(VOID *ArgPtr, VOID *ReplyResult);     //M851命令的执行函数
S32 GcodeM851Reply(VOID *ReplyResult, S8 *Buff);        //M851命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

