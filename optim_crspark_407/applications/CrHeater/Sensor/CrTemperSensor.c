/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     CrTemperSensor.c
  Author :        chenli
  Version:        1.0
  Description:    Temp Sensor Posix
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenli          2020-12-09    V1.0.0.1
****************************************************************************/
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "../Adc/CrAdc.h"
#include "CrTemperSensor.h"

#define COUNT(a)                    (sizeof(a)/sizeof(*a))

#define KA                          273.15          //绝对温度
#define T2                          (KA + 25)
#define RP                          100000          //温感阻值
#define BX                          3950            //温感B值
#define RT                          4700            //电路中分压电阻的阻值

struct TemperSensor_t
{
    S32        TemperSensorFd;
    S32        AdcFd;
    FLOAT      Rp;  //25℃时的电阻值
    FLOAT      Bx;  //温感的B值
    FLOAT      Rt;  //电路中分压电阻的阻值（上拉电阻值）
    S8         SensorName[MAX_SENSOR_NAME_LENGTH];
    TemperCalc CalcFunc;                  //计算函数指针  (默认是给空还是别的计算函数)
};

struct TemperSensor_t TempSensorArray[] = 
{
	{-1, -1, RP, BX, RT, SENSOR_NOZZLE_NAME, NULL},
	{-1, -1, RP, BX, RT, SENSOR_BED_NAME   , NULL}
};

static float DefaultCalcFunc(float Volt, float ReferVoltage)
{
    float Rt = 0.0;
    float temp = -1;
/*
    printf("ADCVreal = %d.%d\n", (S32)(Volt * 1000) /1000 , (S32)(Volt * 1000) % 1000);
    printf("ADCVRefer = %d.%d\n", (S32)(ReferVoltage * 1000) /1000 , (S32)(ReferVoltage * 1000) % 1000);
*/
    if ( ReferVoltage - Volt <= 0 )
        return -1;

    Rt = Volt * RT / (ReferVoltage - Volt);
    temp = ( 1.0 / ( (log(Rt / RP)) / BX + 1.0 / (T2) ) - KA );

    return temp;
}

static BOOL IsFdVaild(S32 Fd)
{
    BOOL Ret = 0;

    if ( Fd < 0 || Fd >= COUNT(TempSensorArray) )
        Ret = -1;

    return Ret;
}

static S32 CmdGetTempProcess(S32 Fd, float *RetTemp)
{
    struct AdcOutPutParam_t Temp = {0};

    if ( CrAdcIoctl(TempSensorArray[Fd].AdcFd, ADC_GET_VALUE_CMD, (VOID *)&Temp) < 0 )
        return -1;

    if ( TempSensorArray[Fd].CalcFunc )
    {
        *RetTemp = TempSensorArray[Fd].CalcFunc(Temp.AdcVolt);
    }
    else
    {
        *RetTemp = DefaultCalcFunc(Temp.AdcVolt, Temp.ReferVoltage);
    }
/*
    if ( *RetTemp < -40 )
        printf("Err:Plese check TempSensor!\n");
    else
        printf("Temp = %d.%d°\n", (S32)(*RetTemp * 1000) /1000 , (S32)(*RetTemp * 1000) % 1000);
*/
    return 0;
}

S32 CrTemperSensorOpen(S8 *Name, S32 Flag, S32 Mode)
{
    S32 Ret = -1;

    if ( !Name || strlen(Name) > MAX_SENSOR_NAME_LENGTH )
        return Ret;

    for ( S8 i = 0; i < COUNT(TempSensorArray); ++i )
    {
        if ( strcmp(TempSensorArray[(S32)i].SensorName, Name) == 0 )
        {
            TempSensorArray[(S32)i].TemperSensorFd = (S32)i;
            Ret = TempSensorArray[(S32)i].TemperSensorFd;
            break;
        }
    }

    return Ret;
}

S32 CrTemperSensorClose(S32 Fd)
{
    if ( IsFdVaild(Fd) < 0)
        return -1;

    CrAdcClose(TempSensorArray[Fd].AdcFd);
    TempSensorArray[Fd].AdcFd = -1;
    TempSensorArray[Fd].CalcFunc = NULL;
    TempSensorArray[Fd].TemperSensorFd = -1;
    return 0;
}

S32 CrTemperSensorIoctl(S32 Fd, U32 Cmd, VOID *Arg)
{
    S32 Ret = -1;

    if ( !Arg )
        return Ret;

    if ( IsFdVaild(Fd) < 0)
        return Ret;

    switch (Cmd )
    {
        case SENSOR_SET_ADC:
        {
            struct AdcParam_t AdcReferVoltage = {1.0};
            AdcReferVoltage.ReferVoltage = ((struct AdcConfig_t *)Arg)->ReferVoltage;
            TempSensorArray[Fd].AdcFd = CrAdcOpen(((struct AdcConfig_t *)Arg)->PinName, 0, 0);                 //设置温感对应的ADC引脚
            Ret = CrAdcIoctl(TempSensorArray[Fd].AdcFd, ADC_SET_PARAM_CMD, (VOID *)&AdcReferVoltage); //设置ADC参考电压
            break;
        }
        case SENSOR_GET_ADC:
        {
            *(S32 *)Arg = TempSensorArray[Fd].AdcFd;
            break;
        }
        case SENSOR_GET_TEMPER:
        {
            Ret = CmdGetTempProcess(Fd, (float *)Arg);
            break;
        }
        case SENSOR_SET_TEMPER_CALC_FUNC:
        {
            TempSensorArray[Fd].CalcFunc = (TemperCalc)Arg;
            Ret = 1;
            break;
        }
        default:
            break;
    }

    return Ret;
}

#ifdef CR_TEMP_SENSOR_TEST
#include "rtthread.h"
VOID TempSensorTestMain()
{
    float Temp = 0;
    S32 AdcFd = -1;
    S32 BedFd = CrTemperSensorOpen(SENSOR_BED_NAME, 0, 0);
    struct AdcConfig_t BedAdcConfig = {BED_PIN_NAME, DEFAULT_ADC_REFER_VOLTAGE};
    CrTemperSensorIoctl(BedFd, SENSOR_SET_ADC, (VOID *)&BedAdcConfig);    //TB1
    CrTemperSensorIoctl(BedFd, SENSOR_GET_ADC, &AdcFd);
    CrTemperSensorIoctl(BedFd, SENSOR_GET_TEMPER, &Temp);
    printf("Get Adc Fd = %d \n\n\n", AdcFd);

    S32 NozFd = CrTemperSensorOpen(SENSOR_NOZZLE_NAME, 0, 0);
    struct AdcConfig_t NozAdcConfig = {NOZZLE_PIN_NAME, DEFAULT_ADC_REFER_VOLTAGE};
    printf("NozFd = %d, BedFd = %d\n", NozFd, BedFd);
    CrTemperSensorIoctl(NozFd, SENSOR_SET_ADC, (VOID *)&NozAdcConfig);   //TH1
    CrTemperSensorIoctl(NozFd, SENSOR_GET_TEMPER, &Temp);
    CrTemperSensorIoctl(NozFd, SENSOR_GET_ADC, &AdcFd);
    printf("Get Adc Fd = %d \n", AdcFd);
    CrTemperSensorClose(BedFd);
    CrTemperSensorClose(NozFd);
}

MSH_CMD_EXPORT(TempSensorTestMain, Test Temp Sensor)
#endif
