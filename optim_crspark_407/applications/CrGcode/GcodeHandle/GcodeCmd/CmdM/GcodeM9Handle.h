#ifndef _CRGCODEM9HANDLE_H
#define _CRGCODEM9HANDLE_H

#include "GcodeM7Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * M9: Coolant OFF
 */
S32 GcodeM9Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM9Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM9Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif





