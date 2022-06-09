#ifndef _GCODEG19HANDLE_H
#define _GCODEG19HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG19Parse(const S8 *CmdBuff, VOID *Arg);    //G19命令的解析函数
S32 GcodeG19Exec(VOID *ArgPtr, VOID *ReplyResult);  //G19命令的执行函数
S32 GcodeG19Reply(VOID *ReplyResult, S8 *Buff);     //G19命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

