#ifndef _CRGCODEM8HANDLE_H
#define _CRGCODEM8HANDLE_H

#include "GcodeM7Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * M8: Flood Coolant On
 */
S32 GcodeM8Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM8Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM8Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif




