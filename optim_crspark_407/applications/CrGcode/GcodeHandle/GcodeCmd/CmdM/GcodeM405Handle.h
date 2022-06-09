#ifndef _CRGCODEM405HANDLE_H
#define _CRGCODEM405HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

extern BOOL    SensorFlag;

S32 GcodeM405Parse(const S8 *CmdBuff, VOID *Arg);       //M405命令的解析函数
S32 GcodeM405Exec(VOID *ArgPtr, VOID *ReplyResult);     //M405命令的执行函数
S32 GcodeM405Reply(VOID *ReplyResult, S8 *Buff);        //M405命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
