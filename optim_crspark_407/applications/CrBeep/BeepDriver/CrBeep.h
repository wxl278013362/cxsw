/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     CrBeep.h
  Author :        FuMin
  Version:        1.0
  Description:    蜂鸣BSP接口
  Other:
  Mode History:
          <author>        <time>      <version>     <desc>
          FuMin         2021-01-18     V1.0.0.1
****************************************************************************/

#ifndef _CRBEEP_H_
#define _CRBEEP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "CrInc/CrType.h"

/*
 * IoCtrl指令
 */
#define BEED_CMD_SHORT_BEEPS   1  /* 设置短鸣 */
#define BEED_CMD_LONG_BEEPS    2  /* 设置长鸣 */
#define BEED_CMD_UNFIXED_BEEPS 3  /* 设置指定时间的蜂鸣 (MS) */

#define SHORT_BEEP_DURATION  3     /* 短鸣时间 (MS) */
#define LONG_BEEP_DURATION   150  /* 长鸣时间 (MS) */

#define BEEP_PIN_NAME ("PC6")

/* 打开蜂鸣器
 * @Name: 蜂鸣器引脚名称
 * @Flag: NA
 * @Mode: NA
 * @Return: 设备节点Fd
 */
S32 CrBeepOpen(S8 *Name, U32 Flag, U32 Mode);

/* 关闭蜂鸣器
 * @Fd: 设备节点Fd
 */
VOID CrBeepClose(S32 Fd);

/* IO控制蜂鸣器
 * @Fd: 设备节点Fd
 * @Cmd:
 *     BEED_CMD_SHORT_BEEPS    设置短鸣
 *     BEED_CMD_LONG_BEEPS     设置长鸣
 *     BEED_CMD_UNFIXED_BEEPS  设置指定时间的蜂鸣 (MS)
 * @Param:
 *           指定时间 (BEED_CMD_UNFIXED_BEEPS), 数值类型强制转换成VOID*
 * @Return:
 *     -1: 失败
 *     0:  成功
 */
S32 CrBeepIoctl(S32 Fd, S32 Cmd, VOID *Param);

/*
 * API单元测试
 */
#define BEEP_UNIT_TEST
#ifdef BEEP_UNIT_TEST
void BeepUnitTest();
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _BEEP_H_ */
