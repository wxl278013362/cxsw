#ifndef _CRGCODEM381HANDLE_H
#define _CRGCODEM381HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

#define PARKING_EXTRUDER

S32 GcodeM381Parse(const S8 *CmdBuff, VOID *Arg);       //M381命令的解析函数
S32 GcodeM381Exec(VOID *ArgPtr, VOID *ReplyResult);     //M381命令的执行函数
S32 GcodeM381Reply(VOID *ReplyResult, S8 *Buff);            //M381命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

