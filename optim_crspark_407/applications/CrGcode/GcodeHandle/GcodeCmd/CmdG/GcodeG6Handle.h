#ifndef _GCODEG6HANDLE_H
#define _GCODEG6HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C"  {
#endif

S32 GcodeG6Parse(const S8 *CmdBuff,VOID *Arg);      //G6命令的解析函数
S32 GcodeG6Exec(VOID *ArgPtr, VOID *ReplyResult);   //G6命令的执行函数
S32 GcodeG6Reply(VOID *ReplyResult, S8 *Buff);      //G6命令的回复函数
void ResetStepperTimeOut();
U32 GetPreviousMoveMs();

#ifdef __cplusplus
}
#endif

#endif
