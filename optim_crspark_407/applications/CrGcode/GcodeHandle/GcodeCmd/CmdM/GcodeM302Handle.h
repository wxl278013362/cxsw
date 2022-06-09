#ifndef _GCODEM302HANDLE_H
#define _GCODEM302HANDLE_H

#include <stdio.h>
#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM302Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM302Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM302Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
