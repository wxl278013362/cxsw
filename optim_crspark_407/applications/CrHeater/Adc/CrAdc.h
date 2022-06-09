#ifndef _CRADC_H
#define _CRADC_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USING_OS_RTTHREAD
#include <rtdef.h>
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_adc_ex.h"

#endif

typedef float (* AdjustFunc)(float);    //定义ADC校正函数指针

#define DEFAULT_ADC_REFER_VOLTAGE (3.30f)
//一般使用的是12位Adc，所以用的是此数值来表示ConvertBits；如果有变化可以使用ADC_SET_PARAM_CMD命令进行设置
#define DEFAULT_ADC_CONVERT_BITS (1 << 12)     

//ADC 对应的引脚名称 (引脚要进行初始化，具体操作参考Gpio)
#define BED_PIN_NAME           "PF3" //"PC4"     //热床对应的引脚的名称
#define NOZZLE_PIN_NAME        "PF4" //"PC5"     //喷嘴对应的引脚的名称
#define ADC3_PIN_NAME          ""        //ADC3对应的引脚的名称

//操作ADC命令
#define ADC_SET_PARAM_CMD               0x01        //设置adc参数(参与实际计算，所以必须按实际的物理设备传入参数进行初始化)
#define ADC_GET_PARAM_CMD               0x02        //获取adc参数
#define ADC_GET_VALUE_CMD               0x04        //获取ADC的值
#define ADC_SET_CORRECTION_FUNC_CMD     0x08        //设置矫正函数

//Adc参数
struct AdcParam_t
{
    float ReferVoltage;       //Adc的参考电压(单位：伏特)
};

/*
因为热敏电阻和串联的电阻的总电压是参考的adc的参考电压
在传感器中是不知道这个电压的所以在给出adc参数时给出电压
*/
struct AdcOutPutParam_t
{
    float ReferVoltage;         //Adc的参考电压(单位：伏特)
    float AdcVolt;              //adc的实际电压值
};

S32 CrAdcOpen(S8 *AdcPin, S32 Flag, S32 Mode);
S32 CrAdcClose(S32 Fd);
S32 CrAdcIoctl(S32 Fd, U32 Cmd, VOID *Arg);

#ifdef CR_ADC_TEST
VOID CrAdcTest();
#endif

#ifdef __cplusplus
}
#endif

#endif
