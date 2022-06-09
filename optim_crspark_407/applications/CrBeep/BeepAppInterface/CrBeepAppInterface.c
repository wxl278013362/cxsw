/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     CrBeepAppInterface.c
  Author :        FuMin
  Version:        1.0
  Description:    蜂鸣应用接口
  Other:
  Mode History:
          <author>        <time>      <version>     <desc>
          FuMin         2021-01-19     V1.0.0.1
****************************************************************************/

#include <ctype.h>
#include "CrBeepAppInterface.h"
#include "CrBeep/BeepDriver/CrBeep.h"

/* 短鸣
 * @return
 *     0:  成功
 *     -1：失败
 */
S32 CrBeepAppShortBeeps()
{
    /* 打开蜂鸣器 */
    S32 BeepFd = CrBeepOpen(BEEP_PIN_NAME, 0, 0);
    if ( BeepFd < 0 ) return -1;

    /* 短鸣 */
    CrBeepIoctl(BeepFd,  BEED_CMD_SHORT_BEEPS, NULL);

    /* 关闭蜂鸣器 */
    CrBeepClose(BeepFd);

    return 0;
}

/* 长鸣
 * @return
 *     0:  成功
 *     -1：失败
 */
S32 CrBeepAppLongBeeps()
{
    /* 打开蜂鸣器 */
    S32 BeepFd = CrBeepOpen(BEEP_PIN_NAME, 0, 0);
    if ( BeepFd < 0 ) return -1;

    /* 长鸣 */
    CrBeepIoctl(BeepFd, BEED_CMD_LONG_BEEPS, NULL);

    /* 关闭蜂鸣器 */
    CrBeepClose(BeepFd);

    return 0;
}

/* 指定时长的蜂鸣
 * @Duration: 蜂鸣时长 (MS)
 * @return
 *     0:  成功
 *     -1：失败
 */
S32 CrBeepAppFlexsibleBeeps(U32 Duration)
{
    /* 打开蜂鸣器 */
    S32 BeepFd = CrBeepOpen(BEEP_PIN_NAME, 0, 0);
    if ( BeepFd < 0 ) return -1;

    /* 指定时长的蜂鸣 */
    CrBeepIoctl(BeepFd, BEED_CMD_UNFIXED_BEEPS, (VOID *)Duration);

    /* 关闭蜂鸣器 */
    CrBeepClose(BeepFd);

    return 0;
}









