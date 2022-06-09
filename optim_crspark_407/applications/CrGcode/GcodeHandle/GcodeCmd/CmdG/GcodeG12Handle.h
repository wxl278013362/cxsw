#ifndef _GCODEG12Handle_H
#define _GCODEG12Handle_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG12Parse(const S8 *CmdBuff, VOID *Arg); //G12命令的解析函数
S32 GcodeG12Exec(VOID *ArgPtr, VOID *ReplyResult);   //G12命令的执行函数
S32 GcodeG12Reply(VOID *ReplyResult, S8 *Buff);   //G12命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

