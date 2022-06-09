#ifndef _CRGCODEM865HANDLE_H
#define _CRGCODEM865HANDLE_H

#include "GcodeM860Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM865Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM865Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM865Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif


