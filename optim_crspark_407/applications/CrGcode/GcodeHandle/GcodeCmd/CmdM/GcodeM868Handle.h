#ifndef _GCODEM868HANDLE_H_
#define _GCODEM868HANDLE_H_

#include "GcodeM860Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM868Parse(const S8 *CmdBuff, VOID *Arg);     //M868命令的解析函数
S32 GcodeM868Exec(VOID *ArgPtr, VOID *ReplyResult);   //M868命令的执行函数
S32 GcodeM868Reply(VOID *ReplyResult, S8 *Buff);      //M868命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM861HANDLE_H_ */
