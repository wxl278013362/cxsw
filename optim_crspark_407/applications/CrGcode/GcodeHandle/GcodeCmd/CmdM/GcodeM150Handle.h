#ifndef _GCODEM150Handle_H
#define _GCODEM150Handle_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM150Parse(const S8 *CmdBuff, VOID *Arg); //M150命令的解析函数
S32 GcodeM150Exec(VOID *ArgPtr, VOID *ReplyResult);   //M150命令的执行函数
S32 GcodeM150Reply(VOID *ReplyResult, S8 *Buff);   //M150命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
