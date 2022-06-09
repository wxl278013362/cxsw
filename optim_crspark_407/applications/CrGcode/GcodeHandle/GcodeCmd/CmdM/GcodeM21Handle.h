#ifndef _GCODEM21HANDLE_H
#define _GCODEM21HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM21Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM21Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM21Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
