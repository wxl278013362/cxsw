#ifndef _CRPID_H
#define _CRPID_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"

#ifdef USING_OS_RTTHREAD
#include <rtthread.h>
#endif

#if 0
//PID参数
struct PidAttr_t
{
    float   Kp;         //
    float   Ki;         //
    float   Kd;         //
};
struct PidParam_t
{
    float  DeadZone;               //死区(输出在此范围内的值，则不做任何操作)
    float  IntegralSaturation;     //积分饱和值
    float  SystemSaturation;       //系统饱和值
};
#endif

#define LIMIT(Value ,Min , Max) (  (Value < Min) ?  (Value = Min)  : ((Value > Max)  ? (Value = Max) : Value ))

// temperature.cpp strings
#define STR_PID_AUTOTUNE_START              "PID Autotune start"
#define STR_PID_BAD_EXTRUDER_NUM            "PID Autotune failed! Bad extruder number"
#define STR_PID_TEMP_TOO_HIGH               "PID Autotune failed! Temperature too high"
#define STR_PID_TIMEOUT                     "PID Autotune failed! timeout"
#define STR_BIAS                            " bias: "
#define STR_D_COLON                         " d: "
#define STR_T_MIN                           " min: "
#define STR_T_MAX                           " max: "
#define STR_KU                              " Ku: "
#define STR_TU                              " Tu: "
#define STR_CLASSIC_PID                     " Classic PID "
#define STR_KP                              " Kp: "
#define STR_KI                              " Ki: "
#define STR_KD                              " Kd: "
#define STR_PID_AUTOTUNE_FINISHED           "PID Autotune finished! Put the last Kp, Ki and Kd constants from below into Configuration.h"
#define STR_PID_DEBUG                       " PID_DEBUG "
#define STR_PID_DEBUG_INPUT                 ": Input "
#define STR_PID_DEBUG_OUTPUT                " Output "
#define STR_PID_DEBUG_PTERM                 " pTerm "
#define STR_PID_DEBUG_ITERM                 " iTerm "
#define STR_PID_DEBUG_DTERM                 " dTerm "
#define STR_PID_DEBUG_CTERM                 " cTerm "
#define STR_INVALID_EXTRUDER_NUM            " - Invalid extruder number !"

class CrPid
{
public:
    CrPid(struct PidAttr_t &Attr, struct PidParam_t &Param);
    virtual ~CrPid();

    //注意此处算出来的是占空，不是占空比
    virtual float PidCalc(float Target, float FeedBack) = 0;      //实际值, 目标值(通过它们可以计算出此处的差值。

    VOID  SetPidAttr(struct PidAttr_t &Attr);                   //to do
    struct PidAttr_t  GetPidAttr();       //to do

    VOID  SetPidParam(struct PidParam_t &Param);                  //to do
    struct PidParam_t GetPidParam(); //TO DO

protected:
    struct PidAttr_t   PidAttr;                //PID参数
    struct PidParam_t  PidParm;
    float              LastError;               //上一次的差值
};

//增量式PID
class CrIncrementalPid : public CrPid
{
public:
    CrIncrementalPid(struct PidAttr_t &Attr, struct PidParam_t &Param);
    ~CrIncrementalPid();

    float PidCalc(float Target, float FeadBack);

private:
    float SecondLastError;    //上上次的差值
};

//位置式PID
class CrPositionalPid : public CrPid
{
public:
    CrPositionalPid(struct PidAttr_t &Attr, struct PidParam_t &Param);
    ~CrPositionalPid();
    float PidCalc(float Target, float FeadBack);

private:
    float ErrorSum;         //差值的和
};

#endif
