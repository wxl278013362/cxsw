#ifndef _CRGCODEM867HANDLE_H
#define _CRGCODEM867HANDLE_H

#include "GcodeM860Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM867Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM867Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM867Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif




