#ifndef _GCODEM303HANDLE_H
#define _GCODEM303HANDLE_H

#include <stdio.h>
#include "CrInc/CrType.h"

//#define PID_DEBUG             // Sends debug data to the serial port. Use 'M303 D' to toggle activation.

#define BUSY_WHILE_HEATING            // Some hosts require "busy" messages even during heating

#define STR_PID_BAD_EXTRUDER_NUM            "PID Autotune failed! Bad extruder number"
#define MSG_PID_AUTOTUNE                    "PID Autotune"
#define STR_PID_TEMP_TOO_HIGH               "PID Autotune failed! Temperature too high"
#define STR_PID_AUTOTUNE_START              "PID Autotune start"
#define STR_STOPPED_HEATER                  ", system stopped! Heater_ID: "
#define STR_HEATER_BED                      "bed"
#define STR_HEATER_CHAMBER                  "chamber"

// Preheat Constants
#define PREHEAT_1_LABEL       "PLA"
#define PREHEAT_1_TEMP_HOTEND 180
#define PREHEAT_1_TEMP_BED     70
#define PREHEAT_1_FAN_SPEED     0 // Value from 0 to 255

//热床的监视参数
#define THERMAL_PROTECTION_BED_PERIOD        20 // Seconds
#define THERMAL_PROTECTION_BED_HYSTERESIS     2 // Degrees Celsius
/*** As described above, except for the bed (M140/M190/M303). */
#define WATCH_BED_TEMP_PERIOD                60 // Seconds
#define WATCH_BED_TEMP_INCREASE               2 // Degrees Celsius

//碰头的监视参数
#define THERMAL_PROTECTION_PERIOD 40        // Seconds
#define THERMAL_PROTECTION_HYSTERESIS 4     // Degrees Celsius
/*** Whenever an M104, M109, or M303 increases the target temperature, the
* firmware will wait for the WATCH_TEMP_PERIOD to expire. If the temperature
* hasn't increased by WATCH_TEMP_INCREASE degrees, the machine is halted and
* requires a hard reset. This test restarts with any M104/M109/M303, but only
* if the current temperature is far enough below the target for a reliable
* test.
* If you get false positives for "Heating failed", increase WATCH_TEMP_PERIOD
* and/or decrease WATCH_TEMP_INCREASE. WATCH_TEMP_INCREASE should not be set
* below 2.
*/
#define WATCH_TEMP_PERIOD 20                // Seconds
#define WATCH_TEMP_INCREASE 2               // Degrees Celsius


//M109相关的内容
#define TEMP_RESIDENCY_TIME     10  // (seconds) Time to wait for hotend to "settle" in M109（喷头在M109命令中等待温度稳定的时间）
#define TEMP_WINDOW              1  // (°C) Temperature proximity for the "temperature reached" timer  “达到温度”计时器的温度接近度
#define TEMP_HYSTERESIS          3  // (°C) Temperature proximity considered "close enough" to the target   温度接近度被认为与目标“足够接近”

//M190相关的内容
#define TEMP_BED_RESIDENCY_TIME 10  // (seconds) Time to wait for bed to "settle" in M190   （喷头在M190命令中等待温度稳定的时间）
#define TEMP_BED_WINDOW          1  // (°C) Temperature proximity for the "temperature reached" timer
#define TEMP_BED_HYSTERESIS      3  // (°C) Temperature proximity considered "close enough" to the target

// Below this temperature the heater will be switched off  低于此温度，加热器将关闭
// because it probably indicates a broken thermistor wire.  因为它可能表明热敏电阻导线损坏
#define HEATER_0_MINTEMP   5    //喷头的最小温度
#define HEATER_1_MINTEMP   5
#define HEATER_2_MINTEMP   5
#define HEATER_3_MINTEMP   5
#define HEATER_4_MINTEMP   5
#define HEATER_5_MINTEMP   5
#define HEATER_6_MINTEMP   5
#define HEATER_7_MINTEMP   5
#define BED_MINTEMP        5    //热床的最小温度

// Above this temperature the heater will be switched off.  高于此温度，加热器将关闭
// This can protect components from overheating, but NOT from shorts and failures.  这样可以防止组件过热，但不能防止短路和故障。
// (Use MINTEMP for thermistor short/failure protection.)
#define HEATER_0_MAXTEMP 275    //喷头的最大温度
#define HEATER_1_MAXTEMP 275
#define HEATER_2_MAXTEMP 275
#define HEATER_3_MAXTEMP 275
#define HEATER_4_MAXTEMP 275
#define HEATER_5_MAXTEMP 275
#define HEATER_6_MAXTEMP 275
#define HEATER_7_MAXTEMP 275
#define BED_MAXTEMP      150    //热床的最大温度

//计算max target
#ifndef BED_OVERSHOOT
    #define BED_OVERSHOOT 10
#endif
#define BED_MAX_TARGET (BED_MAXTEMP - (BED_OVERSHOOT))

#ifndef HOTEND_OVERSHOOT
    #define HOTEND_OVERSHOOT 15
#endif

//最大功率（占空比）
#define MAX_BED_POWER 255 // limits duty cycle to bed; 255=full current
#define BANG_MAX 255     // Limits current to nozzle while in bang-bang mode; 255=full current
#define PID_MAX BANG_MAX // Limits current to nozzle while PID is active (see PID_FUNCTIONAL_RANGE below); 255=full current

//喷头或加热室的风扇
#define HAS_AUTO_FAN 0


#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM303Parse(const S8 *CmdBuff, VOID *Arg);
S32 GcodeM303Exec(VOID *ArgPtr, VOID *ReplyResult);
S32 GcodeM303Reply(VOID *ReplyResult, S8 *Buff);

#ifdef __cplusplus
}
#endif

#endif
