#ifndef _CRGCODEM3HANDLE_H
#define _CRGCODEM3HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

/*******额外定义的变量和函数********/
#define SPEED_POWER_INTERCEPT         0    // (%) 0-100 i.e., Minimum power percentage
#define SPEED_POWER_MIN               0    // (%) 0-100
#define SPEED_POWER_MAX             100    // (%) 0-100
#define SPEED_POWER_STARTUP          80    //M3、M4命令 速度/功率默认值
#define SPINDLE_INVERT_DIR        false
#define SPINDLE_LASER_ACTIVE_STATE    0
#define SPINDLE_DIR_PIN            NULL
#define SPINDLE_LASER_ENA_PIN      NULL
#define SPINDLE_LASER_PWM_PIN      NULL
#define SPINDLE_LASER_PWM_OFF         0     //占空比

struct LaserInlineStatus_t
{
    U8      power;
    BOOL    IsEnabled;
    BOOL    IsPlanned;
};

extern U16 CutterUnitPower;
extern FLOAT MinPct;
extern FLOAT MaxPct;
extern BOOL IsReady;
extern U16 UnitPower;
extern U16 MenuPower;
extern U8  Power;
extern struct LaserInlineStatus_t StatePower;

U16 CutterCpwrToUpwr(U16 Cpwr);
U16 GetPowerS(VOID *ArgPtr);
VOID CutterInlineDirection();
U8 PctToPwm(FLOAT Pct);
U16 CutterPowerToRange(const U16 Pwr,const U8 PwrUnit);
VOID CutterInlineOcrPower(U8 Ocrpwr);
U16 UpowerToOcr(U16 Upwr);
VOID CutterInlinePower(const U16 Upwr);
VOID CutterSetInlineEnabled(const BOOL Enable);
VOID CutterInlineDisable();
VOID PlannerSynchronize();
VOID AnalogWrite(S16 PIN,S32 PwmValue);
VOID SetOcrState(const U8 Ocr);
VOID SetOcr(const U8 Ocr);
VOID CutterSetOcrPower(const U8 Ocr);
VOID CutterSetReverse(const BOOL Reverse);
VOID CutterSetPower(const U8 Upwr);
VOID CutterSetEnabled(const BOOL Enable);

/*******额外定义的变量和函数********/


S32 GcodeM3Parse(const S8 *CmdBuff, VOID *Arg);       //M3命令的解析函数
S32 GcodeM3Exec(VOID *ArgPtr, VOID *ReplyResult);     //M3命令的执行函数
S32 GcodeM3Reply(VOID *ReplyResult, S8 *Buff);        //M3命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
