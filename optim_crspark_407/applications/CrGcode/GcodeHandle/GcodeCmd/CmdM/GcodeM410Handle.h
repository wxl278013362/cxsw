#ifndef _GCODEM410HANDLE_H
#define _GCODEM410HANDLE_H

#include <stdio.h>
#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM410Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM410Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM410Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
