#ifndef _CRGCODEM701HANDLE_H
#define _CRGCODEM701HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM701Parse(const S8 *CmdBuff, VOID *Arg); //M104命令的解析函数
S32 GcodeM701Exec(VOID *ArgPtr, VOID *ReplyResult);   //M104命令的执行函数
S32 GcodeM701Reply(VOID *ReplyResult, S8 *Buff);   //M104命令的回复函数
VOID GcodeM701Init(VOID *Gcode, VOID *Nozzle);

#ifdef __cplusplus
}
#endif

#endif
