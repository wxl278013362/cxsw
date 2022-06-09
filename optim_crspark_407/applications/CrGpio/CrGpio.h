/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     CrGpio.h
  Author :        chenguojin
  Version:        1.0
  Description:    Gpio Posix接口定义
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenguojin     2020-9-30     V1.0.0.1
****************************************************************************/

#ifndef __CRGPIO_H__
#define __CRGPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>

//组名
#define     GPIO_PA  "PA"
#define     GPIO_PB  "PB"
#define     GPIO_PC  "PC"
#define     GPIO_PD  "PD"
#define     GPIO_PE  "PE"
#define     GPIO_PF  "PF"
#define     GPIO_PG  "PG"

//定义命令
#define     GPIO_INIT           0x01    //初始化管脚
#define     GPIO_GET_PINSTATE   0x02    //获取管脚状态
#define     GPIO_SET_PINSTATE   0x04    //设置管脚状态

//常用管脚
#define LED_PIN "PB2"    //LED管脚

//GPIO引脚的速度
#define GPIO_PIN_SPEED_FREQ_LOW GPIO_SPEED_FREQ_LOW                //2MHz
#define GPIO_PIN_SPEED_FREQ_MEDIUM GPIO_SPEED_FREQ_MEDIUM          //12.5MHz to 50Mhz
#define GPIO_PIN_SPEED_FREQ_HIGH GPIO_SPEED_FREQ_HIGH              //25MHz to 100MHz
#define GPIO_PIN_SPEED_FREQ_VERY_HIGH GPIO_SPEED_FREQ_VERY_HIGH    //50MHz to 200MHz

//GPIO引脚的拉高和拉低
#define GPIO_PIN_NOPULL GPIO_NOPULL
#define GPIO_PIN_PULLUP GPIO_PULLUP
#define GPIO_PIN_PULLDOWN GPIO_PULLDOWN

//GPIO引脚模式
#define  GPIO_PIN_MODE_INPUT                  GPIO_MODE_INPUT
#define  GPIO_PIN_MODE_OUTPUT_PP              GPIO_MODE_OUTPUT_PP
#define  GPIO_PIN_MODE_OUTPUT_OD              GPIO_MODE_OUTPUT_OD
#define  GPIO_PIN_MODE_AF_PP                  GPIO_MODE_AF_PP
#define  GPIO_PIN_MODE_AF_OD                  GPIO_MODE_AF_OD

#define  GPIO_PIN_MODE_ANALOG                 GPIO_MODE_ANALOG

#define  GPIO_PIN_MODE_IT_RISING              GPIO_MODE_IT_RISING
#define  GPIO_PIN_MODE_IT_FALLING             GPIO_MODE_IT_FALLING
#define  GPIO_PIN_MODE_IT_RISING_FALLING      GPIO_MODE_IT_RISING_FALLING

#define  GPIO_PIN_MODE_EVT_RISING             GPIO_MODE_EVT_RISING
#define  GPIO_PIN_MODE_EVT_FALLING            GPIO_MODE_EVT_FALLING
#define  GPIO_PIN_MODE_EVT_RISING_FALLING     GPIO_MODE_EVT_RISING_FALLING

struct PinParam_t
{
    GPIO_PinState   PinState;           //电平状态
};

struct PinInitParam_t
{
    U32 Mode;                           //引脚的工作模式 （具体参数查看stm32f1xx_hal_gpio.h line:114）
    U32 Pull;                           //引脚默认电平   （具体参数查看stm32f1xx_hal_gpio.h line:153）
    U32 Speed;                          //引脚速度      （具体参数查看stm32f1xx_hal_gpio.h line:141）
};

//GPIO 管脚

/*
    功能    判断Fd是否在有效范围内

    参数  Fd: CrGpioOpen 调用后返回的描述符
    返回值   : 0   有效
           -1   无效
*/
S32 IsGpioFdVaild(S32 Fd);

/*
    功能      ：获取GPIO的Fd， Name管脚名称(组名 + Pin编号。）
               如：名称PA2是由组名PA和Pin编号2组成。
               特殊的Pin编号有All,pin编号从0-15)
    参数  Name: 设备名称
    参数  Flag: 暂无
    参数  Mode: 暂无
    
    返回值    : 设备描述符（大于65535调用成功）
    描述      :
*/
S32  CrGpioOpen(S8 *Name, U32 Flag, U32 Mode);     

/*
    参数    Fd: CrGpioOpen 调用后返回的描述符

    返回值    : 无
    描述      :
*/
VOID CrGpioClose(S32 Fd);

/*
    参数    Fd: CrGpioOpen 调用后返回的描述符
    参数  Buff: 用户数据指针
    参数   Len: 暂无

    返回值  : 等于0调用成功否则失败
    描述    :
*/
S32  CrGpioWrite(S32 Fd, S8 *Buff, S32 Len);        //Buff[0]存放电平状态

/*
    参数    Fd: CrGpioOpen 调用后返回的描述符
    参数  Buff: 用户数据指针
    参数   Len: 暂无

    返回值  : 大于等于0调用成功返回值为引脚电平
    描述    :
*/
S32  CrGpioRead(S32 Fd, S8 *Buff, S32 Len);

/*
    参数    Fd: CrGpioOpen 调用后返回的描述符
    参数   Cmd: 命令
    参数 Param: 用户数据指针

    返回值  : 无
    描述    :
*/
S32  CrGpioIoctl(S32 Fd, S32 Cmd, VOID *Param);     //判断Fd是否是有效的范围

/*
 * 功能：获取引脚的名称
    参数 PinIndex: Pin的索引
    参数 NameBuff: Pin名称缓冲区
    返回值  : 0成功，非0失败
    描述    :
*/
S32 CrGpioGetPinName(S32 PinIndex, S8 *NameBuff);

#ifdef CRGPIO_TEST

S32 CrGpioTestMain(VOID);

#endif /*CRGPIO_TEST*/

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_CRGPIO_H*/
