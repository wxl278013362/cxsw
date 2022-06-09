#ifndef _CRCOMMON_H
#define _CRCOMMON_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

VOID SystemReset();   //系统重置

VOID PrintFloat(FLOAT Value, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
