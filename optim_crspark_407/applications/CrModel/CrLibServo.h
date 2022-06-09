/**
 * @file CrLibServo .h
 * @author Creality
 * @brief 舵机驱动的底层接口头文件
 * @version 0.1
 * @date 2021-10-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __CRLIBSERVO_H__
#define __CRLIBSERVO_H__

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"

#define DAFAULT_SERVO_SAFE_DELAY        50

#define NOT_ATTACHED                    (-1)
#define INVALID_SERVO                   255

#ifndef MAX_SERVOS
  #define MAX_SERVOS                    3
#endif

#define HAL_SERVO_LIB CrLibServo

typedef struct _CrLibServoCfg
{
    uint8_t ServoTimerNo;                // 舵机使用到的定时器号
    uint8_t SetChannel;                  // 控制高电平使用的通道
    uint8_t CycleChannel;                // 控制周期使用的通道
    GPIO_TypeDef *Gpiox;                 // 控制舵机的Gpio
    int32_t PinNo;                       // 控制舵机的脚位
    int32_t MinAng;                      // 舵机运行的最小角度
    int32_t MaxAng;                      // 舵机运行的最大角度
    int32_t MinDuty;                     // 舵机运行最小角度需要的高电平时间
    int32_t MaxDuty;                     // 舵机运行最大角度需要的高电平时间
}CrLibServoCfg;

class CrLibServo
{
    public:
        CrLibServo();
        bool Attach(CrLibServoCfg Cfg);
        bool Attached(void) const
        {
            return Pin != NOT_ATTACHED;
        }
        bool Detach(void);
        void SetAngle(const int32_t AngleValue, uint32_t SafeDelay = DAFAULT_SERVO_SAFE_DELAY);
        int32_t GetAngle(void) const;

        int32_t Read(void)
        {
            return GetAngle();
        }
        void Write(const int32_t AngleValue)
        {
            SetAngle(AngleValue);
        }
        void Move(const int32_t AngleValue)
        {
            SetAngle(AngleValue);
        }
    private:
        void ServoWrite(uint16_t DutyCycle);

        uint8_t ServoIndex;               // index into the channel data for this servo
        GPIO_TypeDef *Gpio = NULL;
        int32_t Pin = NOT_ATTACHED;
        int32_t MinAngle;
        int32_t MaxAngle;
        int32_t MinPw;
        int32_t MaxPw;
        int32_t Angle;
        uint8_t ServoTimerNo;
        uint8_t SetChannel;
        uint8_t CycleChannel;
        bool SetupSoftPWM();
        void PauseSoftPWM();
        void PwmSetDuty(const uint16_t DutyCycle);
};

#endif  // __CRLIBSERVO_H__
