#ifndef _CRGCODEM126HANDLE_H
#define _CRGCODEM126HANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

#define Constrain(value, arg_min, arg_max) ((value) < (arg_min) ? (arg_min) :((value) > (arg_max) ? (arg_max) : (value)))

#define BARICUDA

#ifdef BARICUDA

#define HEATER0_PIN     "PB15"
#define HEATER1_PIN     "PC8"

#define _PIN(IO) (DIO ## IO ## _PIN)

#define PIN_EXISTS(PN) (defined(PN##_PIN) && PN##_PIN >= 0)

#if PIN_EXISTS(HEATER0_PIN)
    #define HAS_HEATER_0 1
#endif
#if PIN_EXISTS(HEATER1_PIN)
    #define HAS_HEATER_1 1
#endif

#if HAS_HEATER_0
S32 GcodeM126Parse(const S8 *CmdBuff, VOID *Arg);       //M126命令的解析函数
S32 GcodeM126Exec(VOID *ArgPtr, VOID *ReplyResult);     //M126命令的执行函数
S32 GcodeM126Reply(VOID *ReplyResult, S8 *Buff);            //M126命令的回复函数

extern U8 BaricudavalvePressure;
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif

