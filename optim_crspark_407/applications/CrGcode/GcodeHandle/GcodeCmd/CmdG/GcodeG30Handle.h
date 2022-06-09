#ifndef _GCODEG30HANDLE_H
#define _GCODEG30HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG30Parse(const S8 *CmdBuff, VOID *Arg);    //G30命令的解析函数
S32 GcodeG30Exec(VOID *ArgPtr, VOID *ReplyResult);  //G30命令的执行函数
S32 GcodeG30Reply(VOID *ReplyResult, S8 *Buff);     //G30命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

