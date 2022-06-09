#include "CrSensor.h"
#include <rtthread.h>
#include <math.h>
CrTemperSensor::CrTemperSensor(S8 *SensorName, S8 *AdcPinName, FLOAT AdcReferVol, AdjustFunc  AdcFunc, TemperCalc CalcFunc)
    :CrSensor(), LastValidValue(0.0f)
{
    if ( !SensorName || !AdcPinName )
        return ;

    if ( (SensorFd = CrTemperSensorOpen(SensorName, 0, 0)) < 0 )
    {
        rt_kprintf("SensorFd < 0\n");
        return ;
    }

    this->LastValidValue = 0.0f;

    struct AdcConfig_t SensorConfig = {
                        .PinName = AdcPinName,
                        .ReferVoltage = AdcReferVol
                        };
    if (CrTemperSensorIoctl(SensorFd, SENSOR_SET_ADC, (VOID *)&SensorConfig) < 0)    //TB1
    {
        rt_kprintf("CrTemperSensorIoctl: SENSOR_SET_ADC failed!\n");
        return ;
    }
    if ( AdcFunc )
    {
        S32 AdcFd = -1;
        CrTemperSensorIoctl(SensorFd, SENSOR_GET_ADC, &AdcFd);
        CrAdcIoctl(AdcFd, ADC_SET_CORRECTION_FUNC_CMD, ( VOID *)AdcFunc);
    }
}

CrTemperSensor::~CrTemperSensor()
{
    S32 AdcFd = -1;
    CrTemperSensorIoctl(SensorFd, SENSOR_GET_ADC, &AdcFd);
    CrTemperSensorClose(SensorFd);
    CrAdcClose(AdcFd);
}

#include "Common/Include/CrCommon.h"

FLOAT CrTemperSensor::GetVaule()
{
    FLOAT Temp = 0.0f;
    FLOAT Average = 0.0f;

    /* Take the average */
    S32 i = 10;
    while ( i-- )
    {
        CrTemperSensorIoctl(SensorFd, SENSOR_GET_TEMPER, &Temp);
        Average += Temp;
    }

    if ( fabs(Average) > 0.00001 )   //值为0
    {
        Temp = Average / 10;
        LastValidValue = Temp;
    }
    else
    {
        if ( fabs(LastValidValue) > 0.000001  )
            Temp = LastValidValue;
        else
        {
            Temp = INVALID_TEMP;
        }
    }

    return Temp;
}

