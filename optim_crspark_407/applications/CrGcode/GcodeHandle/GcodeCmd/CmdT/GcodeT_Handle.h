#ifndef _GCODET_HANDLE_H
#define _GCODET_HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeCmdT_Parse(const S8 *CmdBuff, VOID *Arg);  //T命令解析
S32 GcodeCmdT_Exec(VOID *ArgPtr, VOID *ReplyResult);   //T命令执行
S32 GcodeCmdT_Reply(VOID *ReplyResult, S8 *Buff); //T命令回复
    
#ifdef __cplusplus
}
#endif

#endif

