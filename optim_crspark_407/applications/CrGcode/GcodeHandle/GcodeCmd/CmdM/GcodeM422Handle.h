#ifndef _CRGCODEM422HANDLE_H
#define _CRGCODEM422HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * M422: Set the XY probe position for a given Z Stepper.
 */

#define NUM_Z_STEPPER_DRIVERS 1   // (1-4) Z options change based on how many

#define Z_STEPPER_AUTO_ALIGN

#ifdef Z_STEPPER_AUTO_ALIGN

S32 GcodeM422Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM422Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM422Reply(VOID *ReplyResult, S8 *Buff);

#endif

#ifdef __cplusplus
}
#endif

#endif


