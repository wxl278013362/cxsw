#ifndef _GCODEG92Handle_H
#define _GCODEG92Handle_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG92Parse(const S8 *CmdBuff, VOID *Arg); //G92命令的解析函数
S32 GcodeG92Exec(VOID *ArgPtr, VOID *ReplyResult);   //G92命令的执行函数
S32 GcodeG92Reply(VOID *ReplyResult, S8 *Buff);   //G92命令的回复函数
    
#ifdef __cplusplus
}
#endif

#endif

