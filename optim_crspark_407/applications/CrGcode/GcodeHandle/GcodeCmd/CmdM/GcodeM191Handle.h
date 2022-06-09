#ifndef _CRGCODEM191HANDLE_H
#define _CRGCODEM191HANDLE_H

#include "CrInc/CrType.h"
#include "CrModel/CrHeaterManager.h"

#ifdef __cplusplus
extern "C" {
#endif

VOID GcodeM191Init();                                 //初始化消息发送队列

S32 GcodeM191Parse(const S8 *CmdBuff, VOID *Arg);     //M191命令的解析函数
S32 GcodeM191Exec(VOID *ArgPtr, VOID *ReplyResult);   //M191命令的执行函数
S32 GcodeM191Reply(VOID *ReplyResult, S8 *Buff);      //M191命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _CRGCODEM191HANDLE_H */
