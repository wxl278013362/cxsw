#ifndef _GCODEM900HANDLE_H_
#define _GCODEM900HANDLE_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM900Parse(const S8 *CmdBuff, VOID *Arg);     //M900命令的解析函数
S32 GcodeM900Exec(VOID *ArgPtr, VOID *ReplyResult);   //M900命令的执行函数
S32 GcodeM900Reply(VOID *ReplyResult, S8 *Buff);      //M900命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM900HANDLE_H_ */
