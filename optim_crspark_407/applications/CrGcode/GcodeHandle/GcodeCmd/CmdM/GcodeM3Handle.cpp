#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "GcodeM3Handle.h"
#include "CrInc/CrMsgType.h"
#include "CrInc/CrType.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrMotion/CrMotionPlanner.h"
#include "CrModel/CrPrinter.h"
#include "Common/Include/CrSleep.h"
#include "CrGpio/CrGpio.h"


#define CMD_M3 "M3"
/*******额外定义的变量和函数********/
U16 CutterCpwrToUpwr(U16 Cpwr)
{
    return Cpwr * 255 / 100;
}

U16 GetPowerS(VOID *ArgPtr)
{
    struct ArgM3_t *Param = (struct ArgM3_t *)ArgPtr;
    if ( Param->HasS )
    {
        FLOAT Spwr = Param->S;
#ifdef SPINDLE_SERVO
        CutterUnitPower = Spwr;
#else
#ifdef SPINDLE_LASER_PWM
        CutterUnitPower = CutterPowerToRange();
#else
        CutterUnitPower = Spwr > 0 ? 255 : 0;
#endif
#endif
    }
    else
    {
        CutterUnitPower = CutterCpwrToUpwr(SPEED_POWER_STARTUP);
    }

    return CutterUnitPower;
}

VOID CutterInlineDirection()
{

}

U8 PctToPwm(FLOAT Pct)
{
    return Pct * 255 / 100;
}

U16 CutterPowerToRange(const U16 Pwr,const U8 PwrUnit)
{
    if ( Pwr <= 0 )
    {
        return 0;
    }
    U16 Upwr = 0;
    switch(PwrUnit)
    {
        case 0:
        {
            if ( Pwr < PctToPwm(MinPct) )
            {
                Upwr = PctToPwm(MinPct);
            }
            else if ( Pwr > PctToPwm(MaxPct) )
            {
                Upwr = PctToPwm(MaxPct);
            }
            else
            {
                Upwr = Pwr;
            }
            break;
        }
        case 1:
        {
            Upwr = (Pwr < MinPct) ? MinPct : (Pwr > MaxPct) ? MaxPct : Pwr;
            break;
        }
        case 2:
        {
            Upwr = (Pwr < SPEED_POWER_MIN ) ? SPEED_POWER_MIN : (Pwr > SPEED_POWER_MAX) ? SPEED_POWER_MAX : Pwr;
            break;
        }
        default:
            break;
    }
    return Upwr;
}

VOID CutterInlineOcrPower(U8 Ocrpwr)
{
    IsReady = Ocrpwr > 0;
    StatePower.IsEnabled = Ocrpwr > 0;
    StatePower.power = Ocrpwr;
}

U16 UpowerToOcr(U16 Upwr)
{
    //分成三种情况：PWM、百分比、RPM,因暂时未使用定义到，目前只考虑百分比情况
    return PctToPwm(Upwr);
}

VOID CutterInlinePower(const U16 Upwr)
{
    UnitPower = MenuPower = Upwr;
    StatePower.IsEnabled = Upwr > 0;
    StatePower.power = Upwr;
    IsReady = Upwr > 0;
}
VOID CutterSetInlineEnabled(const BOOL Enable)
{
    if ( Enable )
    {
        CutterInlinePower(255);
    }
    else
    {
        IsReady = false;
        UnitPower = MenuPower = 0;
        StatePower.IsPlanned = 0;
#ifdef SPINDLE_LASER_PWM
        CutterInlineOcrPower(0);
#else
        CutterInlinePower(0);
#endif
    }
}

VOID CutterInlineDisable()
{
    IsReady = false;
    UnitPower = 0;
    StatePower.IsEnabled = false;
    StatePower.IsPlanned = false;
    StatePower.power = 0;
}

VOID PlannerSynchronize()
{
    if ( !Printer )
    {
        return;
    }
    //当打印机处于运动状态下不执行，直到打印停止
    while ( Printer->GetPrintStatus() == PRINTING || Printer->GetPrintStatus() == RESUME_PRINT )
    {
        CrU_Sleep(5);
    }
}
VOID AnalogWrite(S16 PIN,S32 PwmValue)
{
    //对指定的PIN脚写入PWM值
}

