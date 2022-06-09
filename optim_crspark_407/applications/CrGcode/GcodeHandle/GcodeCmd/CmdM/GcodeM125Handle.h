#ifndef _GCODEM125HANDLE_H_
#define _GCODEM125HANDLE_H_

#include "CrInc/CrType.h"

/*
 *
 **/
#ifdef __cplusplus
extern "C" {
#endif

extern BOOL NozParkPaused;

S32 GcodeM125Parse(const S8 *CmdBuff, VOID *Arg);     //M125命令的解析函数
S32 GcodeM125Exec(VOID *ArgPtr, VOID *ReplyResult);   //M125命令的执行函数
S32 GcodeM125Reply(VOID *ReplyResult, S8 *Buff);      //M125命令的回复函数

#ifdef __cplusplus
}
#endif


#endif /* _GCODEM125HANDLE_H_ */
