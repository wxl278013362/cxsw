/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     CrLcdSpi.c
  Author :        chenli
  Version:        1.0
  Description:    LCD Spi Posix接口实现
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenli          2020-9-30    V1.0.0.1
****************************************************************************/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include "CrLcdSpi.h"
#include "CrSpi.h"
#include <string.h>
/*
    参数  Name: 设备名称
    参数  Flag: 暂无
    参数  Mode: 暂无

    返回值    : 设备描述符（大于等于0调用成功）
    描述      :
*/
S32 CrLcdSpiOpen(S8 *Name, S32 Flag, S32 Mode)
{
    return CrSpiOpen(Name, Flag, Mode);
}

/*
    参数  Fd: CrLcdSpiOpen成功调用后返回的描述符

    返回值    : 无
    描述      :
*/
VOID CrLcdSpiClose(S32 Fd)
{
    CrSpiClose(Fd);
    return ;
}

/*
    参数    Fd: CrLcdSpiOpen成功调用后返回的描述符
    参数  Buff: 暂无
    参数  Len : 暂无

    返回值    : SPI的状态
                HAL_OK       = 0x00U,
                HAL_ERROR    = 0x01U,
                HAL_BUSY     = 0x02U,
                HAL_TIMEOUT  = 0x03U
    描述      :
*/
S32 CrLcdSpiWrite(S32 Fd, S8 *Buff, S32 Len)
{
    return CrSpiWrite(Fd, Buff, Len);
}

/*
    参数    Fd: CrLcdSpiOpen成功调用后返回的描述符
    参数    Buff: 保存读出数据的缓冲区
    参数    Len : 暂无


    返回值    : SPI的状态
                HAL_OK       = 0x00U,
                HAL_ERROR    = 0x01U,
                HAL_BUSY     = 0x02U,
                HAL_TIMEOUT  = 0x03U
    描述      :
*/
S32 CrLcdSpiRead(S32 Fd, S8 *Buff, S32 Len)
{
    return CrSpiRead(Fd, Buff, Len);
}

/*
    参数    Fd: CrLcdSpiOpen成功调用后返回的描述符
    参数    Cmd: 暂无
    参数    Param : 暂无

    返回值    : 返回0调用成功
    描述      : 初始化LcdSpi
*/
S32 CrLcdSpiIoctl(S32 Fd, S32 Cmd, VOID *Param)
{
    struct  SpiInitParam_t LcdSpiInitParam = {0};

    LcdSpiInitParam.Mode = SPI_MODE_MASTER;
    LcdSpiInitParam.Duplex = SPI_DIRECTION_1LINE;
    LcdSpiInitParam.DataSize = SPI_DATASIZE_8BIT;
    LcdSpiInitParam.CLKPolarity = SPI_POLARITY_LOW;
    LcdSpiInitParam.CLKPhase = SPI_PHASE_1EDGE;
    LcdSpiInitParam.NSS = SPI_NSS_SOFT;
    LcdSpiInitParam.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
    LcdSpiInitParam.Endian = SPI_FIRSTBIT_MSB;
    LcdSpiInitParam.TIMode = SPI_TIMODE_DISABLE;
    LcdSpiInitParam.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    LcdSpiInitParam.CRCPolynomial = 10;

    /*SPI相关引脚配置*/
/*    strcpy(LcdSpiInitParam.MisoPin.GpioPinName, SPI2_MISO_PIN);
    LcdSpiInitParam.MisoPin.Mode = GPIO_MODE_AF_INPUT;
    LcdSpiInitParam.MisoPin.Pull = GPIO_NOPULL;
    LcdSpiInitParam.MisoPin.Speed = GPIO_SPEED_FREQ_HIGH;*/

    strcpy(LcdSpiInitParam.MosiPin.GpioPinName, SPI2_MOSI_PIN);
    LcdSpiInitParam.MosiPin.Mode = GPIO_MODE_AF_PP;
    LcdSpiInitParam.MosiPin.Speed = GPIO_SPEED_FREQ_VERY_HIGH;//GPIO_SPEED_FREQ_HIGH;

    strcpy(LcdSpiInitParam.SpiSck.GpioPinName, SPI2_SCK_PIN);
    LcdSpiInitParam.SpiSck.Mode = GPIO_MODE_AF_PP;
    LcdSpiInitParam.SpiSck.Speed = GPIO_SPEED_FREQ_VERY_HIGH;//GPIO_SPEED_FREQ_HIGH;

    strcpy(LcdSpiInitParam.CsPin.GpioPinName, SPI2_NSS_PIN);
    LcdSpiInitParam.CsPin.Mode = GPIO_MODE_OUTPUT_PP;
    LcdSpiInitParam.CsPin.Speed = GPIO_SPEED_FREQ_VERY_HIGH;//GPIO_SPEED_FREQ_HIGH;
    LcdSpiInitParam.CsPin.Pull = GPIO_PULLUP;
    LcdSpiInitParam.CsPin.PinState = GPIO_PIN_SET;

    return CrSpiIoctl(Fd, SPI_INIT, &LcdSpiInitParam);
}


#ifdef CRLCDSPI_TEST

S32 CrlcdSpiTestMain(VOID)
{
    S32 SpiFd = CrLcdSpiOpen(SPI2_NAME, 0, 0);
    if ( SpiFd < 0 )
    {
        printf("Lcd Spi OpenFail\n");
        return SpiFd;
    }

    CrLcdSpiIoctl(SpiFd, 0, NULL);  //初始化SPI

    return SpiFd;
}

#endif /*CRLCDSPI_TEST*/
