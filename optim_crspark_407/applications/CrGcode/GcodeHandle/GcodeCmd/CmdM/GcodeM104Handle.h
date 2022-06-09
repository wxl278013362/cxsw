#ifndef _GCODEM104Handle_H
#define _GCODEM104Handle_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM104Parse(const S8 *CmdBuff, VOID *Arg); //M104命令的解析函数
S32 GcodeM104Exec(VOID *ArgPtr, VOID *ReplyResult);   //M104命令的执行函数
S32 GcodeM104Reply(VOID *ReplyResult, S8 *Buff);   //M104命令的回复函数

#ifdef __cplusplus
}
#endif

#endif

