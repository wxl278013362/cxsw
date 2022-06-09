#ifndef _GCODEG10HANDLE_H
#define _GCODEG10HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG10Parse(const S8 *CmdBuff, VOID *Arg); //G0命令的执行函数
S32 GcodeG10Exec(VOID *ArgPtr, VOID *ReplyResult);   //G0命令的执行函数
S32 GcodeG10Reply(VOID *ReplyResult, S8 *Buff);   //G0命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

