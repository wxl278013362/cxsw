#ifndef _GCODEM910HANDLE_H_
#define _GCODEM910HANDLE_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM910Parse(const S8 *CmdBuff, VOID *Arg);     //M910命令的解析函数
S32 GcodeM910Exec(VOID *ArgPtr, VOID *ReplyResult);   //M910命令的执行函数
S32 GcodeM910Reply(VOID *ReplyResult, S8 *Buff);      //M910命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM910HANDLE_H_ */
