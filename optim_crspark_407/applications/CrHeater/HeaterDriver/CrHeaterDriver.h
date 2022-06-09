#ifndef _CRHEATERDRIVER_H
#define _CRHEATERDRIVER_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

//加热器名称
#define HEATER_NOZZLE_NAME "Nozzle"
#define HEATER_BED_NAME "Bed"

//加热管脚的名称
//#define BED_HEATTING_PIN_NAME       "PD12" //ender-3:"PA2"
//#define NOZZLE_HEATTING_PIN_NAME    "PB1"  //ender-3:"PA1"
#define BED_HEATTING_PIN_NAME       "PB15" //ender-3:"PA2"
#define NOZZLE_HEATTING_PIN_NAME    "PF6"  //ender-3:"PA1"

#define HEATER_NAME_MAX             (12)

//加热器的操作命令
//#define HEATER_SET_HEAT_VALID_LEVEL 0x01      //设置加热器加热的有效电频
#define HEATER_HEAT_POWERON         0x02      //加热器加热 (要传电频参数)
#define HEATER_HEAT_POWEROFF        0x04      //加热器不加热 (要传电频参数)

S32 CrHeaterOpen(S8 *Name, S32 Flag, S32 Mode);   //此处Name直接就传加热的管脚， 使用Gpio的操作
S32 CrHeaterClose(S32 Fd);
S32 CrHeaterIoctl(S32 Fd, U32 Cmd, VOID* Arg);

#ifdef TEST_HEATER
VOID TestHeater();
#endif

#ifdef __cplusplus
}
#endif

#endif
