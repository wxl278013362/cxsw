#ifndef _CRGCODEM281HANDLE_H
#define _CRGCODEM281HANDLE_H

#include "GcodeM280Handle.h"    //依赖宏 NUM_SERVOS、WITHIN(N,L,H)
#include "CrInc/CrConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef EDITABLE_SERVO_ANGLES
/**
 * M281: Set or get the position of a servo. Without L or U the current values will be reported.
 */
S32 GcodeM281Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM281Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM281Reply(VOID *ReplyResult, S8 *Buff);

#endif

#ifdef __cplusplus
}
#endif

#endif