VOID SetOcrState(const U8 Ocr)
{
#if NEEDS_HARDWARE_PWM && SPINDLE_LASER_FREQUENCY
    SetPwmFrequency((SPINDLE_LASER_PWM_PIN),SPINDLE_LASER_FREQUENCY);      //设置PWM占空比
    SetPwmDuty((SPINDLE_LASER_PWM_PIN), Ocr ^ SPINDLE_LASER_PWM_OFF);
#else
    AnalogWrite(SPINDLE_LASER_PWM_PIN, (Ocr ^ SPINDLE_LASER_PWM_OFF));          //设置PWM占空比
#endif
}

VOID SetOcr(const U8 Ocr)
{
    S32 Fd;
    struct PinParam_t State = {(GPIO_PinState)SPINDLE_LASER_ACTIVE_STATE};
    Fd = CrGpioOpen(SPINDLE_LASER_ENA_PIN, 0, 0);
    CrGpioIoctl(SPINDLE_LASER_ENA_PIN, GPIO_SET_PINSTATE, &State);
    SetOcrState(Ocr);
}

VOID CutterSetOcrPower(const U8 Ocr)
{
    Power = Ocr;
    SetOcr(Ocr);
}

VOID CutterSetReverse(const BOOL Reverse)
{
    BOOL DirState = (Reverse == SPINDLE_INVERT_DIR);
    S32 Fd;
    struct PinParam_t State = {(GPIO_PinState)DirState};
    Fd = CrGpioOpen(SPINDLE_DIR_PIN, 0, 0);
    CrGpioIoctl(SPINDLE_DIR_PIN, GPIO_SET_PINSTATE, &State);
}

VOID CutterSetPower(const U8 Upwr)
{
    Power = Upwr;
}

VOID CutterSetEnabled(const BOOL Enable)
{
    CutterSetPower(Enable ? 255 : 0);
}
/*******额外定义的变量和函数********/


S32 GcodeM3Parse(const S8 *CmdBuff, VOID *Arg)
{
    //1、判空
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    //2、判断Gcode命令长度是否大于1且是否为M命令
    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || CmdBuff[0] != GCODE_CMD_M )
    {
        return 0;
    }

    //3、获取命令
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )   //防止获取命令失败
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_M3) != 0 )    //判断是否是M3命令
    {
        return 0;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M3) + sizeof(S8);
    struct ArgM3_t *CmdArg = (struct ArgM3_t *)Arg;

    S32 OptLen = 0;
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;
        }

        OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M3 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_I:
            {
                CmdArg->HasI = true;
                break;
            }
            case OPT_O:
            {
                CmdArg->HasO = true;
                if ( OptLen > 1 )
                {
                    CmdArg->O = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->O = 0;
                }
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                {
                    CmdArg->O = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->HasS = false;
                }
            }
            default:
                break;
        }
        Offset += strlen(Opt) + sizeof(S8);
    }
    return 1;
}

S32 GcodeM3Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM3_t *Param = (struct ArgM3_t *)ArgPtr;
    BOOL Flag = 0;
#ifdef LASER_POWER_INLINE
#ifdef LASER_POWER_INLINE_INVERT
    Flag = 0;
#else
    Flag = 1;
#endif
    if ( Param->HasI == Flag )
    {
        CutterInlineDirection(false);
#ifdef SPINDLE_LASER_PWM
        if ( Param->HasO )
        {
            CutterUnitPower = CutterPowerToRange(Param->O,0);
            CutterInlineOcrPower(CutterUnitPower);
        }
        else
        {
            CutterInlinePower(UpowerToOcr(GetPowerS()));
        }
#else
        CutterSetInlineEnabled();

#endif
        return 0;
    }
    CutterInlineDisable();
#endif

    //改变电源前等待所有的运动命令执行完
    PlannerSynchronize();

    //
    CutterSetReverse(false);

#ifdef SPINDLE_LASER_PWM
    if ( Param->HasO )
    {
        UnitPower = CutterPowerToRange(Param->O, 0);
        CutterSetOcrPower(UnitPower);
    }
    else
    {
        CutterSetPower(UpowerToOcr(GetPowerS(Param)));
    }
#elif SPINDLE_SERVO
    CutterSetPower(GetPowerS(Param));
#else
    CutterSetEnabled(true);
#endif
    MenuPower = UnitPower;
    return 1;
}

S32 GcodeM3Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
