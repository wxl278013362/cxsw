/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     CrAdc.c
  Author :        chenli
  Version:        1.0
  Description:    Adc Posix接口实现
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenli          2020-12-09    V1.0.0.1
****************************************************************************/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_def.h"
#include <stdio.h>
#include <string.h>
#include <rtthread.h>
#include "CrAdc.h"
#include "CrGpio/CrGpio.h"

#define COUNT(a)                (sizeof(a)/sizeof(*a))

#define ADC_SCAN_MODE DISABLE

static ADC_HandleTypeDef Adc1Handle, Adc2Handle, Adc3Handle;

struct AdcDeviceOpt_t
{
    S8                      PinName[8];     //用作ADC信号输入的管脚号
    struct AdcParam_t       Param;
    S32                     Channel;        //Adc通道 (通道是给一个负数值作为默认的还是给一个0，这个在open时会进行初始化，这个时候的通道不一定是adc所需要的 )
    AdjustFunc              Func;           //校正函数
    ADC_TypeDef             *Instance;
    ADC_HandleTypeDef       *AdcHandle;
    S32                     PinFd;
};

struct AdcDeviceOpt_t AdcArray[] =
{
    {BED_PIN_NAME   , {1.00}, ADC_CHANNEL_9/*ADC_CHANNEL_14*/, NULL, ADC3, NULL, -1},
    {NOZZLE_PIN_NAME, {1.00}, ADC_CHANNEL_14/*ADC_CHANNEL_15*/, NULL, ADC3, NULL, -1}
};

static S32 AdcRegArr[COUNT(AdcArray)] = {0};

static S32 CrAdcPinInit(S8 *PinName)
{   
    if ( !PinName )
        return -1;

    struct PinInitParam_t AdcPinParam = {0};
    AdcPinParam.Mode = GPIO_MODE_ANALOG;
    AdcPinParam.Speed = GPIO_SPEED_FREQ_HIGH;

    S32 AdcPinFd = CrGpioOpen(PinName, 0, 0);
    if ( AdcPinFd < 0 )
        return -1;

    CrGpioIoctl(AdcPinFd, GPIO_INIT, (VOID *)&AdcPinParam);

    return AdcPinFd;
}

static BOOL IsFdVaild(S32 Fd)
{
    BOOL Ret = 0;

    if ( Fd < 0 )
        Ret = -1;

    return Ret;
}

static S32 ADCInit(struct AdcDeviceOpt_t *Param)
{
    if ( !Param )
        return -1;

    if ( Param->Instance == ADC1 )
    {
        Param->AdcHandle = &Adc1Handle;
        __HAL_RCC_ADC1_CLK_ENABLE();
    }
    else if ( Param->Instance == ADC2 )
    {
        Param->AdcHandle = &Adc2Handle;
        __HAL_RCC_ADC2_CLK_ENABLE();
    }
    else
    {
        Param->AdcHandle = &Adc3Handle;
        __HAL_RCC_ADC3_CLK_ENABLE();
    }

    Param->AdcHandle->Instance = Param->Instance;
    Param->AdcHandle->Init.ScanConvMode = ADC_SCAN_MODE;
    Param->AdcHandle->Init.ContinuousConvMode = DISABLE;
    Param->AdcHandle->Init.DiscontinuousConvMode = DISABLE;
    Param->AdcHandle->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    Param->AdcHandle->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    Param->AdcHandle->Init.NbrOfConversion = 1;
    if ( HAL_ADC_Init(Param->AdcHandle) != HAL_OK )
    {
        return -1;
    }
    return 0;
}

static S8 FindArraySubscript(S32 Fd)
{
    S8 Ret = -1;
    for ( S8 i = 0; i < COUNT(AdcArray); i++ )
    {
        if ( Fd == (S32)(AdcArray[(S32)i].Instance) + (S32)(AdcArray[(S32)i].Channel) )
        {
            Ret = i;
            break;
        }
    }

    return Ret;
}

static float CalcActualVoltage(U32 AdcValue, float ReferenceVoltage)
{
    return  (AdcValue * ReferenceVoltage) / DEFAULT_ADC_CONVERT_BITS;
}

static S32 CrAdcRead(S8 Subscript, VOID *Buff)
{
    if ( !Buff )
        return -1;

    S32 Ret = -1;
    U32 AdValue = 0;
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel = AdcArray[(S32)Subscript].Channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;

    if ( HAL_ADC_ConfigChannel( AdcArray[(S32)Subscript].AdcHandle, &sConfig) != HAL_OK )
        return -1;

//    HAL_ADCEx_Calibration_Start(AdcArray[(S32)Subscript].AdcHandle);

    HAL_ADC_Start(AdcArray[(S32)Subscript].AdcHandle);
    HAL_ADC_PollForConversion(AdcArray[(S32)Subscript].AdcHandle, 200);    //等待转换完成，第二个参数表示超时时间，单位ms
    if( HAL_IS_BIT_SET(HAL_ADC_GetState(AdcArray[(S32)Subscript].AdcHandle), HAL_ADC_STATE_REG_EOC) )
    {
        AdValue = HAL_ADC_GetValue(AdcArray[(S32)Subscript].AdcHandle);
        Ret = 0;
    }
    HAL_ADC_Stop(AdcArray[(S32)Subscript].AdcHandle);

    if ( AdcArray[(S32)Subscript].Func )
    {
        ((struct AdcOutPutParam_t *)Buff)->AdcVolt = AdcArray[(S32)Subscript].Func(CalcActualVoltage(AdValue, AdcArray[(S32)Subscript].Param.ReferVoltage));
    }
    else
    {
        ((struct AdcOutPutParam_t *)Buff)->AdcVolt = CalcActualVoltage(AdValue, AdcArray[(S32)Subscript].Param.ReferVoltage);
    }

    ((struct AdcOutPutParam_t *)Buff)->ReferVoltage = AdcArray[(S32)Subscript].Param.ReferVoltage;

    return Ret;
}

