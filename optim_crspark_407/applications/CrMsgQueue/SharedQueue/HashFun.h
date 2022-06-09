#ifndef _HASHFUN_H
#define _HASHFUN_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

VOID Hash(S8 *Out, S32 OutLen, S8 *In, S8 InLen);
S32 BernsteinHash(S8 *Key, S32 Prime);
#ifdef __cplusplus
}
#endif

#endif

