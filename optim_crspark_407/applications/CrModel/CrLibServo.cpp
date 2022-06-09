/**
 * @file CrLibServo.cpp
 * @author Creality
 * @brief 舵机驱动的底层接口源文件
 * @version 0.1
 * @date 2021-10-27
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "../CrInc/CrConfig.h"

#if HAS_SERVOS

#include "CrLibServo.h"

static uint8_t ServoCount = 0;

template<typename T>
static T WithIn(T v, T s, T e)
{
    T Temp;

    if (v < s)
    {
        Temp = s;
    }
    else if(v > e)
    {
        Temp = e;
    }
    else
    {
        Temp = v;
    }
    return Temp;
}

#define ROUND(x)        ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define MAX_OVERFLOW    UINT16_MAX
#define CYC_MSEC        (1000 * 168)                                        // 1ms占用多少时钟周期,这里按主频168M计算
#define TAU_MSEC        20                                                  // 比较器溢出值，ms
#define TAU_USEC        (TAU_MSEC * 1000)                                   // 比较器溢出值，us
#define TAU_CYC         (TAU_MSEC * CYC_MSEC)                               // 比较器溢出值占多少时钟周期
#define SERVO_PRESCALER (TAU_CYC / MAX_OVERFLOW + 1)
#define SERVO_OVERFLOW  ((uint16_t)ROUND((double)TAU_CYC / SERVO_PRESCALER))


// 单位换算
#define US_TO_COMPARE(us)   uint16_t(Map((us), 0, TAU_USEC, 0, SERVO_OVERFLOW))
#define COMPARE_TO_US(c)    uint32_t(Map((c),  0, SERVO_OVERFLOW, 0, TAU_USEC))
#define ANGLE_TO_US(a)      uint16_t(Map((a),  MinAngle, MaxAngle, MinPw, MaxPw))
#define US_TO_ANGLE(us)     int16_t(Map((us), MinPw, MaxPw, minAngle, maxAngle))

// 舵机用到的定时器
typedef void (*ServoTimerCb)(uint8_t);
typedef struct ServoTimer
{
    GPIO_TypeDef *Gpio;
    uint16_t Pin;
    TIM_TypeDef *Timer;
    ServoTimerCb Handler;
    uint8_t SetChannel;             // 拉高IO口用的比较通道
    uint8_t CycleChannel;             // 拉低IO口用的比较通道
} ServoTimerDev;


#define SERVEO_TIMER(num)        \
{                                \
    .Gpio = NULL,                \
    .Pin = 1,                    \
    .Timer = TIM##num,           \
    .Handler = NULL,             \
    .SetChannel = 1,             \
    .CycleChannel = 2              \
}

#if 0
ServoTimerDev ServoTimer1 = SERVEO_TIMER(1);
ServoTimerDev ServoTimer2 = SERVEO_TIMER(2);
ServoTimerDev ServoTimer3 = SERVEO_TIMER(3);
ServoTimerDev ServoTimer4 = SERVEO_TIMER(4);
ServoTimerDev ServoTimer5 = SERVEO_TIMER(5);
ServoTimerDev ServoTimer6 = SERVEO_TIMER(6);
ServoTimerDev ServoTimer7 = SERVEO_TIMER(7);
ServoTimerDev ServoTimer8 = SERVEO_TIMER(8);
ServoTimerDev ServoTimer9 = SERVEO_TIMER(9);
ServoTimerDev ServoTimer10 = SERVEO_TIMER(10);
ServoTimerDev ServoTimer11 = SERVEO_TIMER(11);
ServoTimerDev ServoTimer12 = SERVEO_TIMER(12);
ServoTimerDev ServoTimer13 = SERVEO_TIMER(13);
ServoTimerDev ServoTimer14 = SERVEO_TIMER(14);

const ServoTimerDev ServoTimers[14] = {ServoTimer1, ServoTimer2, ServoTimer3, ServoTimer4, ServoTimer5,  \
                                       ServoTimer6, ServoTimer7, ServoTimer8, ServoTimer9, ServoTimer10, \
                                       ServoTimer11, ServoTimer12, ServoTimer13, ServoTimer14};
#else
const ServoTimerDev ServoTimers[14] = {SERVEO_TIMER(1), SERVEO_TIMER(2), SERVEO_TIMER(3), SERVEO_TIMER(4), SERVEO_TIMER(5),  \
        SERVEO_TIMER(6), SERVEO_TIMER(7), SERVEO_TIMER(8), SERVEO_TIMER(9), SERVEO_TIMER(10), \
        SERVEO_TIMER(11), SERVEO_TIMER(12), SERVEO_TIMER(13), SERVEO_TIMER(14)};
#endif
/**
 * @brief Get the Servo Timer object
 *
 * @param TimerNum 舵机定时器号
 * @return ServoTimerDev* 舵机定时器句柄
 */
static ServoTimerDev *GetServoTimer(uint8_t TimerNum)
{
    if (TimerNum < 1 || TimerNum > 14)
    {
        return NULL;
    }
    else
    {
        return (ServoTimerDev *)&(ServoTimers[TimerNum - 1]);
    }
    return NULL;
}

/**
 * @brief 线性映射
 *
 * @param value 需要映射的数据
 * @param fromStart 原数据域起始位置
 * @param fromEnd 原数据域终止位置
 * @param toStart 映射域起始位置
 * @param toEnd 映射域终止位置
 * @return int32_t
 */
static inline int32_t Map(int32_t value,
                          int32_t fromStart, int32_t fromEnd,
                          int32_t toStart, int32_t toEnd)
{
    return ((int64_t)(value - fromStart) * (toEnd - toStart)) / (fromEnd - fromStart) + toStart;
}

/**
 * @brief 检查定时器和通道配置是否正确
 *
 * @param Dev 舵机定时器句柄
 * @return true 正确
 * @return false 不正确
 */
static bool ServoTimerDevChnnelCheck(ServoTimerDev *Dev)
{
    auto ChannelCheck = [&](ServoTimerDev *Dev, uint8_t Channel)
    {
        switch (Channel)
        {
            case 1:
            {
                return IS_TIM_CC1_INSTANCE(Dev->Timer);
            }
            case 2:
            {
                return IS_TIM_CC2_INSTANCE(Dev->Timer);
            }
            case 3:
            {
                return IS_TIM_CC3_INSTANCE(Dev->Timer);
            }
            case 4:
            {
                return IS_TIM_CC4_INSTANCE(Dev->Timer);
            }
            default:
            {
                return false;
            }
        }
    };

    if (!Dev || !IS_TIM_INSTANCE(Dev->Timer))
    {
        return false;
    }
    else if (!ChannelCheck(Dev, Dev->CycleChannel))
    {
        return false;
    }
    else if (!ChannelCheck(Dev, Dev->SetChannel))
    {
        return false;
    }
    else
    {
        return true;
    }
    return true;
}

/**
 * @brief 设置高电平持续时间比较器的值
 *
 * @param Dev 舵机定时器句柄
 * @param Channel 通道
 * @param Value 配置值
 */
static void CompareSet(ServoTimerDev *Dev, uint8_t Channel, uint16_t Value)
{
    __IO uint32_t *RegCcr = &(Dev->Timer->CCR1) + ((Channel - 1) >> 2);
    *RegCcr = Value;
}
/**
 * @brief 实例化一个舵机驱动
 *
 */
CrLibServo::CrLibServo()
{
    ServoIndex = ServoCount < MAX_SERVOS ? ServoCount++ : INVALID_SERVO;
}

/**
 * @brief 挂载一个舵机驱动
 * 
 * @param Cfg 舵机驱动的配置信息
 * @return true 挂载成功
 * @return false 挂载失败
 */
