#ifndef _GCODEM413HANDLE_H
#define _GCODEM413HANDLE_H

#include <stdio.h>
#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

extern BOOL RecoverEnable;
extern BOOL RecoverValid;
extern BOOL RecoverExist;

S32 GcodeM413Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM413Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM413Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
