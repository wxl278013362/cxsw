#ifndef _CRGCODEM7HANDLE_H
#define _CRGCODEM7HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

#define COOLANT_MIST_PIN        22
#define COOLANT_FLOOD_PIN       44
#define COOLANT_MIST_INVERT     false

/**
 * M7: Mist Coolant On
 */
S32 GcodeM7Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM7Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM7Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif



