
#ifndef _GCODEM120HANDLE_H_
#define _GCODEM120HANDLE_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM120Parse(const S8 *CmdBuff, VOID *Arg);     //M120命令的解析函数
S32 GcodeM120Exec(VOID *ArgPtr, VOID *ReplyResult);   //M120命令的执行函数
S32 GcodeM120Reply(VOID *ReplyResult, S8 *Buff);      //M120命令的回复函数

#ifdef __cplusplus
}
#endif


#endif /* _GCODEM120HANDLE_H_ */
