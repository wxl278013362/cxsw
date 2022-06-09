#ifndef _CRGCODEM165HANDLE_H
#define _CRGCODEM165HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

/********外部需要引用的变量*************/
#define MIXING_STEPPERS 2   //混合挤出机个数
#define MIXING_VIRTUAL_TOOLS 1
enum MixTool {
    FIRST_USER_VIRTUAL_TOOL = 0
  , LAST_USER_VIRTUAL_TOOL = MIXING_VIRTUAL_TOOLS - 1
  , NR_USER_VIRTUAL_TOOLS
  , MIXER_DIRECT_SET_TOOL = NR_USER_VIRTUAL_TOOLS
  #if HAS_MIXER_SYNC_CHANNEL
    , MIXER_AUTORETRACT_TOOL
  #endif
  , NR_MIXING_VIRTUAL_TOOLS
};

extern U8  MixerSelectedvtool;
extern S16 MixerColor[NR_MIXING_VIRTUAL_TOOLS][MIXING_STEPPERS];

#ifdef __AVR__
#define COLOR_A_MASK 0x8000
#else
#define COLOR_A_MASK 0x80
#endif


/********外部需要引用的变量*************/


S32 GcodeM165Parse(const S8 *CmdBuff, VOID *Arg); //M165命令的解析函数
S32 GcodeM165Exec(VOID *ArgPtr, VOID *ReplyResult);   //M165命令的执行函数
S32 GcodeM165Reply(VOID *ReplyResult, S8 *Buff);   //M165命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
