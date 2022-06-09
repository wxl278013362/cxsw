/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     FanPwmCallback.h
  Author :        chenli
  Version:        1.0
  Description:
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenli          2021-1-20    V1.0.0.1
****************************************************************************/
#ifndef __FANPWMCALLBACK_H__
#define __FANPWMCALLBACK_H__

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif  /*__cplusplus*/


VOID FanPwmRun(VOID *PData);


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__FANPWMCALLBACK_H__*/
