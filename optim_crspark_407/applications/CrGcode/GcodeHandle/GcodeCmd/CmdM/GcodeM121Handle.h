
#ifndef _GCODEM121HANDLE_H_
#define _GCODEM121HANDLE_H_

#include "CrInc/CrType.h"


#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM121Parse(const S8 *CmdBuff, VOID *Arg);     //M121命令的解析函数
S32 GcodeM121Exec(VOID *ArgPtr, VOID *ReplyResult);   //M121命令的执行函数
S32 GcodeM121Reply(VOID *ReplyResult, S8 *Buff);      //M121命令的回复函数

#ifdef __cplusplus
}
#endif


#endif /* _GCODEM121HANDLE_H_ */
