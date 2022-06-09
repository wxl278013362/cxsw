#ifndef _CRGCODEM250HANDLE_H
#define _CRGCODEM250HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM250Prase(const S8 *CmdBuff, VOID *Arg);   //M250命令的解析函数
S32 GcodeM250Exec(VOID *ArgPrt,VOID *ReplyResult);  //M250命令的执行函数
S32 GcodeM250Reply(VOID *ReplyReslut, S8 *Buff);    //M250命令的回复函数

#ifdef __cplusplus
}
#endif
#endif /*_CRGCODEM250HANDLE_H*/