bool CrLibServo::Attach(CrLibServoCfg Cfg)
{
    ServoTimerDev *Dev = GetServoTimer(Cfg.ServoTimerNo);
    if (!ServoTimerDevChnnelCheck(Dev))
    {
        return false;
    }
    else
    {
        MinAngle = Cfg.MinAng;
        MaxAngle = Cfg.MaxAng;
        MinPw = Cfg.MinDuty;
        MaxPw = Cfg.MaxDuty;
        Angle = -1;
        Gpio = Cfg.Gpiox;
        Pin = Cfg.PinNo;
        ServoTimerNo = Cfg.ServoTimerNo;
        SetChannel = Cfg.SetChannel;
        CycleChannel = Cfg.CycleChannel;
    }
    return SetupSoftPWM();
}

/**
 * @brief 卸载一个舵机驱动
 *
 * @return true 卸载成功
 * @return false 卸载失败
 */
bool CrLibServo::Detach(void)
{
    if (!Attached())
    {
        return false;
    }
    else
    {
        PwmSetDuty(0);
        return true;
    }
}

/**
 * @brief 获取舵机角度
 *
 * @return int32_t
 */
int32_t CrLibServo::GetAngle(void) const
{
  if (Attached())
  {
      return Angle;
  }
  else
  {
  }
  return 0;
}

/**
 * @brief 设置舵机度数
 *
 * @param AngleValue 需要设置的度数，单位°
 * @param SafeDelay 设置后舵机需要一定时间才会到达指定角度，这儿的延时(ms)就是为了这个目的
 */
void CrLibServo::SetAngle(const int32_t AngleValue, uint32_t SafeDelay)
{
  if (Attached())
  {
      Angle = WithIn(AngleValue, MinAngle, MaxAngle);
      // 设置角度
      PwmSetDuty(US_TO_COMPARE(ANGLE_TO_US(Angle)));
      // 等待转动完成
      HAL_Delay(SafeDelay);
      #ifdef DEACTIVATE_SERVOS_AFTER_MOVE
          Detach();
      #endif  // DEACTIVATE_SERVOS_AFTER_MOVE
  }
}

/**
 * @brief 舵机定时器中断回调函数
 *
 * @param ServoTimerNo 舵机定时器号
 */
// TODO:需要根据实际情况将这个回调函数应用到定时器中断函数里面
extern "C" void ServoIRQHandler(uint8_t ServoTimerNo)
{
    ServoTimerDev *Dev = GetServoTimer(ServoTimerNo);
    if (!ServoTimerDevChnnelCheck(Dev))
    {
        return;
    }
    else
    {
        if (Dev->Timer->SR & (1 << (Dev->SetChannel)))
        {
            // 拉低引脚
            Dev->Gpio->BSRR |= ((1 << (Dev->Pin)) << 16);
            // 清除中断标志
            Dev->Timer->SR &= (~(1 << (Dev->SetChannel)));
        }
        else if (Dev->Timer->SR & (1 << (Dev->CycleChannel)))
        {
            // 拉高引脚
            Dev->Gpio->BSRR |= (1 << (Dev->Pin));
            // 清除中断标志
            Dev->Timer->SR &= (~(1 << (Dev->CycleChannel)));
        }
        else
        {
            // 不应该到这儿
            // MARK:模块出现问题可以在这儿加软断点调试
        }
    }
}

/**
 * @brief 配置软PWM，两路比较器实现
 *
 * @return true 配置成功
 * @return false 配置失败
 */
