#include "../CrInc/CrConfig.h"

#if HAS_SERVOS

#include "CrServo.h"

#define SERVO_DEFAULT_MIN_PW            544     // 舵机运转最小角度需要的高电平时间
#define SERVO_DEFAULT_MAX_PW            2400    // 舵机运转最大角度需要的高电平时间
#define SERVO_DEFAULT_MIN_ANGLE         0       // 舵机能运转的最小角度
#define SERVO_DEFAULT_MAX_ANGLE         180     // 舵机能运转的最大角度

CrLibServo Servos[NUM_SERVOS];
CrLibServoCfg ServoCfg[NUM_SERVOS] =
{
    {
        .ServoTimerNo = 1,
        .SetChannel = 1,
        .CycleChannel = 2,
        .Gpiox = GPIOA,
        .PinNo = 1,
        .MinAng = SERVO_DEFAULT_MIN_ANGLE,
        .MaxAng = SERVO_DEFAULT_MAX_ANGLE,
        .MinDuty = SERVO_DEFAULT_MIN_PW,
        .MaxDuty = SERVO_DEFAULT_MAX_PW
    },
};

#ifdef EDITABLE_SERVO_ANGLES
  uint16_t servo_angles[NUM_SERVOS][2];
#endif  // EDITABLE_SERVO_ANGLES

/**
 * @brief 初始化舵机
 * 
 */
void CrServoInit(void)
{
    int i = 0;
    for (i = 0; i < NUM_SERVOS; i++)
    {
        Servos[i].Attach(ServoCfg[i]);
    }
}

/**
 * @brief 打开舵机
 * 
 * @param Node Linux下运行的时候的设备节点
 * @param File Linux下运行的时候的文件指针
 * @return int 无意义
 */
int CrServoOpen(void *Node, void *File)
{
    Node = Node;
    File = File;
    CrServoInit();
    return 0;
}

/**
 * @brief 关闭舵机
 * 
 * @param Filedes Linux下运行的时候的文件句柄
 * @return int 无意义
 */
int CrServoClose(int Filedes)
{
    int i = 0;
    for (i = 0; i < NUM_SERVOS; i++)
    {
        Servos[i].Detach();
    }
    return 0;
}

//int CrServoRead(void *Fp, void *UserBuff, int Size, void *Lofft)
//{
//    return 0;
//}
//
//int CrServoWrite(void *Fp, void *UserBuff, int Size, void *Lofft)
//{
//    return 0;
//}

#endif  // #if HAS_SERVOS
