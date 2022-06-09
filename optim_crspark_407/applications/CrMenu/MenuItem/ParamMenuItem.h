#ifndef _PARAMMENUITEM_H
#define _PARAMMENUITEM_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 ParamOptMenuInit();

VOID* ParamSaveOkAction(VOID *ArgOut, VOID *ArgIn);
VOID* ParamLoadOkAction(VOID *ArgOut, VOID *ArgIn);
VOID* ParamRecoverOkAction(VOID *ArgOut, VOID *ArgIn);

VOID* ParamPlaSaveOkAction(VOID *ArgOut, VOID *ArgIn);
VOID* ParamPlaLoadOkAction(VOID *ArgOut, VOID *ArgIn);
VOID* ParamPlaRecoverOkAction(VOID *ArgOut, VOID *ArgIn);

VOID* ParamAbsSaveOkAction(VOID *ArgOut, VOID *ArgIn);
VOID* ParamAbsLoadOkAction(VOID *ArgOut, VOID *ArgIn);
VOID* ParamAbsRecoverOkAction(VOID *ArgOut, VOID *ArgIn);

#ifdef __cplusplus
}
#endif

#endif
