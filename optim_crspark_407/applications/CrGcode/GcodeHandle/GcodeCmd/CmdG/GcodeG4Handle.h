#ifndef _GCODEG4Handle_H
#define _GCODEG4Handle_H

#include "CrInc/CrType.h"

#define STR_Z_MOVE_COMP                     "Z_move_comp"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG4Parse(const S8 *CmdBuff, VOID *Arg); //G4命令的解析函数
S32 GcodeG4Exec(VOID *ArgPtr, VOID *ReplyResult);   //G4命令的执行函数
S32 GcodeG4Reply(VOID *ReplyResult, S8 *Buff);   //G4命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

