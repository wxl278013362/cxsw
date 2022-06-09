#ifndef _CRFEEDBACKDEV_H
#define _CRFEEDBACKDEV_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "stm32f4xx_hal.h"
#include "CrTemperSensor.h"

class CrSensor
{
public:
    CrSensor(){}
    virtual ~CrSensor(){}

    virtual FLOAT GetVaule() = 0;
};

#define INVALID_TEMP (10000000)
#define SAMPLING_ERROR (10)   //采样误差

class CrTemperSensor : public CrSensor
{
public:
    CrTemperSensor(S8 *SensorName, S8 *AdcPinName, FLOAT AdcReferVol, AdjustFunc  AdcFunc, TemperCalc CalcFunc);      // CalcFunc如果为空则使用默认的计算函数，最好是有自身的计算函数
    ~CrTemperSensor();

    FLOAT GetVaule();

private:
    S32   SensorFd;      //感应器的Fd
    S8    AdcName[12];
    FLOAT LastValidValue;   //上次采样的有效值
};

#endif
