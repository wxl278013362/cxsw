#ifndef _CRGCODEM350HANDLE_H
#define _CRGCODEM350HANDLE_H

#include "CrInc/CrType.h"
#include "CrMotion/StepperApi.h"

#ifndef HIGH
#define HIGH         0x01
#endif

#ifndef LOW
#define LOW          0x00
#endif

#define HAS_MICROSTEPS 1
#if HAS_MICROSTEPS

  // MS1 MS2 MS3 Stepper Driver Microstepping mode table
  #ifndef MICROSTEP1
    #define MICROSTEP1 LOW,LOW,LOW
  #endif
  #ifdef HEROIC_STEPPER_DRIVERS
    #ifndef MICROSTEP128
      #define MICROSTEP128 LOW,HIGH,LOW
    #endif
  #else
    #ifndef MICROSTEP2
      #define MICROSTEP2 HIGH,LOW,LOW
    #endif
    #ifndef MICROSTEP4
      #define MICROSTEP4 LOW,HIGH,LOW
    #endif
  #endif
  #ifndef MICROSTEP8
    #define MICROSTEP8 HIGH,HIGH,LOW
  #endif
  #ifdef __SAM3X8E__
    #if MB(ALLIGATOR)
      #ifndef MICROSTEP16
        #define MICROSTEP16 LOW,LOW,LOW
      #endif
      #ifndef MICROSTEP32
        #define MICROSTEP32 HIGH,HIGH,LOW
      #endif
    #else
      #ifndef MICROSTEP16
        #define MICROSTEP16 HIGH,HIGH,LOW
      #endif
    #endif
  #else
    #ifndef MICROSTEP16
      #define MICROSTEP16 HIGH,HIGH,LOW
    #endif
  #endif

  #ifdef MICROSTEP1
    #define HAS_MICROSTEP1 1
  #endif
  #ifdef MICROSTEP2
    #define HAS_MICROSTEP2 1
  #endif
  #ifdef MICROSTEP4
    #define HAS_MICROSTEP4 1
  #endif
  #ifdef MICROSTEP8
    #define HAS_MICROSTEP8 1
  #endif
  #ifdef MICROSTEP16
    #define HAS_MICROSTEP16 1
  #endif
  #ifdef MICROSTEP32
    #define HAS_MICROSTEP32 1
  #endif
  #ifdef MICROSTEP64
    #define HAS_MICROSTEP64 1
  #endif
  #ifdef MICROSTEP128
    #define HAS_MICROSTEP128 1
  #endif

#endif // HAS_MICROSTEPS



#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM350Parse(const S8 *CmdBuff, VOID *Arg); //M350命令的解析函数
S32 GcodeM350Exec(VOID *ArgPtr, VOID *ReplyResult);   //M350命令的执行函数
S32 GcodeM350Reply(VOID *ReplyResult, S8 *Buff);   //M350命令的回复函数


VOID MicrostepMs(enum Axis_t  AxisType, U8 Ms1Level, U8 Ms2Level, U8 Ms3Level);
VOID SteppersMicrostepReadings();

#ifdef __cplusplus
}
#endif

#endif
