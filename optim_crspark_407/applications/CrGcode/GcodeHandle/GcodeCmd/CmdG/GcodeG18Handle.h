#ifndef _GCODEG18HANDLE_H
#define _GCODEG18HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG18Parse(const S8 *CmdBuff, VOID *Arg);    //G18命令的解析函数
S32 GcodeG18Exec(VOID *ArgPtr, VOID *ReplyResult);  //G18命令的执行函数
S32 GcodeG18Reply(VOID *ReplyResult, S8 *Buff);     //G18命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

