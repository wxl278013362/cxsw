#ifndef _GCODEM928HANDLE_H_
#define _GCODEM928HANDLE_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM928Parse(const S8 *CmdBuff, VOID *Arg);     //M928命令的解析函数
S32 GcodeM928Exec(VOID *ArgPtr, VOID *ReplyResult);   //M928命令的执行函数
S32 GcodeM928Reply(VOID *ReplyResult, S8 *Buff);      //M928命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM928HANDLE_H_ */
