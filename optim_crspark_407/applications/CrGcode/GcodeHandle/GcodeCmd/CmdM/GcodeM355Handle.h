#ifndef _CRGCODEM355HANDLE_H
#define _CRGCODEM355HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif


#define CASE_LIGHT_ENABLE
#define CASELIGHT_USES_BRIGHTNESS

#ifdef CASE_LIGHT_ENABLE

#define CASE_LIGHT_PIN  4

/**
 * M355: Set the case light power state and/or brightness.
 */
S32 GcodeM355Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM355Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM355Reply(VOID *ReplyResult, S8 *Buff);

#endif

#ifdef __cplusplus
}
#endif

#endif



