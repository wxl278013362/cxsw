#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "GcodeM80Handle.h"
#include "Common/Include/CrTime.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGpio/CrGpio.h"
#include "Common/Include/CrSleep.h"


#define CMD_M80 "M80"

/*****额外定义的变量和函数头*******/
BOOL PowerSupplyOn = false;     //由marlin中是否开启PSU_CONTROL宏进行赋值
//#define HAS_SUICIDE
//#define AUTO_POWER_CONTROL
#define PSU_POWERUP_DELAY 250
//#define PSU_POWERUP_GCODE  "M355 S1"
VOID RestoreStepperDrivers()
{
    //函数调用电机的库函数重置每个电机的驱动
}

VOID PowerOn()
{
    CrTimeSpec_t Now = {0};
    CrGetCurrentSystemTime(&Now);
    if ( !PowerSupplyOn )
    {
        //Marlin关于PowerOn程序的流程
        //1、将电源对应的引脚拉低
        S32 Fd = CrGpioOpen(PS_ON_PIN,0,0);
        if ( Fd < 0 )   //打开失败
        {
            return;
        }

        //设置引脚模式
        struct PinInitParam_t Pin = { 0 };
        Pin.Mode = GPIO_MODE_OUTPUT_PP;
        Pin.Pull = GPIO_PULLDOWN;
        Pin.Speed = GPIO_SPEED_FREQ_HIGH;

        CrGpioIoctl(Fd, GPIO_INIT, (VOID *)&Pin);

        //写入
        struct PinParam_t Status = { (GPIO_PinState)false };
        CrGpioWrite(Fd, (S8 *)&Status, sizeof(Status));

        //关闭引脚
        CrGpioClose(Fd);

        //2、delay 250ms
        CrU_Sleep(PSU_POWERUP_DELAY);

        //3、还原电机的驱动程序
        RestoreStepperDrivers();

        //4、如果已经有HAS_TRINAMIC_CONFIG的宏，则再次delay250ms
#ifdef HAS_TRINAMIC_CONFIG
        CrU_Sleep(PSU_POWERUP_DELAY);
#endif
        //5、运行电源重启后的Gcode命令
#ifdef PSU_POWERUP_GCODE
        S8 Replay[20] = {0};
        GcodeHandle(PSU_POWERUP_GCODE, strlen(PSU_POWERUP_GCODE), Replay);
#endif

    }
}
/*****额外定义的变量和函数头*******/


S32 GcodeM80Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || CmdBuff[0] != GCODE_CMD_M ) //Gcode命令长度需要大于1且是M命令
    {
        return 0;
    }

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_M80) != 0 )
    {
        return 0;
    }

    S32 Offset = strlen(CMD_M80) + sizeof(S8);
    struct ArgM80_t *CmdArg = (struct ArgM80_t *)Arg;
    S32 OptLen = 0;
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )    //获取参数失败
        {
            break;
        }

        OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M80 has no param opt = %s\n",CmdBuff);
        }
        switch ( Opt[0] )
        {
            case OPT_S:
            {
                CmdArg->HasS = true;
                break;
            }
            default:
                break;
        }
        Offset += strlen(Opt) + sizeof(S8);
    }
    return 1;
}

S32 GcodeM80Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }
    struct ArgM80_t *Param = (struct ArgM80_t *)ArgPtr;
    //根据是否有S来输出提示信息
    if ( Param->HasS )
    {
        if ( PowerSupplyOn )
            printf("PS:1\n");
        else
            printf("PS:0\n");
        return 1;
    }
    //电源打开函数
    PowerOn();

    //如果定义了重启的宏,则重新定义GPIO的输出电平
    #ifdef HAS_SUICIDE
            S32 Fd = CrGpioOpen(SUICIDE_PIN,0,0);
            if ( Fd < 0 )   //打开失败
            {
                return;
            }

            //设置引脚模式
            struct PinInitParam_t Pin = { 0 };
            Pin.Mode = GPIO_MODE_OUTPUT_PP;
            Pin.Pull = GPIO_NOPULL;
            Pin.Speed = GPIO_SPEED_FREQ_HIGH;

            CrGpioIoctl(Fd, GPIO_INIT, (VOID *)&Pin);

            //写入
            struct PinParam_t Status = { (GPIO_PinState)false };
            CrGpioWrite(Fd, (S8 *)&Status, sizeof(Status));

            //关闭引脚
            CrGpioClose(Fd);
    #endif

    //如果没有定义了电源控制的宏，则重新恢复电机的驱动程序并且delay一段时间(未实现)
    #ifndef AUDIO_STREAM_RECORD
            CrU_Sleep(PSU_POWERUP_DELAY);
            RestoreStepperDrivers();
        #ifdef HAS_TRINAMIC_CONFIG
            CrU_Sleep(PSU_POWERUP_DELAY);
        #endif
    #endif

    //最后刷新屏幕上的状态栏
    return 1;

}

S32 GcodeM80Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
