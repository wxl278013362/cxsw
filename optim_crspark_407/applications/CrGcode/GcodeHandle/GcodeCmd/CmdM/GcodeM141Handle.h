#ifndef _GCODEM141Handle_H
#define _GCODEM141Handle_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM141Parse(const S8 *CmdBuff, VOID *Arg);       //M141命令的解析函数
S32 GcodeM141Exec(VOID *ArgPtr, VOID *ReplyResult);     //M141命令的执行函数
S32 GcodeM141Reply(VOID *ReplyResult, S8 *Buff);        //M141命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