bool CrLibServo::SetupSoftPWM(void)
{
     auto OcModeSet = [&](ServoTimerDev *Dev, uint8_t Channel) -> void
     {
         // 通道1、2配置寄存器为CCMR1，通道3、4配置寄存器为CCMR2，
         __IO uint32_t *RegCcmr = (&(Dev->Timer->CCMR1) + ((Channel - 1) >> 1) * 4);
         // 判断配置数据在寄存器的位置
         uint8_t Shift = 8 * (1 - (Dev->SetChannel & 1));
         // 回读寄存器数据
         uint32_t CcmrData = *RegCcmr;
         // 清空
         CcmrData &= (~(0xFF << Shift));
         // 设置为输出比较，使用冻结模式
         CcmrData |= ((1 << 4) << Shift);
         // 写寄存器
         *RegCcmr = CcmrData;
     };

     ServoTimerDev *Dev = GetServoTimer(ServoTimerNo);
     if (!ServoTimerDevChnnelCheck(Dev))
     {
         return false;
     }
     else
     {
         uint32_t Temp = 0;
         // 配置IO口端口模式：通用输出模式
         Temp = Dev->Gpio->MODER;
         Temp &= (~(3 << (Dev->Pin *2)));
         Temp |= (1 << (Dev->Pin * 2));
         Dev->Gpio->MODER = Temp;
         // 配置IO口端口类型：推挽输出
         Temp = Dev->Gpio->OTYPER;
         Temp &= (~(1 << Dev->Pin));
         Dev->Gpio->OTYPER = Temp;
         // 配置IO口速度：中速25MHz
         Temp = Dev->Gpio->OSPEEDR;
         Temp &= (~(3 << (Dev->Pin *2)));
         Temp |= (1 << (Dev->Pin * 2));
         Dev->Gpio->OSPEEDR = Temp;
         // 配置IO口上下拉：上拉
         Temp = Dev->Gpio->PUPDR;
         Temp &= (~(3 << (Dev->Pin *2)));
         Temp |= (1 << (Dev->Pin * 2));
         Dev->Gpio->PUPDR = Temp;

         Dev->Handler = ServoIRQHandler;
         Dev->Timer->CR1 &= (~(1 << 0));
         // MARK：SetChannel输出使能，可以不要,Marlin上面有配置暂时不知道为什么
         Dev->Timer->CCER |= (1 << (Dev->SetChannel - 1) >> 2);
         // 输出比较模式配置
         OcModeSet(Dev, Dev->SetChannel);
         OcModeSet(Dev, Dev->CycleChannel);
         // 设置预分频器值
         Dev->Timer->PSC = SERVO_PRESCALER - 1;
         // 设置重载值
         Dev->Timer->ARR = SERVO_OVERFLOW;
         // 设置比较值
         CompareSet(Dev, Dev->SetChannel, SERVO_OVERFLOW);
         CompareSet(Dev, Dev->CycleChannel, SERVO_OVERFLOW);
         // 清除中断标志
         Dev->Timer->SR &= (~(1 << Dev->SetChannel));
         Dev->Timer->SR &= (~(1 << Dev->CycleChannel));
         // 开启中断
         Dev->Timer->DIER |= (1 << Dev->SetChannel);
         Dev->Timer->DIER |= (1 << Dev->CycleChannel);
         // MARK:板级初始化的时候应该统一配置NVIC中断控制，实现所有中断优先级的统一管理，如果没有，这儿还需要配置NVIC
         // 更新
         Dev->Timer->EGR |= (1 << 0);
         // 开启定时器
         Dev->Timer->CR1 |= (1 << 0);
     }
     return true;
}

/**
 * @brief 设置高电平持续时间
 *
 * @param DutyCycle 高电平时间需要的时钟数
 */
void CrLibServo::PwmSetDuty(const uint16_t DutyCycle)
{
    ServoTimerDev *Dev = GetServoTimer(ServoTimerNo);
    if (!ServoTimerDevChnnelCheck(Dev))
    {
        return ;
    }
    else
    {
        CompareSet(Dev, Dev->SetChannel, DutyCycle);
        if (DutyCycle)
        {
            Dev->Timer->DIER |= (1 << Dev->SetChannel);
            Dev->Timer->DIER |= (1 << Dev->CycleChannel);
        }
        else
        {
            Angle = -1;
            Dev->Timer->DIER &= (~(1 << Dev->SetChannel));
            Dev->Timer->DIER &= (~(1 << Dev->CycleChannel));
            // 拉低IO口
            Dev->Gpio->BSRR |= ((1 << (Dev->Pin)) << 16);
        }
    }
}

/**
 * @brief 停止软PWM输出
 *
 */
void CrLibServo::PauseSoftPWM()
{
    ServoTimerDev *Dev = GetServoTimer(ServoTimerNo);
    if (!ServoTimerDevChnnelCheck(Dev))
    {
        return ;
    }
    else
    {
        Dev->Timer->CR1 &= (~(1 << 0));
        PwmSetDuty(0);
    }
}

#endif // HAS_SERVOS
