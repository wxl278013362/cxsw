#ifndef _GCODEM29HANDLE_H
#define _GCODEM29HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM29Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM29Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM29Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
