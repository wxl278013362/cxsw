#ifndef _GCODEM666HANDLE_H
#define _GCODEM666HANDLE_H

#include <stdio.h>
#include "CrInc/CrType.h"
#include "CrMotion/StepperApi.h"

#define Z_HOME_DIR (-1)
#define DELTA (1)

//每个轴有多个限位
#define Z_TRIPLE_ENDSTOPS (0)
#define Z_DUAL_ENDSTOPS (0)
#define Y_DUAL_ENDSTOPS (0)
#define X_DUAL_ENDSTOPS (0)

#define Z_MULTI_ENDSTOPS (Z_TRIPLE_ENDSTOPS || Z_DUAL_ENDSTOPS)
#define HAS_EXTRA_ENDSTOPS (Z_MULTI_ENDSTOPS || Y_DUAL_ENDSTOPS || X_DUAL_ENDSTOPS)

#if DELTA
extern FLOAT DeltaEndstopAdj[AxisNum];

#elif HAS_EXTRA_ENDSTOPS

#if X_DUAL_ENDSTOPS
extern FLOAT X2EndstopAdj;
#endif

#if Y_DUAL_ENDSTOPS
extern FLOAT Y2EndstopAdj;
#endif
#if Z_MULTI_ENDSTOPS
extern FLOAT Z2EndstopAdj;
#endif

#if Z_TRIPLE_ENDSTOPS
extern FLOAT Z3EndstopAdj;
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM666Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM666Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM666Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
