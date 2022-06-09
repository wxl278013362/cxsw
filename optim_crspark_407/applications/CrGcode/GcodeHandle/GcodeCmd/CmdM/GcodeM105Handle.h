
#ifndef _GCODEM105HANDLE_H_
#define _GCODEM105HANDLE_H_

#include "CrInc/CrType.h"

/*
 *
 **/
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM105Parse(const S8 *CmdBuff, VOID *Arg);     //M105命令的解析函数
S32 GcodeM105Exec(VOID *ArgPtr, VOID *ReplyResult);   //M105命令的执行函数
S32 GcodeM105Reply(VOID *ReplyResult, S8 *Buff);      //M105命令的回复函数

#ifdef __cplusplus
}
#endif


#endif /* _GCODEM105HANDLE_H_ */
