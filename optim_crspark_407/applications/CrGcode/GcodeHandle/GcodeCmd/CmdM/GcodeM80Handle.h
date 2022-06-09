#ifndef _GCODEM80HANDLE_H
#define _GCODEM80HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PS_ON_PIN "NULL"    //启用时需要重新定义电源引脚

S32 GcodeM80Parse(const S8 *CmdBuff, VOID *Arg);     //M80命令的解析函数
S32 GcodeM80Exec(VOID *ArgPtr, VOID *ReplyResult);   //M80命令的执行函数
S32 GcodeM80Reply(VOID *ReplyResult, S8 *Buff);      //M80命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
