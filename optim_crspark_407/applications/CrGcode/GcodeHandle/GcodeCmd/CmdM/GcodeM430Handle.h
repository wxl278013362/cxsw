#ifndef _CRGCODEM430HANDLE_H
#define _CRGCODEM430HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAS_POWER_MONITOR
#define HAS_WIRED_LCD
#define POWER_MONITOR_CURRENT
#define POWER_MONITOR_VOLTAGE
#define HAS_POWER_MONITOR_WATTS

#ifdef HAS_POWER_MONITOR

S32 GcodeM430Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM430Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM430Reply(VOID *ReplyResult, S8 *Buff);

#endif

#ifdef __cplusplus
}
#endif

#endif


