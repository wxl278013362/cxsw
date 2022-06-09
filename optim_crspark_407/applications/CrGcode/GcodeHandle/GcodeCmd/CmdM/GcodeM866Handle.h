#ifndef _CRGCODEM866HANDLE_H
#define _CRGCODEM866HANDLE_H

#include "GcodeM860Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM866Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM866Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM866Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif



