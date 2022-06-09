#ifndef _CRGCODEM428HANDLE_H
#define _CRGCODEM428HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

VOID GcodeM428Init(VOID *Print, VOID *Nozzle, VOID *Bed, VOID *Gcode);
S32 GcodeM428Parse(const S8 *CmdBuff, VOID *Arg); //M104命令的解析函数
S32 GcodeM428Exec(VOID *ArgPtr, VOID *ReplyResult);   //M104命令的执行函数
S32 GcodeM428Reply(VOID *ReplyResult, S8 *Buff);   //M104命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
