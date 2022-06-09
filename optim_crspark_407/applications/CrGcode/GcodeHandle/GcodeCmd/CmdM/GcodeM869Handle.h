#ifndef _CRGCODEM869HANDLE_H
#define _CRGCODEM869HANDLE_H

#include "GcodeM860Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM869Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM869Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM869Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif





