#ifndef _GCODEM200HANDLE_H_
#define _GCODEM200HANDLE_H_

#include "CrInc/CrType.h"


#ifdef __cplusplus
extern "C" {
#endif

VOID GcodeM200Init(VOID *MotionModel);

S32 GcodeM200Parse(const S8 *CmdBuff, VOID *Arg);     //M200命令的解析函数
S32 GcodeM200Exec(VOID *ArgPtr, VOID *ReplyResult);   //M200命令的执行函数
S32 GcodeM200Reply(VOID *ReplyResult, S8 *Buff);      //M200命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM200HANDLE_H_ */
