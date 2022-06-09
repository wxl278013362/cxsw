#ifndef _CRGCODEM401HANDLE_H
#define _CRGCODEM401HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * M401: Deploy the bed probe. The Z axis may raise up to make room for the probe to deploy.
 */

#define HAS_BED_PROBE   1

#define Z_CLEARANCE_DEPLOY_PROBE   6    // Z Clearance for Deploy/Stow
#define Z_CLEARANCE_BETWEEN_PROBES  5   // Z Clearance between probe points

#if HAS_BED_PROBE

S32 GcodeM401Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM401Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM401Reply(VOID *ReplyResult, S8 *Buff);
BOOL SetDeployed(const BOOL Deploy);     //探针伸出收起函数
VOID SetZprobeEnabled(const BOOL Onoff);
BOOL GetZprobeEnabled(VOID);
VOID ReportCurrentPosition(VOID);

#endif

#ifdef __cplusplus
}
#endif

#endif



