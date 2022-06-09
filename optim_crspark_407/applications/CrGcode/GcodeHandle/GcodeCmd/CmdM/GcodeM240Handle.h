#ifndef _GCODEM240HANDLE_H
#define _GCODEM240HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM240Parse(const S8 *CmdBuff, VOID *Arg);   //M240命令的解析函数
S32 GcodeM240Exec(VOID *ArgPtr, VOID *ReplyResult); //M240命令的执行函数
S32 GcodeM240Reply(VOID *ReplyResult, S8 *Buff);    //M240命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
