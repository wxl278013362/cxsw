#ifndef _CRGCODEM85HANDLE_H
#define _CRGCODEM85HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM85Parse(const S8 *CmdBuff, VOID *Arg);       //M85命令的解析函数
S32 GcodeM85Exec(VOID *ArgPtr, VOID *ReplyResult);     //M85令的执行函数
S32 GcodeM85Reply(VOID *ReplyResult, S8 *Buff);            //M85令的回复函数
U32 GetMaxInactiveTime(void);

#ifdef __cplusplus
}
#endif

#endif

