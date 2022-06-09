#ifndef _GCODEM140Handle_H
#define _GCODEM140Handle_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif
//VOID GcodeM140Init(VOID *HeaterBed);
S32 GcodeM140Parse(const S8 *CmdBuff, VOID *Arg); //M140命令的解析函数
S32 GcodeM140Exec(VOID *ArgPtr, VOID *ReplyResult);   //M140命令的执行函数
S32 GcodeM140Reply(VOID *ReplyResult, S8 *Buff);   //M140命令的回复函数
    
#ifdef __cplusplus
}
#endif

#endif

