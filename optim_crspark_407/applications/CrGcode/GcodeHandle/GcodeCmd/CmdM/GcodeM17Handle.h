#ifndef _GCODEM17HANDLE_H
#define _GCODEM17HANDLE_H

#include "CrInc/CrType.h"

/*
 * M17使能电机
 **/
#ifdef __cplusplus
extern "C" {
#endif

VOID GcodeM17Init(VOID *Motion);

S32 GcodeM17Parse(const S8 *CmdBuff, VOID *Arg);     //M17命令的解析函数
S32 GcodeM17Exec(VOID *ArgPtr, VOID *ReplyResult);   //M17命令的执行函数
S32 GcodeM17Reply(VOID *ReplyResult, S8 *Buff);      //M17命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
