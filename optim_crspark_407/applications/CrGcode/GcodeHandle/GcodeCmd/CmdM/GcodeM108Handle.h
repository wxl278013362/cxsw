#ifndef _CRGCODEM108HANDLE_H
#define _CRGCODEM108HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM108Parse(const S8 *CmdBuff, VOID *Arg); //M220命令的解析函数
S32 GcodeM108Exec(VOID *ArgPtr, VOID *ReplyResult);   //M220命令的执行函数
S32 GcodeM108Reply(VOID *ReplyResult, S8 *Buff);   //M220命令的回复函数
VOID GcodeM108Init(VOID *HeaterBed, VOID *HeaterNoz);

#ifdef __cplusplus
}
#endif

#endif
