#ifndef _CRFANCTTRL_H
#define _CRFANCTTRL_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrGpio/CrGpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MODEL_FAN_PIN "PE7"

#define CMD_FAN_OPEN                0x01
#define CMD_FAN_CLOSE               0x02

S32     CrFanOpen(S8 *Name, S32 Flag, S32 Mode);  //就是使用的Gpio接口的操作
S32     CrFanClose(S32 Fd);
S32     CrFanIoctl(S32 Fd, U32 Cmd, VOID *Arg);

#ifdef TEST_SENSOR
VOID TestFan();
#endif

#ifdef __cplusplus
}
#endif

#endif




