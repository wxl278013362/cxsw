#ifndef _GCODEG32HANDLE_H
#define _GCODEG32HANDLE_H

#include "../CmdM/GcodeM401Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeG32Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeG32Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeG32Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif

