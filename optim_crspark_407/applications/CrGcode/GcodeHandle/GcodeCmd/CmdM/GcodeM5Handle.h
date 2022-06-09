#ifndef _CRGCODEM5HANDLE_H
#define _CRGCODEM5HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM5Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM5Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM5Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif //_CRGCODEM5HANDLE_H
