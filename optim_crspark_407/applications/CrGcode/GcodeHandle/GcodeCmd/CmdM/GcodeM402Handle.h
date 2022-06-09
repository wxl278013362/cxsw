#ifndef _CRGCODEM402HANDLE_H
#define _CRGCODEM402HANDLE_H

#include "GcodeM401Handle.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * M402: Stow the bed probe. The Z axis may raise up to make room for the probe to stow.
 */
#if HAS_BED_PROBE

S32 GcodeM402Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM402Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM402Reply(VOID *ReplyResult, S8 *Buff);

#endif

#ifdef __cplusplus
}
#endif

#endif




