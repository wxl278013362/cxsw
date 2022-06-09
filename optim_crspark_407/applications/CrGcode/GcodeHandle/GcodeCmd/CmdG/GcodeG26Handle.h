#ifndef _GCODEG26HANDLE_H
#define _GCODEG26HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG26Parse(const S8 *CmdBuff, VOID *Arg);    //G26命令的解析函数
S32 GcodeG26Exec(VOID *ArgPtr, VOID *ReplyResult);  //G26命令的执行函数
S32 GcodeG26Reply(VOID *ReplyResult, S8 *Buff);     //G26命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

