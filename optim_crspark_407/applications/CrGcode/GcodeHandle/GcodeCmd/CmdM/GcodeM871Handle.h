#ifndef _GCODEM871HANDLE_H_
#define _GCODEM871HANDLE_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM871Parse(const S8 *CmdBuff, VOID *Arg);     //M871命令的解析函数
S32 GcodeM871Exec(VOID *ArgPtr, VOID *ReplyResult);   //M871命令的执行函数
S32 GcodeM871Reply(VOID *ReplyResult, S8 *Buff);      //M871命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM871HANDLE_H_ */