static S32 CrAdcCmdProcess(S32 Cmd, S8 Subscript, VOID *Arg)
{
    S32 Ret = -1;

    switch (Cmd)
    {
        case ADC_SET_PARAM_CMD:
        {
            AdcArray[(S32)Subscript].Param.ReferVoltage = ((struct AdcParam_t *)Arg)->ReferVoltage;
            Ret = 0;
            break;
        }
        case ADC_GET_PARAM_CMD:
        {
            memcpy(Arg, &(AdcArray[(S32)Subscript].Param), sizeof(struct AdcParam_t));
            Ret = 0;
            break;
        }
        case ADC_GET_VALUE_CMD:
        {
            Ret = CrAdcRead(Subscript, Arg);
            break;
        }
        case ADC_SET_CORRECTION_FUNC_CMD:
        {
            AdcArray[(S32)Subscript].Func = (AdjustFunc)Arg;
            Ret = 0;
            break;
        }
        default:
            break;
    }

    return Ret;
}

static S32 CrAdcCloseProcess(S8 Subscript, S32 Fd)
{
    CrGpioClose(AdcArray[(S32)Subscript].PinFd);
    AdcArray[(S32)Subscript].PinFd = -1;
    AdcArray[(S32)Subscript].AdcHandle = NULL;
    AdcArray[(S32)Subscript].Func = NULL;
    AdcArray[(S32)Subscript].Param.ReferVoltage = 1.00f;

    for ( S8 Flag = 0; Flag < COUNT(AdcRegArr); Flag++ )
    {
        if ( Fd == AdcRegArr[(S32)Flag] )
        {
            AdcRegArr[(S32)Flag] = 0;
            break;
        }
    }

    return 0;
}

S32 CrAdcOpen(S8 *AdcPin, S32 Flag, S32 Mode)
{
   S32 Ret = -1;
   S32 Subscript = 0;

   for ( S8 i = 0; i < COUNT(AdcArray); i++ )
   {
       if ( strcmp(AdcArray[(S32)i].PinName, AdcPin) == 0 )
       {
           AdcArray[(S32)i].PinFd = CrAdcPinInit(AdcPin);

           if ( AdcArray[(S32)i].PinFd < 0 )
               return Ret;

           Ret = (S32)(AdcArray[(S32)i].Instance) + (S32)AdcArray[(S32)i].Channel;

           for ( S32 j = 0; j < COUNT(AdcRegArr); ++j )
           {
               if ( Ret == AdcRegArr[j] )
               {
                   Ret = -1;
                   return Ret;
               }

               if ( AdcRegArr[j] == 0 )
               {
                   Subscript = j;
               }

               if ( j == COUNT(AdcRegArr) - 1 )
               {
                   AdcRegArr[Subscript] = Ret;
               }
           }

           if ( ADCInit(&AdcArray[(S32)i]) != 0 )
           {
               Ret = -1;
               AdcRegArr[Subscript] = 0;
           }
           break;
       }
   }

   return Ret;
}

S32 CrAdcClose(S32 Fd)
{
    if ( IsFdVaild(Fd) < 0 )
        return -1;

    S8 Subscript = FindArraySubscript(Fd);
    if ( Subscript < 0 || Subscript > COUNT(AdcArray) )
        return -1;

    return CrAdcCloseProcess(Subscript, Fd);
}

S32 CrAdcIoctl(S32 Fd, U32 Cmd, VOID *Arg)
{
    if ( IsFdVaild(Fd) < 0 )
        return -1;

    S8 Subscript = FindArraySubscript(Fd);
    if ( Subscript < 0 || Subscript > COUNT(AdcArray) )
        return -1;

    return CrAdcCmdProcess(Cmd, Subscript, Arg);
}

#ifdef CR_ADC_TEST
#include "rtthread.h"
VOID CrAdcTest()
{
    struct AdcOutPutParam_t Temp = {0};
    struct AdcParam_t Tem = {DEFAULT_ADC_REFER_VOLTAGE};

    S32 Fd1 = CrAdcOpen(NOZZLE_PIN_NAME,0,0);
    S32 Fd = CrAdcOpen(BED_PIN_NAME,0,0);
    printf("%d, %d\n", Fd, Fd1);

    CrAdcIoctl(Fd, ADC_SET_PARAM_CMD, (VOID *)&Tem);
    CrAdcIoctl(Fd, ADC_GET_VALUE_CMD, (VOID *)&Temp);

    printf("1Voltage = %d.%d\n", (S32)(Temp.AdcVolt * 10000) /10000 , (S32)(Temp.AdcVolt * 10000) % 10000);
    memset(&Temp, 0, sizeof(Temp));

    CrAdcIoctl(Fd1, ADC_SET_PARAM_CMD, (VOID *)&Tem);
    CrAdcIoctl(Fd1, ADC_GET_VALUE_CMD, (VOID *)&Temp);
    printf("2Voltage = %d.%d\n", (S32)(Temp.AdcVolt * 10000) /10000 , (S32)(Temp.AdcVolt * 10000) % 10000);
    CrAdcClose(Fd);
    CrAdcClose(Fd1);
}

MSH_CMD_EXPORT(CrAdcTest, Adc Test)

#endif

