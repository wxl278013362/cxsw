#ifndef _GCODEM211HANDLE_H_
#define _GCODEM211HANDLE_H_

#include "CrInc/CrType.h"


#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM211Parse(const S8 *CmdBuff, VOID *Arg);     //M211命令的解析函数
S32 GcodeM211Exec(VOID *ArgPtr, VOID *ReplyResult);   //M211命令的执行函数
S32 GcodeM211Reply(VOID *ReplyResult, S8 *Buff);      //M211命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM211HANDLE_H_ */
