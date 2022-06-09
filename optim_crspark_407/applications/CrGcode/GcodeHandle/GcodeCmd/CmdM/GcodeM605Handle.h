#ifndef _CRGCODEM605HANDLE_H
#define _CRGCODEM605HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

enum DualXMode
{
    DXC_FULL_CONTROL_MODE,
    DXC_AUTO_PARK_MODE,
    DXC_DUPLICATION_MODE,
    DXC_MIRRORED_MODE
};

S32 GcodeM605Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM605Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM605Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif

