#ifndef _GCODEG90Handle_H
#define _GCODEG90Handle_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG90Parse(const S8 *CmdBuff, VOID *Arg); //G90命令的解析函数
S32 GcodeG90Exec(VOID *ArgPtr, VOID *ReplyResult);   //G90命令的执行函数
S32 GcodeG90Reply(VOID *ReplyResult, S8 *Buff);   //G90命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

