#ifndef _GCODEM23HANDLE_H
#define _GCODEM23HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM23Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM23Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM23Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
