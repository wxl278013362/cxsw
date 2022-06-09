/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     CrBeepAppInterface.h
  Author :        FuMin
  Version:        1.0
  Description:    蜂鸣应用接口
  Other:
  Mode History:
          <author>        <time>      <version>     <desc>
          FuMin         2021-01-19     V1.0.0.1
****************************************************************************/

#ifndef _CR_BEEP_APP_INTERFACE_H_
#define _CR_BEEP_APP_INTERFACE_H_

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 短鸣
 * @return
 *     0:  成功
 *     -1：失败
 */
S32 CrBeepAppShortBeeps();

/* 长鸣
 * @return
 *     0:  成功
 *     -1：失败
 */
S32 CrBeepAppLongBeeps();

/* 指定时长的蜂鸣 (MS)
 * @Duration: 蜂鸣时长
 * @return
 *     0:  成功
 *     -1：失败
 */
S32 CrBeepAppFlexsibleBeeps(U32 Duration);

#ifdef __cplusplus
}
#endif

#endif /* _CR_BEEP_APP_INTERFACE_H_ */
