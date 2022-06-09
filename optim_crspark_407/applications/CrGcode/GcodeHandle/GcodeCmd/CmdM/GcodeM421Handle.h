#ifndef _CRGCODEM421HANDLE_H
#define _CRGCODEM421HANDLE_H

#include "GcodeM420Handle.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GRID_MAX_POINTS_X 3
#define GRID_MAX_POINTS_Y GRID_MAX_POINTS_X
#define GRID_MAX_POINTS ((GRID_MAX_POINTS_X) * (GRID_MAX_POINTS_Y))

#define EXTENSIBLE_UI

S32 GcodeM421Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM421Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM421Reply(VOID *ReplyResult, S8 *Buff);

#ifdef AUTO_BED_LEVELING_BILINEAR
    extern FLOAT AblZvalues[GRID_MAX_POINTS_X][GRID_MAX_POINTS_Y];
#endif
#ifdef MESH_BED_LEVELING
    extern FLOAT MblZvalues[GRID_MAX_POINTS_X][GRID_MAX_POINTS_Y];
#endif
#ifdef AUTO_BED_LEVELING_UBL
    extern FLOAT UblZvalues[GRID_MAX_POINTS_X][GRID_MAX_POINTS_Y];
#endif


#ifdef __cplusplus
}
#endif

#endif

