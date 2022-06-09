#ifndef _GCODEM907HANDLE_H_
#define _GCODEM907HANDLE_H_

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DAC_STEPPER_CURRENT 1
#define HAS_I2C_DIGIPOT 0
#define HAS_DIGIPOTSS 0
#define HAS_MOTOR_CURRENT_PWM 0
#define MOTOR_CURRENT_PWM_X_PIN 10
#define MOTOR_CURRENT_PWM_Y_PIN 20
#define MOTOR_CURRENT_PWM_Z_PIN 30
#define MOTOR_CURRENT_PWM_E_PIN 40


S32 GcodeM907Parse(const S8 *CmdBuff, VOID *Arg);     //M907命令的解析函数
S32 GcodeM907Exec(VOID *ArgPtr, VOID *ReplyResult);   //M907命令的执行函数
S32 GcodeM907Reply(VOID *ReplyResult, S8 *Buff);      //M907命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM907HANDLE_H_ */
