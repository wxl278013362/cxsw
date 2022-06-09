#ifndef _CRGCODEM672HANDLE_H
#define _CRGCODEM672HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM672Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM672Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM672Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif


