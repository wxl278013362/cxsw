#ifndef _GCODEM304HANDLE_H
#define _GCODEM304HANDLE_H

#include <stdio.h>
#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM304Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM304Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM304Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
