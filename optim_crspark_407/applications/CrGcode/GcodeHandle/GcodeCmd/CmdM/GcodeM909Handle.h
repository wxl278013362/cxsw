#ifndef _GCODEM909HANDLE_H_
#define _GCODEM909HANDLE_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM909Parse(const S8 *CmdBuff, VOID *Arg);     //M909命令的解析函数
S32 GcodeM909Exec(VOID *ArgPtr, VOID *ReplyResult);   //M909命令的执行函数
S32 GcodeM909Reply(VOID *ReplyResult, S8 *Buff);      //M909命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM909HANDLE_H_ */
