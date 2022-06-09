#ifndef _CRGCODEM280HANDLE_H
#define _CRGCODEM280HANDLE_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WITHIN(N,L,H)       ((N) >= (L) && (N) <= (H))

#if HAS_SERVOS

/**
 * M280: Set or get the position of a servo.
 */
S32 GcodeM280Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM280Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM280Reply(VOID *ReplyResult, S8 *Buff);

#endif

#ifdef __cplusplus
}
#endif

#endif


