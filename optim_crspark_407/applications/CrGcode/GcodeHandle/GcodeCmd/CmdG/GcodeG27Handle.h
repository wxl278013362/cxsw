#ifndef _GCODEG27Handle_H
#define _GCODEG27Handle_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG27Parse(const S8 *CmdBuff, VOID *Arg); //G1命令的解析函数
S32 GcodeG27Exec(VOID *ArgPtr, VOID *ReplyResult);   //G1命令的执行函数
S32 GcodeG27Reply(VOID *ReplyResult, S8 *Buff);   //G1命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

