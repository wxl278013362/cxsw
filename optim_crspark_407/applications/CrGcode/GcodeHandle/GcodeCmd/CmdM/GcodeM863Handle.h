#ifndef _CRGCODEM863HANDLE_H
#define _CRGCODEM863HANDLE_H

#include "GcodeM860Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM863Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM863Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM863Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif




