#ifndef _GCODEM261HANDLE_H_
#define _GCODEM261HANDLE_H_

#include "CrInc/CrType.h"

#define EXPERIMENTAL_I2CBUS 1

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM261Parse(const S8 *CmdBuff, VOID *Arg);     //M261命令的解析函数
S32 GcodeM261Exec(VOID *ArgPtr, VOID *ReplyResult);   //M261命令的执行函数
S32 GcodeM261Reply(VOID *ReplyResult, S8 *Buff);      //M261命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM261HANDLE_H_ */
