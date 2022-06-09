#ifndef _GCODEM911HANDLE_H_
#define _GCODEM911HANDLE_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM911Parse(const S8 *CmdBuff, VOID *Arg);     //M911命令的解析函数
S32 GcodeM911Exec(VOID *ArgPtr, VOID *ReplyResult);   //M911命令的执行函数
S32 GcodeM911Reply(VOID *ReplyResult, S8 *Buff);      //M911命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM911HANDLE_H_ */
