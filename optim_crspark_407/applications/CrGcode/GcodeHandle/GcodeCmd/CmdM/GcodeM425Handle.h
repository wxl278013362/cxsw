#ifndef _GCODEM425HANDLE_H
#define _GCODEM425HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * CoreXY, CoreXZ, and CoreYZ - and their reverse
 */
#define COREXY 0
#define COREXZ 0
#define COREXZ 0

#if COREXY || COREYX
  #define CORE_IS_XY 1
#endif
#if COREXZ || COREZX
  #define CORE_IS_XZ 1
#endif
#if COREYZ || COREZY
  #define CORE_IS_YZ 1
#endif
#if CORE_IS_XY || CORE_IS_XZ || CORE_IS_YZ
  #define IS_CORE 1
#endif


#if IS_CORE
  #if CORE_IS_XY
    #define CORE_AXIS_1 X
    #define CORE_AXIS_2 Y
    #define NORMAL_AXIS Z
  #elif CORE_IS_XZ
    #define CORE_AXIS_1 X
    #define NORMAL_AXIS Y
    #define CORE_AXIS_2 Z
  #elif CORE_IS_YZ
    #define NORMAL_AXIS X
    #define CORE_AXIS_1 Y
    #define CORE_AXIS_2 Z
  #endif
  //#define CORESIGN(n) (ANY(COREYX, COREZX, COREZY) ? (-(n)) : (n))
#endif

#define BACKLASH_GCODE 1
#define CALIBRATION_GCODE 0

#if (BACKLASH_GCODE || CALIBRATION_GCODE)
#if IS_CORE
    #define X_INDEX 0
    #define Y_INDEX 1
    #define Z_INDEX 2
    #define CAN_CALIBRATE(A,B) (A##_INDEX == B##_INDEX)
#else
    #define CAN_CALIBRATE(A,B) 1
  #endif
#endif

#define AXIS_CAN_CALIBRATE(A) CAN_CALIBRATE(A,NORMAL_AXIS)
//#define AXIS_CAN_CALIBRATE(A) CAN_CALIBRATE(A,Z)


S32 GcodeM425Parse(const S8 *CmdBuff, VOID *Arg);     //M425命令的解析函数
S32 GcodeM425Exec(VOID *ArgPtr, VOID *ReplyResult);   //M425命令的执行函数
S32 GcodeM425Reply(VOID *ReplyResult, S8 *Buff);      //M425命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
