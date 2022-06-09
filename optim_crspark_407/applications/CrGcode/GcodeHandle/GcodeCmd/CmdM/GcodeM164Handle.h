#ifndef _CRGCODEM164HANDLE_H
#define _CRGCODEM164HANDLE_H

#include "GcodeM163Handle.h"
#include "../../GcodeType.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MIXING_EXTRUDER

#define GRADIENT_VTOOL
#define MIXING_VIRTUAL_TOOLS 16  // Use the Virtual Tool method with M163 and M164
#define COLOR_A_MASK    0x8000
#define COLOR_MASK      0x7FFF

#define MIXER_STEPPER_LOOP(VAR) for (uint_fast8_t VAR = 0; VAR < MIXING_STEPPERS; VAR++)
#define NOLESS(v, n) \
  do{ \
    __typeof__(v) _n = (n); \
    if (_n > v) v = _n; \
  }while(0)

#define COPY(a,b) do{ \
    static_assert(sizeof(a[0]) == sizeof(b[0]), "COPY: '" STRINGIFY(a) "' and '" STRINGIFY(b) "' types (sizes) don't match!"); \
    memcpy(&a[0],&b[0],MIN(sizeof(a),sizeof(b))); \
  }while(0)

enum MixTool {
    FIRST_USER_VIRTUAL_TOOL = 0,
    LAST_USER_VIRTUAL_TOOL = MIXING_VIRTUAL_TOOLS - 1,
    NR_USER_VIRTUAL_TOOLS,
    MIXER_DIRECT_SET_TOOL = NR_USER_VIRTUAL_TOOLS,
    MIXER_AUTORETRACT_TOOL,
    NR_MIXING_VIRTUAL_TOOLS,
};

struct Gradient_t{
    BOOL    Enabled;                   // This gradient is enabled
    S8      GradientColor[MIXING_STEPPERS];      // The current gradient color
    FLOAT   StartZ;                  // Region for gradient
    FLOAT   EndZ;
    S8      StartVtool, EndVtool;      // Start and end virtual tools
    S8      StartMix[MIXING_STEPPERS];  // Start and end mixes from those tools
    S8      EndMix[MIXING_STEPPERS];
#ifdef GRADIENT_VTOOL
    S32     VtoolIndex;                 // Use this virtual tool number as index
#endif
};

S32 GcodeM164Parse(const S8 *CmdBuff, VOID *Arg);       //M164命令的解析函数
S32 GcodeM164Exec(VOID *ArgPtr, VOID *ReplyResult);     //M164命令的执行函数
S32 GcodeM164Reply(VOID *ReplyResult, S8 *Buff);             //M164命令的回复函数

VOID SetVtool(U8 Val);
U8 GetVtool();

extern U8 Color[NR_MIXING_VIRTUAL_TOOLS][MIXING_STEPPERS];
extern S8 Mix[MIXING_STEPPERS];

#endif

#ifdef __cplusplus
}
#endif

#endif


