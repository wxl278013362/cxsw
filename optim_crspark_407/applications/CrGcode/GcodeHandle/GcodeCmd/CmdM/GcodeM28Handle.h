#ifndef _GCODEM28HANDLE_H
#define _GCODEM28HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM28Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM28Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM28Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
