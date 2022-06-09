#ifndef _GCODEM600HANDLE_H
#define _GCODEM600HANDLE_H

#include <stdio.h>
#include "CrInc/CrType.h"
#include "CrModel/CrMotion.h"

#ifndef NOZZLE_PARK_POINT
#define NOZZLE_PARK_POINT { (X_MIN_POS + 10), (Y_MAX_POS - 10), 20 }
#endif

//#define FILAMENT_CHANGE_ALERT_BEEPS         10  // Number of alert beeps to play when a response is needed.


#ifdef __cplusplus
extern "C" {
#endif

VOID ReleaseM600Wait();

S32 GcodeM600Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM600Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM600Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
