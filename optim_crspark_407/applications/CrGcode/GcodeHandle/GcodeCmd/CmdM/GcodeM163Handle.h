#ifndef _CRGCODEM163HANDLE_H
#define _CRGCODEM163HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

#define MIXING_EXTRUDER

#ifdef MIXING_EXTRUDER

#define MIXING_STEPPERS 2                               // 一个混合挤出机含有的电机数

S32 GcodeM163Parse(const S8 *CmdBuff, VOID *Arg);       // M163命令的解析函数
S32 GcodeM163Exec(VOID *ArgPtr, VOID *ReplyResult);     // M163命令的执行函数
S32 GcodeM163Reply(VOID *ReplyResult, S8 *Buff);        // M163命令的回复函数

extern float *Collector;

#endif

#ifdef __cplusplus
}
#endif

#endif

