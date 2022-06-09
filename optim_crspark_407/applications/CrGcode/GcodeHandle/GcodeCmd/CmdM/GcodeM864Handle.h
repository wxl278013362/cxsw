#ifndef _CRGCODEM864HANDLE_H
#define _CRGCODEM864HANDLE_H

#include "GcodeM860Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM864Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM864Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM864Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif

