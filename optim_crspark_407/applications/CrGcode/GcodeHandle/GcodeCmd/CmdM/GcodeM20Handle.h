#ifndef _GCODEM20HANDLE_H
#define _GCODEM20HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM20Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM20Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM20Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
