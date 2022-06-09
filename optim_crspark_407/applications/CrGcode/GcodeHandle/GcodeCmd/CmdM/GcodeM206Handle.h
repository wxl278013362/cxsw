#ifndef _GCODEM206HANDLE_H_
#define _GCODEM206HANDLE_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM206Parse(const S8 *CmdBuff, VOID *Arg);     //M205命令的解析函数
S32 GcodeM206Exec(VOID *ArgPtr, VOID *ReplyResult);   //M205命令的执行函数
S32 GcodeM206Reply(VOID *ReplyResult, S8 *Buff);      //M205命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM205HANDLE_H_ */
