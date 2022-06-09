#ifndef _GCODEM149Handle_H
#define _GCODEM149Handle_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM149Parse(const S8 *CmdBuff, VOID *Arg); //M145命令的解析函数
S32 GcodeM149Exec(VOID *ArgPtr, VOID *ReplyResult);   //M145命令的执行函数
S32 GcodeM149Reply(VOID *ReplyResult, S8 *Buff);   //M145命令的回复函数
    
#ifdef __cplusplus
}
#endif

#endif

