/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     CrSpi.h
  Author :        chenguojin
  Version:        1.0
  Description:    Spi Posix接口实现
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenguojin     2020-9-30     V1.0.0.1
****************************************************************************/
#ifndef __CRSPI_H__
#define __CRSPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrLcdShowStruct.h"

#define  SPI1_NAME  "SPI1"
#define  SPI2_NAME  "SPI2"
#define  SPI3_NAME  "SPI3"

#if 0
//Ioctl的命令
#define     SPI_INIT    0x01    //SPI初始化

//SPI初始化参数
struct  SpiInitParam
{
    U32     Mode;                       //工作模式  ： 作为从机 还是主机
    U32     Duplex;                     //全双工or半双工  Duplex SPI_DIRECTION_2LINES, SPI_DIRECTION_2LINES_RXONLY, SPI_DIRECTION_1LINE
    U32     DataSize;                   //数据大小
    U32     CLKPolarity;                //时钟极性：SPI_POLARITY_LOW表示SPI总线在空闲时SCK为低电平，SPI_POLARITY_HIGH表示SPI总线在空闲时SCK为高电平
    U32     CLKPhase;                   //时钟相位：SPI_PHASE_1EDGE表示在第一个时钟延采样，SPI_PHASE_2EDGE表示在第二个时钟延采样
    U32     NSS;                        //SPI_NSS_SOFT 表示使用软件片选（随便一个IO口都可以） SPI_NSS_HARD_OUTPUT 或 SPI_NSS_HARD_INTPUT 表示使用硬件片选，引脚必须为SPI_NSS
    U32     BaudRatePrescaler;          //分频系数，控制SPI总线的速度
    U32     Endian;                     //SPI_FIRSTBIT_MSB表示数据高位先传，SPI_FIRSTBIT_LSB 表示数据低位先传
    U32     TIMode;                     //
    U32     CRCCalculation;             //是否使用校验  enable or disable
    U32     CRCPolynomial;              //校验位  (校验寄存器中的校验函数指针)
};
#endif


//Spi
/*
    参数    Name: 设备名称
    参数    Flag: 暂无
    参数    Mode: 暂无

    返回值      : 成功返回数组下标，失败返回-1;
    描述        :
*/
S32     CrSpiOpen(S8 *Name, S32 Flag, S32 Mode);    //获取SPI的Fd，在Ioctl中完成初始化

/*
    参数      Fd: CrSpiOpen成功调用后返回的描述符

    返回值      :
    描述        :
*/
VOID    CrSpiClose(S32 Fd);

/*
    参数      Fd: CrSpiOpen成功调用后返回的描述符
    参数  Buff: 要写入的数据
    参数  Len : 暂无

    返回值    : SPI的状态
                HAL_OK       = 0x00U,
                HAL_ERROR    = 0x01U,
                HAL_BUSY     = 0x02U,
                HAL_TIMEOUT  = 0x03U
    描述      :
*/
S32     CrSpiWrite(S32 Fd, S8 *Buff, S32 Len);

/*
    参数      Fd: CrSpiOpen成功调用后返回的描述符
    参数  Buff: 存放读出的数据的缓冲区
    参数  Len : 暂无

    返回值    : SPI的状态
                HAL_OK       = 0x00U,
                HAL_ERROR    = 0x01U,
                HAL_BUSY     = 0x02U,
                HAL_TIMEOUT  = 0x03U
    描述      :
*/
S32     CrSpiRead(S32 Fd, S8 *Buff, S32 Len);

/*
    参数     Fd: CrSpiOpen成功调用后返回的描述符
    参数   Cmd : SPI_INIT
    参数 Param : 初始化SPI的参数

    返回值    : 成功返回0
    描述      : 初始化SPI
*/
S32     CrSpiIoctl(S32 Fd, S32 Cmd, struct SpiInitParam_t *Param);

#ifdef CRSPI_TEST

/*參考LcdSpi.c*/

#endif /*CRSPI_TEST*/

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_CRSPI_H*/
