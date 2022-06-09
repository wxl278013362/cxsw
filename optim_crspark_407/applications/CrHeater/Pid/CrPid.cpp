#include <stm32f4xx_hal.h>
#include "CrPid.h"
#include <string.h>
#include <stdio.h>

#define ABS(a)  ( ((a) > 0) ? (a) : (-a))

CrPid::CrPid(struct PidAttr_t &Attr, struct PidParam_t &Param)
{
    memcpy(&PidAttr, &Attr, sizeof(struct PidAttr_t));
    memcpy(&PidParm, &Param, sizeof(struct PidParam_t));
    LastError = 0.0f;
    return;
}

CrPid::~CrPid()
{
    memset(&PidAttr, 0, sizeof(struct PidAttr_t));
    memset(&PidParm, 0, sizeof(struct PidParam_t));
    LastError = 0.0f;
}

VOID  CrPid::SetPidAttr(struct PidAttr_t &Attr)
{
    memcpy(&PidAttr, &Attr, sizeof(struct PidAttr_t));
    return;
}
struct PidAttr_t CrPid::GetPidAttr()
{
    return PidAttr;
}

VOID CrPid::SetPidParam(struct PidParam_t &Param)
{
    memcpy(&PidParm, &Param, sizeof(struct PidParam_t));
    return;
}
struct PidParam_t CrPid::GetPidParam()
{
    return PidParm;
}

/*
 * 增量式PID
 * */
CrIncrementalPid::CrIncrementalPid(struct PidAttr_t &Attr, struct PidParam_t &Param)
    : CrPid(Attr, Param)
{
    SecondLastError = 0.0f;
    return ;
}

CrIncrementalPid::~CrIncrementalPid()
{
    SecondLastError = 0.0f;
}

float CrIncrementalPid::PidCalc(float Target, float FeadBack)
{
    struct PidAttr_t PidConfig = GetPidAttr();
//    struct PidParam_t PidLimitConfig = GetPidParam();

    float CurError = Target - FeadBack, Increase = 0.0f;

    Increase = PidConfig.Kp * (CurError - LastError)                          //比例p
             + PidConfig.Ki *  CurError                                      //积分i
             + PidConfig.Kd * (CurError - 2 * LastError + SecondLastError); //微分d

    SecondLastError = LastError;
    LastError = CurError;

    return Increase;                           //控制量的增量
}

/*
 * 位置式PID
 * */
CrPositionalPid::CrPositionalPid(struct PidAttr_t &Attr, struct PidParam_t &Param)
    : CrPid(Attr, Param)
{
    ErrorSum = 0.0f;
    return ;
}
CrPositionalPid::~CrPositionalPid()
{
    ErrorSum = 0.0f;
}

float CrPositionalPid::PidCalc(float Target, float FeadBack)
{
    struct PidAttr_t PidConfig = GetPidAttr();
    struct PidParam_t PidLimitConfig = GetPidParam();

    static float PrevRealize = 0.0f;

    BOOL Index = 0;
    float CurError = Target - FeadBack;
    float Realize = 0.0f;

    float KI = 0;

    if ( ABS(CurError) > PidLimitConfig.DeadZone )
    {
        if ( ABS(CurError) < PidLimitConfig.IntegralSaturation )        //积分分离法
        {
            if ( PrevRealize >= PidLimitConfig.SystemSaturation )       //U(k-1) > Umax    抗积分饱和
            {
                if ( CurError < 0 )                                     //只累加负方向的误差
                {
                    ErrorSum += CurError;
                }
            }
            else if ( PrevRealize <= 0 )                                //U(k-1) < Umin
            {
                if ( CurError > 0 )                                     //只累加正方向的误差
                {
                    ErrorSum += CurError;
                }
            }
            else
            {
                ErrorSum += CurError;
            }

            Index = 1;
        }

        KI = PidConfig.Ki * ErrorSum * Index;
        Realize = PidConfig.Kp * CurError                       //比例p
                + KI                                            //积分
                + PidConfig.Kd * (CurError - LastError);        //微分d
    }

    LastError = CurError;
    LIMIT(Realize, 0, PidLimitConfig.SystemSaturation);         //输出限幅

    PrevRealize = Realize;

    return Realize;
}
