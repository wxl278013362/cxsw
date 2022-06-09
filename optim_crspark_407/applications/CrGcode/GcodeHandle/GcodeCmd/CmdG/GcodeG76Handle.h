#ifndef _GCODEG76HANDLE_H
#define _GCODEG76HANDLE_H

#include "CrInc/CrType.h"

/*
 * M17使能电机
 **/
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG76Parse(const S8 *CmdBuff, VOID *Arg);     //M17命令的解析函数
S32 GcodeG76Exec(VOID *ArgPtr, VOID *ReplyResult);   //M17命令的执行函数
S32 GcodeG76Reply(VOID *ReplyResult, S8 *Buff);      //M17命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
