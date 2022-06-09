#ifndef _CRGCODEM862HANDLE_H
#define _CRGCODEM862HANDLE_H

#include "GcodeM860Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM862Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM862Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM862Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif



