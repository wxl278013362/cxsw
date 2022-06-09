/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     CrLcdSpi.h
  Author :        chenguojin
  Version:        1.0
  Description:    Lcd Spi Posix接口定义
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenguojin     2020-9-30     V1.0.0.1
****************************************************************************/
#ifndef __CRLCDSPI_H__
#define __CRLCDSPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>
#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#define  SPI1_NAME  "SPI1"
#define  SPI2_NAME  "SPI2"
#define  SPI3_NAME  "SPI3"


/*SPI相关引脚宏定义*/
#define  SPI1_MOSI_PIN  "PB7"
#define  SPI1_MISO_PIN  "PA6"
#define  SPI1_SCK_PIN   "PA5"
#define  SPI1_NSS_PIN   "PA4"

#define  SPI2_MOSI_PIN  "PB15"
#define  SPI2_MISO_PIN  "PB14"
#define  SPI2_SCK_PIN   "PB13"
#define  SPI2_NSS_PIN   "PB12"

#define  SPI3_MOSI_PIN
#define  SPI3_MISO_PIN
#define  SPI3_SCK_PIN
#define  SPI3_NSS_PIN


//Lcd 的Spi参数都写成固定的

/*
    参数  Name: 设备名称
    参数  Flag: 暂无
    参数  Mode: 暂无

    返回值    : 设备描述符（大于等于0调用成功）
    描述      :
*/
S32     CrLcdSpiOpen(S8 *Name, S32 Flag, S32 Mode);    //获取SPI的Fd并完成初始化工作

/*
    参数  Fd: CrLcdSpiOpen成功调用后返回的描述符

    返回值    : 无
    描述      :
*/
VOID    CrLcdSpiClose(S32 Fd);

/*
    参数    Fd: CrLcdSpiOpen成功调用后返回的描述符
    参数  Buff: 要写入的数据
    参数  Len : 暂无

    返回值    : SPI的状态
                HAL_OK       = 0x00U,
                HAL_ERROR    = 0x01U,
                HAL_BUSY     = 0x02U,
                HAL_TIMEOUT  = 0x03U
    描述      :
*/
S32     CrLcdSpiWrite(S32 Fd, S8 *Buff, S32 Len);

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
S32     CrLcdSpiRead(S32 Fd, S8 *Buff, S32 Len);

/*
    参数    Fd: CrLcdSpiOpen成功调用后返回的描述符
    参数    Cmd: 暂无
    参数    Param : 暂无

    返回值    : 返回0调用成功
    描述      : 初始化LcdSpi
*/
S32     CrLcdSpiIoctl(S32 Fd, S32 Cmd, VOID *Param);

#ifdef CRLCDSPI_TEST

S32 CrlcdSpiTestMain(VOID);

#endif /*CRLCDSPI_TEST*/


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_CRLCDSPI_H*/
