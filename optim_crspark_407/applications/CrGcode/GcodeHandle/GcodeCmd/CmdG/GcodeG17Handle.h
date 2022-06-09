#ifndef _GCODEG17HANDLE_H
#define _GCODEG17HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG17Parse(const S8 *CmdBuff, VOID *Arg);    //G17命令的解析函数
S32 GcodeG17Exec(VOID *ArgPtr, VOID *ReplyResult);  //G17命令的执行函数
S32 GcodeG17Reply(VOID *ReplyResult, S8 *Buff);     //G17命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

