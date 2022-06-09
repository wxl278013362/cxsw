#ifndef _CRGCODEG38HANDLE_H
#define _CRGCODEG38HANDLE_H

#include "CrGcode/GcodeHandle/GcodeCmd/CmdM/GcodeM126Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

#define G38_PROBE_TARGET
#define G38_PROBE_AWAY
#define MULTIPLE_PROBING 2

#ifdef G38_PROBE_TARGET
S32 GcodeG38Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeG38Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeG38Reply(VOID *ReplyResult, S8 *Buff);
#endif

#ifdef __cplusplus
}
#endif

#endif




