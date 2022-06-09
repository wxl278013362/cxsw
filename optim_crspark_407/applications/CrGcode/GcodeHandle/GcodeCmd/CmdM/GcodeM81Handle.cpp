#include "GcodeM81Handle.h"
#include "GcodeM80Handle.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Common/Include/CrTime.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrModel/CrHeaterManager.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrFans.h"
#include "CrMotion/CrMotionPlanner.h"
#include "Common/Include/CrSleep.h"
#include "CrGpio/CrGpio.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"
#include "CrModel/CrPrinter.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrInc/CrConfig.h"


#define CMD_M81 "M81"
/*****增加的函数及变量*******/
extern BOOL PowerSupplyOn;
//#define SUICIDE_PIN      //启用时需要定义SUICIDE_PIN的引脚
VOID ProcessSubcommandsNow(U8 PGcode)
{

}
VOID PowerOff()
{
    if ( PowerSupplyOn )
    {
#if PSU_POWEROFF_GCODE
        //如果支持电源管理函数，运行一系列命令，绕过命令队列，允许从其他G代码处理程序中调用G代码“宏”。
        ProcessSubcommandsNow();
#endif
#if PS_OFF_SOUND
        //蜂鸣器响一秒
        CrBeepAppFlexsibleBeeps(1000);
#endif
        //关闭电源
        S32 Fd = CrGpioOpen(PS_ON_PIN,0,0);
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

        //关闭电源后将PowerSupplyOn赋值为false
        PowerSupplyOn = false;
    }

}
#if HAS_SUICIDE
VOID Suicide()
{
    //打开引脚
    S32 Fd = CrGpioOpen(SUICIDE_PIN,0,0);
    if ( Fd < 0 )   //打开失败
    {
        return;
    }

    //设置输出模式
    struct PinInitParam_t Pin = { 0 };
    Pin.Mode = GPIO_MODE_OUTPUT_PP;
    Pin.Pull = GPIO_NOPULL;
    Pin.Speed = GPIO_SPEED_FREQ_HIGH;

    CrGpioIoctl(Fd, GPIO_INIT, (VOID *)&Pin);

    //写入
    struct PinParam_t Status = {false};
    CrGpioWrite(Fd, (S8 *)&Status, sizeof(Status));

}
#endif
/*****增加的函数及变量*******/


S32 GcodeM81Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || CmdBuff[0] != GCODE_CMD_M )    //Gcode命令是M命令
    {
        return 0;
    }

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
    {
        return 0;
    }

    if ( strcmp(Cmd, CMD_M81) != 0 )
    {
        return 0;
    }

    return 1;
}

S32 GcodeM81Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    //进入函数判空
    if ( !ArgPtr || !ReplyResult || !BedHeater || !HotEndArr || !ProbeHeater || !Printer )
    {
        return 0;
    }

    //1、关闭所有加热器，将目标温度设置为0
    struct HeaterManagerAttr_t Attr = {0};
    Attr = BedHeater->CrHeaterMagagerGetAttr();
    Attr.TargetTemper = 0;
    BedHeater->CrHeaterMagagerSetAttr(Attr);
    BedHeater->CrHeaterManagerStop();

    for ( int i = 0; i < HOTENDS; i++ )
    {
        Attr = HotEndArr[i]->CrHeaterMagagerGetAttr();
        Attr.TargetTemper = 0;
        HotEndArr[i]->CrHeaterMagagerSetAttr(Attr);
        HotEndArr[i]->CrHeaterManagerStop();
    }

    Attr = ProbeHeater->CrHeaterMagagerGetAttr();
    Attr.TargetTemper = 0;
    ProbeHeater->CrHeaterMagagerSetAttr(Attr);
    ProbeHeater->CrHeaterManagerStop();

    //2、停止打印，失能所有电机
    Printer->StopHandle();
    MotionModel->DisableAllSteppers();

    //3、打印计时停止
    //4、有风扇则设置风扇转速为0
    if ( !PwmFan[0] )
    {
        return 0;
    }
    struct PwmControllerAttr_t FanAttr = {0};
    for ( int i = 0; i < PWM_FAN_COUNT; i++ )
    {
        FanAttr = PwmFan[i]->GetFanAttr();
        FanAttr.Duty = 0;
        PwmFan[i]->SetFanAttr(FanAttr);
    }

    //5、在关闭电源前delay1秒钟
    CrU_Sleep(1000);

    //6、调用Suicide函数
#if HAS_SUICIDE
    Suicide();
#elif PSU_CONTROL
    //7、调用电源关闭函数
    PowerOff();
#endif

    //8、屏幕显示提示信息
    CrMsg_t LineMsgQueue = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));
    struct MsgLine_t LineMsg = {0};
    LineMsg.MsgType = SPECIAL_MSG;
    LineMsg.Finished = 1;
    strcpy(LineMsg.Info,MACHINE_NAME);
    strcat(LineMsg.Info," OFF.");

    if ( LineMsgQueue )
    {
        //参数1是优先级
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
    }

    return 1;
}

S32 GcodeM81Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
