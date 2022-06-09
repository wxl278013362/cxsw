#ifndef _CRGCODEM48HANDLE_H
#define _CRGCODEM48HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM48Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM48Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM48Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif //_CRGCODEM48HANDLE_H
