#ifndef _GCODEM861HANDLE_H_
#define _GCODEM861HANDLE_H_

#include "GcodeM860Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM861Parse(const S8 *CmdBuff, VOID *Arg);     //M861命令的解析函数
S32 GcodeM861Exec(VOID *ArgPtr, VOID *ReplyResult);   //M861命令的执行函数
S32 GcodeM861Reply(VOID *ReplyResult, S8 *Buff);      //M861命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM861HANDLE_H_ */
