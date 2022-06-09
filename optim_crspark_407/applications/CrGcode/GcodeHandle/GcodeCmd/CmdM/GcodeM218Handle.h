#ifndef _GCODEM211HANDLE_H_
#define _GCODEM211HANDLE_H_

#include "CrInc/CrType.h"


#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM218Parse(const S8 *CmdBuff, VOID *Arg);     //M218命令的解析函数
S32 GcodeM218Exec(VOID *ArgPtr, VOID *ReplyResult);   //M218命令的执行函数
S32 GcodeM218Reply(VOID *ReplyResult, S8 *Buff);      //M218命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM218HANDLE_H_ */
