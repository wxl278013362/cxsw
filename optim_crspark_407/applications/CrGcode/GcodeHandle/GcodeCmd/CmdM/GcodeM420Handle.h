#ifndef _CRGCODEM420HANDLE_H
#define _CRGCODEM420HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUTO_BED_LEVELING_BILINEAR  //abl
//#define MESH_BED_LEVELING           //mbl
//#define AUTO_BED_LEVELING_UBL       //ubl

#define EEPROM_SETTINGS
#define HAS_LEVELING
#define HAS_MESH
#define ENABLE_LEVELING_FADE_HEIGHT

#define GRID_MAX_CELLS_X    GRID_MAX_POINTS_X - 1
#define GRID_MAX_CELLS_Y    GRID_MAX_POINTS_Y - 1

#ifdef HAS_LEVELING

S32 GcodeM420Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM420Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM420Reply(VOID *ReplyResult, S8 *Buff);
VOID BedLevelVirtInterpolate(VOID);

#endif
#ifdef __cplusplus
}
#endif

#endif





