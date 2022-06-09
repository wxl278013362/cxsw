#ifndef _GCODEG31HANDLE_H
#define _GCODEG31HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG31Parse(const S8 *CmdBuff, VOID *Arg);    //G31命令的解析函数
S32 GcodeG31Exec(VOID *ArgPtr, VOID *ReplyResult);  //G31命令的执行函数
S32 GcodeG31Reply(VOID *ReplyResult, S8 *Buff);     //G31命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
