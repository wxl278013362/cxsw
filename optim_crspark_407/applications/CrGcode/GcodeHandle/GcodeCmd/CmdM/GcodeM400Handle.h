#ifndef _GCODEM400HANDLE_H
#define _GCODEM400HANDLE_H

#include <stdio.h>
#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM400Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM400Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM400Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
