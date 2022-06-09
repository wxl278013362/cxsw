#ifndef _GCODEG28Handle_H
#define _GCODEG28Handle_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG28Parse(const S8 *CmdBuff, VOID *Arg); //G28命令的解析函数
S32 GcodeG28Exec(VOID *ArgPtr, VOID *ReplyResult);   //G28命令的执行函数
S32 GcodeG28Reply(VOID *ReplyResult, S8 *Buff);   //G28命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

