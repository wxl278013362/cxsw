#ifndef _GCODEM603HANDLE_H
#define _GCODEM603HANDLE_H

#include <stdio.h>
#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM603Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM603Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM603Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
