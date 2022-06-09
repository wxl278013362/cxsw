#ifndef _CRTEMPSENSOR_H
#define _CRTEMPSENSOR_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "../Adc/CrAdc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SENSOR_NAME_LENGTH  12

#define SENSOR_NOZZLE_NAME      "Nozzle"        //根据主板的实际情况确定
#define SENSOR_BED_NAME         "Bed"           //根据主板的实际情况确定
#define SENSOR_NAME             ""              //根据主板的实际情况确定

//温度传感器的操作命令
#define SENSOR_SET_ADC              0x01         //设置传感器对应的ADC引脚及参考电压，使二者相关联
#define SENSOR_GET_ADC              0x02         //获取传感器使用的ADC的Fd
#define SENSOR_GET_TEMPER           0x03         //获取温度值
#define SENSOR_SET_TEMPER_CALC_FUNC 0x04         //设置计算函数

//定义计算函数指针的类型
typedef float (*TemperCalc)(float Volt);
float TemperCalcK100(float Volt);        //参考marlin命名
float TemperCalcThermistor(float Volt);  //参考marlin命名

struct AdcConfig_t
{
    S8    *PinName;           //温感对应的ADC引脚名称
    float ReferVoltage;       //ADC基准电压
};

S32 CrTemperSensorOpen(S8 *Name, S32 Flag, S32 Mode);
S32 CrTemperSensorClose(S32 Fd);
S32 CrTemperSensorIoctl(S32 Fd, U32 Cmd, VOID *Arg);


#define CR_TEMP_SENSOR_TEST

#ifdef TEST_TEMPER_SENSOR
VOID TestTemperSensor();
#endif

#ifdef __cplusplus
}
#endif

#endif







