#ifndef _CRGCODEM43HANDLE_H
#define _CRGCODEM43HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM43Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM43Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM43Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
