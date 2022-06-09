#ifndef _GCODEM301HANDLE_H
#define _GCODEM301HANDLE_H

#include <stdio.h>
#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM301Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM301Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM301Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
