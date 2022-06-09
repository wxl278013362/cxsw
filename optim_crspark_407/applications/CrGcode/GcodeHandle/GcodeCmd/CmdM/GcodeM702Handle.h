#ifndef _CRGCODEM702HANDLE_H
#define _CRGCODEM702HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM702Parse(const S8 *CmdBuff, VOID *Arg); //M104命令的解析函数
S32 GcodeM702Exec(VOID *ArgPtr, VOID *ReplyResult);   //M104命令的执行函数
S32 GcodeM702Reply(VOID *ReplyResult, S8 *Buff);   //M104命令的回复函数
//VOID GcodeM702Init(VOID *Gcode, VOID *Nozzle);

#ifdef __cplusplus
}
#endif

#endif
