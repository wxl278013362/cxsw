#ifndef GCODECMD_CMDM_GCODEM192HANDLE_H_
#define GCODECMD_CMDM_GCODEM192HANDLE_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM192Parse(const S8 *CmdBuff, VOID *Arg);     //M192命令的解析函数
S32 GcodeM192Exec(VOID *ArgPtr, VOID *ReplyResult);   //M192命令的执行函数
S32 GcodeM192Reply(VOID *ReplyResult, S8 *Buff);      //M192命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* GCODECMD_CMDM_GCODEM192HANDLE_H_ */
