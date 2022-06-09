/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     CrSpi.c
  Author :        chenli
  Version:        1.0
  Description:    Spi Posix接口实现
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenli          2020-9-30    V1.0.0.1
****************************************************************************/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include "CrSpi.h"
#include <string.h>
#include "CrGpio/CrGpio.h"
#include <rtthread.h>

struct CrSpi_t
{
    S8  *Name;
    SPI_HandleTypeDef   *Spi;
};

#if 0
SPI_HandleTypeDef   Spi1;
SPI_HandleTypeDef   Spi2;
SPI_HandleTypeDef   Spi3;

struct CrSpi_t  SpiArray[] =
{
    {(S8 *)SPI1_NAME, &Spi1},
    {(S8 *)SPI2_NAME, &Spi2},
    {(S8 *)SPI3_NAME, &Spi3}
};
#endif
struct CrSpi_t  SpiArray[] =
{
    {(S8 *)SPI1_NAME, (SPI_HandleTypeDef   *)NULL},  //动态分配内存，防止不必要的内存占用
    {(S8 *)SPI2_NAME, (SPI_HandleTypeDef   *)NULL},
    {(S8 *)SPI3_NAME, (SPI_HandleTypeDef   *)NULL}
};

#define SPI2_TRANSMIT_TIMEOUT 100
#define SPI2_RECEIVE_TIMEOUT  100
/*
    参数      : 无

    返回值    : 返回SpiArray成员个数
    描述      : 计算SpiArray成员个数
*/
static U8 SpiGroupCount(VOID)
{
    return sizeof(SpiArray) / sizeof(SpiArray[0]);
}

/*
    参数    Name: 设备名称

    返回值      : SpiArray数组下标
    描述        :
*/
static S32 GetSpiGroupIndex(S8 *Name)
{
    S32 Ret = -1;
    U8 Flag = 0;

    if ( !Name )
        return Ret;

    for ( Flag = 0; Flag < SpiGroupCount(); Flag++)
    {
        if ( strncmp(Name, SpiArray[Flag].Name, strlen(Name)) == 0 )
        {
            Ret = Flag;
            break;
        }
    }

    return Ret;
}

static S32 CrSpiGpioInit(struct SpiPinInit_t *Param)
{
    if ( !Param )
        return -1;

    S32 PinFd = CrGpioOpen(Param->GpioPinName, 0, 0);

    if ( PinFd < 65536 )
        return -1;

    struct PinInitParam_t Pin = {0};
    struct PinParam_t CsPinState = {GPIO_PIN_RESET};

    Pin.Mode = Param->Mode;
    Pin.Pull = Param->Pull;
    Pin.Speed = Param->Speed;
    CsPinState.PinState = Param->PinState;

    CrGpioIoctl(PinFd, GPIO_INIT, &Pin);
    CrGpioIoctl(PinFd, GPIO_SET_PINSTATE, &CsPinState);
    return PinFd;
}

