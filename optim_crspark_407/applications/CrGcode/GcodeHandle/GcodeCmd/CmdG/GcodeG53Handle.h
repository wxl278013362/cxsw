#ifndef _GCODEG53HANDLE_H
#define _GCODEG53HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG53Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeG53Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeG53Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
