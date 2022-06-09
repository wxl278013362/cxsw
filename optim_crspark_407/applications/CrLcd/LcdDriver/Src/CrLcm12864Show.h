/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :      CrLcm12864Show.h
  Author :        chenguojin
  Version:        1.0
  Description:    12864 显示Posix接口定义
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenguojin     2020-9-30     V1.0.0.1
****************************************************************************/
#ifndef __CRLCM12864SHOW_H__
#define __CRLCM12864SHOW_H__

#include "../U8g2/u8g2.h"
#include "../U8g2/u8x8.h"
#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrLcdShowStruct.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>

/*
    参数    Name: 设备名称
    参数    Flag: 暂无
    参数    Mode: 暂无
    
    返回值      : 返回0调用成功
    描述        : 初始化U8G2
*/
S32 CrLcm12864Open(S8 *Name, S32 Flag, S32 Mode);    //打开12864屏，用flag可以设置字符集

/*
    参数      Fd: CrLcdSpiOpen成功调用后返回的描述符

    返回值      : 无
    描述        : 清除屏幕显示，反初始化SPI
*/
VOID CrLcm12864Close(S32 Fd);  //关闭12864屏

/*
    参数    Name: CrLcdSpiOpen成功调用后返回的描述符
    参数    Buff: 要写入的数据
    参数    Len : 暂无
    
    返回值      : 写图片时成功时返回0，写字符串成功返回长度
    描述        :
*/
S32 CrLcm12864Write(S32 Fd, S8 *Buff, S32 Len);   //写数据

/*
    参数    Name: CrLcdSpiOpen成功调用后返回的描述符
    参数    Buff: 保存读出数据的缓冲区
    参数    Len : 暂无
    
    返回值      :SPI的状态
                HAL_OK       = 0x00U,
                HAL_ERROR    = 0x01U,
                HAL_BUSY     = 0x02U,
                HAL_TIMEOUT  = 0x03U
    描述        :
*/
S32 CrLcm12864Read(S32 Fd, S8 *Buff, S32 Len);      // 读数据

/*
    参数          Fd: CrLcdSpiOpen成功调用后返回的描述符
    参数         Cmd: 参考CrLcm12864Show.h中宏定义
    参数  CtrlParam : 暂无
    
    返回值      : 返回0调用成功
    描述        :
*/
S32 CrLcm12864Ioctl(S32 Fd, U32 Cmd, struct LcdCtrlParam_t *CtrlParam);    //控制

#ifdef CRLCM12864_TEST

S32 CrLcm12864TestMain(VOID);

#endif /*CRLCM12864_TEST*/

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_CRLCM12864SHOW_H*/ 

