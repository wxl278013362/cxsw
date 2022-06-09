#ifndef _CRCONTROLLER_H
#define _CRCONTROLLER_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrLcd/KnobEvent/CrKnobEvent.h"

#ifdef __cplusplus
extern "C" {
#endif

S32  CrControllerInit(S32 KnobFd);
VOID CrControllerUnInit();

#ifdef __cplusplus
}
#endif

#endif
