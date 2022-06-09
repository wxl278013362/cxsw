#ifndef _GCODEM305HANDLE_H
#define _GCODEM305HANDLE_H

#include <stdio.h>
#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM305Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM305Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM305Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
