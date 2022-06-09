
#ifndef _GCODEM122HANDLE_H_
#define _GCODEM122HANDLE_H_

#include "CrInc/CrType.h"

/*
 *
 **/
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM122Parse(const S8 *CmdBuff, VOID *Arg);     //M122命令的解析函数
S32 GcodeM122Exec(VOID *ArgPtr, VOID *ReplyResult);   //M122命令的执行函数
S32 GcodeM122Reply(VOID *ReplyResult, S8 *Buff);      //M122命令的回复函数

#ifdef __cplusplus
}
#endif


#endif /* _GCODEM122HANDLE_H_ */
