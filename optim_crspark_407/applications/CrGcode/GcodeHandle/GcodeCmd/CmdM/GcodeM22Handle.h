#ifndef _GCODEM22HANDLE_H
#define _GCODEM22HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM22Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM22Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM22Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
