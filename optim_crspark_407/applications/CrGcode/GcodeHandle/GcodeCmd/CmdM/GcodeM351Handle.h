#ifndef _GCODEM351HANDLE_H
#define _GCODEM351HANDLE_H

#include <stdio.h>
#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM351Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM351Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM351Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