static S32 CrSpiInit(S32 Fd, VOID *Param)
{
    if ( ( !Param ) || ( Fd < 0 ) ||  Fd >= SpiGroupCount() )
        return -1;

    if( SpiArray[Fd].Spi == NULL)
    {
        SpiArray[Fd].Spi = (SPI_HandleTypeDef*)rt_malloc(sizeof(SPI_HandleTypeDef));

        if(SpiArray[Fd].Spi == NULL)
        {
            return -1;
        }
    }

    if ( Fd == 0 )
    {
        SpiArray[Fd].Spi->Instance = SPI1;
        __HAL_RCC_SPI1_CLK_ENABLE();
    }
    else if ( Fd == 1 )
    {
        SpiArray[Fd].Spi->Instance = SPI2;
        __HAL_RCC_SPI2_CLK_ENABLE();
    }
    else if ( Fd == 2 )
    {
        SpiArray[Fd].Spi->Instance = SPI3;
        __HAL_RCC_SPI3_CLK_ENABLE();
    }
    else
    {
        if( SpiArray[Fd].Spi != NULL)
        {
            rt_free(SpiArray[Fd].Spi);
        }

        return -1;
    }

    struct SpiInitParam_t *SpiParam = (struct SpiInitParam_t *)Param;
    SpiArray[Fd].Spi->Init.Mode = SpiParam->Mode;
    SpiArray[Fd].Spi->Init.Direction = SpiParam->Duplex;
    SpiArray[Fd].Spi->Init.DataSize = SpiParam->DataSize;
    SpiArray[Fd].Spi->Init.CLKPolarity = SpiParam->CLKPolarity;
    SpiArray[Fd].Spi->Init.CLKPhase = SpiParam->CLKPhase;
    SpiArray[Fd].Spi->Init.NSS = SpiParam->NSS;
    SpiArray[Fd].Spi->Init.BaudRatePrescaler = SpiParam->BaudRatePrescaler;
    SpiArray[Fd].Spi->Init.FirstBit = SpiParam->Endian;
    SpiArray[Fd].Spi->Init.TIMode = SpiParam->TIMode;
    SpiArray[Fd].Spi->Init.CRCCalculation = SpiParam->CRCCalculation;
    SpiArray[Fd].Spi->Init.CRCPolynomial = SpiParam->CRCPolynomial;

    CrSpiGpioInit(&(SpiParam->CsPin));
    CrSpiGpioInit(&(SpiParam->MisoPin));
    CrSpiGpioInit(&(SpiParam->MosiPin));
    CrSpiGpioInit(&(SpiParam->SpiSck));

    if ( HAL_SPI_Init(SpiArray[Fd].Spi) != HAL_OK )
    {
        if( SpiArray[Fd].Spi != NULL)
        {
            rt_free(SpiArray[Fd].Spi);
        }

        return -1;
    }

    return 0;
}

/*
    参数    Name: 设备名称
    参数    Flag: 暂无
    参数    Mode: 暂无

    返回值      : 成功返回数组下标，失败返回-1;
    描述        :
*/
S32 CrSpiOpen(S8 *Name, S32 Flag, S32 Mode)
{
    if ( NULL != Name )
        return GetSpiGroupIndex(Name);

    return -1;
}

/*
    参数      Fd: CrSpiOpen成功调用后返回的描述符

    返回值      :
    描述        :
*/
VOID CrSpiClose(S32 Fd)
{
    if ( ( Fd < 0 ) || Fd >= SpiGroupCount() )
        return ;

    HAL_SPI_DeInit(SpiArray[Fd].Spi);

    return ;
}

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
S32 CrSpiWrite(S32 Fd, S8 *Buff, S32 Len)
{
    if ( ( Fd < 0 ) || Fd >= SpiGroupCount() || ( !Buff ) )
        return -1;

    while ( HAL_SPI_GetState(SpiArray[Fd].Spi) != HAL_SPI_STATE_READY );

    return HAL_SPI_Transmit(SpiArray[Fd].Spi, (U8 *)Buff, Len, SPI2_TRANSMIT_TIMEOUT);
}

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
S32 CrSpiRead(S32 Fd, S8 *Buff, S32 Len)
{
    if ( ( Fd < 0 ) || Fd >= SpiGroupCount() ||  ( !Buff ) )
        return -1;

    while ( HAL_SPI_GetState(SpiArray[Fd].Spi) != HAL_SPI_STATE_READY );
    
    return HAL_SPI_Receive(SpiArray[Fd].Spi, (U8 *)Buff, Len, SPI2_RECEIVE_TIMEOUT);
}

/*
    参数     Fd: CrSpiOpen成功调用后返回的描述符
    参数   Cmd : SPI_INIT
    参数 Param : 初始化SPI的参数

    返回值    : SPI的状态
                HAL_OK       = 0x00U,
                HAL_ERROR    = 0x01U,
                HAL_BUSY     = 0x02U,
                HAL_TIMEOUT  = 0x03U
    描述      : 初始化SPI
*/
S32 CrSpiIoctl(S32 Fd, S32 Cmd, struct SpiInitParam_t *Param)
{
    S32 Ret = -1;

    if ( (Fd < 0) || (Fd >= SpiGroupCount()) || ( !Param ) )
        return Ret;
 
    S32 Temp = Fd;

    switch ( Cmd )
    {
        case SPI_INIT: 
        {
            Ret = CrSpiInit(Temp, Param);
            break;
        }
        default:
            return Ret;
    }

    return Ret;
}

#ifdef CRSPI_TEST

/*參考LcdSpi.c*/

#endif /*CRSPI_TEST*/
