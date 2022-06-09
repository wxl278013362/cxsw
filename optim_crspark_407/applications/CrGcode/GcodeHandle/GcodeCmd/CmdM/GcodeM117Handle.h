#ifndef _GCODEM117HANDLE_H_
#define _GCODEM117HANDLE_H_

#include "CrInc/CrType.h"

#define WELCOME_STR "Ender3 Ready"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM117Parse(const S8 *CmdBuff, VOID *Arg);     //M117命令的解析函数
S32 GcodeM117Exec(VOID *ArgPtr, VOID *ReplyResult);   //M117命令的执行函数
S32 GcodeM117Reply(VOID *ReplyResult, S8 *Buff);      //M117命令的回复函数

#ifdef __cplusplus
}
#endif


#endif /* _GCODEM117HANDLE_H_ */
