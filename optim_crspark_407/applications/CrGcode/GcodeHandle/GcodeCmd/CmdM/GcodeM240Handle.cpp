#include "GcodeM240Handle.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrMsgQueue/sharedQueue/CrSharedMsgQueue.h"
#include "CrInc/CrMsgType.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrModel/CrHeaterManager.h"
#include "CrGpio/CrGpio.h"
#include "Common/Include/CrSleep.h"
#include "Common/Include/CrTime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define CMD_M240 "M240"

/*********额外定义的变量和函数************/
#define PHOTO_RETRACT_MM                6.5
#define PAUSE_PARK_RETRACT_FEEDRATE     60
#define RETRACT_FEEDRATE                45
#define PHOTO_POSITION                  { X_MAX_POS - 5, Y_MAX_POS, 0 }
#define PHOTO_SWITCH_POSITION           { X_MAX_POS, Y_MAX_POS }
#define PHOTO_SWITCH_MS                 50
#define PHOTO_DELAY_MS                  100
#define PIN_EXISTS(PN)                  (defined(PN##_PIN) && PN##_PIN >= 0)
#define CHDK                            NULL    //对应的PIN脚
#define PHOTOGRAPH_PIN                  NULL
//#define PHOTO_PULSE_DELAY_US            13

VOID SetPhotoPin(U8 State)
{
    U32 PulseLength = (
#ifdef PHOTO_PULSES_US
            PHOTO_PULSE_DELAY_US
#else
            15
#endif
            );
    //获取Fd
    S32 Fd = CrGpioOpen(PHOTOGRAPH_PIN,0,0);

    //设置引脚模式
    struct PinInitParam_t Pin = { 0 };
    Pin.Mode = GPIO_MODE_OUTPUT_PP;
    Pin.Pull = GPIO_NOPULL;
    Pin.Speed = GPIO_SPEED_FREQ_HIGH;

    CrGpioIoctl(Fd, GPIO_INIT, (VOID *)&Pin);

    //写入
    struct PinParam_t Status = { (GPIO_PinState)State };
    CrGpioWrite(Fd, (S8 *)&Status, sizeof(Status));

    CrU_Sleep(PulseLength);

    //关闭
    CrGpioClose(Fd);
}

VOID  LimitPosBySoftEndstop(XYZFloat_t Target)
{
    //获取系统软限位开关位置，并在归零情况下将Target的值限制在软限位的坐标范围内
}

VOID PulsePhotoPin(const uint32_t Duration, const uint8_t State)
{
    if ( State )
    {
        CrTimeSpec_t *Time;
        CrGetCurrentSystemTime(Time);
        for ( const U32 Stop = (Time->tv_sec * 1000) + Duration; (Time->tv_sec * 1000) < Stop; )
        {
            CrGetCurrentSystemTime(Time);
            SetPhotoPin(true);
            SetPhotoPin(false);
        }
    }
    else
    {
        CrU_Sleep(Duration);
    }
}

VOID SpinPhotoPin()
{
#ifdef PHOTO_PULSES_US
    U32 Sequence[] = PHOTO_PULSES_US;
    for ( int i = 0; i < sizeof(Sequence)/sizeof(U32); i++ )
    {
        PulsePhotoPin(Sequence[i],!(i & 1));
    }
#else
    for ( U8 i = 16; i > 0; i--)    //与marlin不同
    {
        SetPhotoPin(true);
        SetPhotoPin(false);
    }
#endif

}
//让电机运动到指定位置
VOID Unscaled_E_Move(FLOAT Length, FLOAT Fr_mm_s)
{
    struct MotionGcode_t CurPosition = {0};
    struct MotionPositionMsg_t Pos = {0};
    Pos = MotionModel->GetCurPosition();
    CurPosition.Coord.CoorX = Pos.PosX;
    CurPosition.Coord.CoorY = Pos.PosY;
    CurPosition.Coord.CoorZ = Pos.PosZ;
    CurPosition.Coord.CoorE += (Length / Pos.PosE);   //Marlin的e_factor默认值为1
    MotionModel->PutMotionCoord(CurPosition);
}

VOID E_Move_M240(FLOAT Length,FLOAT Fr_mm_s)
{
    if ( !HotEndArr[0] )
    {
        return ;
    }
    if ( Length && (HotEndArr[0]->CrHeaterManagerGetTemper() > HotEndArr[0]->GetExtrudeMinTemper()) )
    {
        Unscaled_E_Move(Length,Fr_mm_s);
    }
}
/*********额外定义的变量和函数************/

S32 GcodeM240Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd,CMD_M240) != 0 )    //判断是否是M240命令
    {
        return 0;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M240) + sizeof(S8);
    struct ArgM240_t *CmdArg = (struct ArgM240_t *)Arg;

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
            printf("Gcode M240 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_A:
            {
                CmdArg->HasA = true;
                if ( OptLen > 1 )
                {
                    CmdArg->A = atoff(Opt + 1);
                    CmdArg->HasValueA = true;
                }
                else
                {
                    CmdArg->HasValueA = false;
                }
                break;
            }
            case OPT_B:
            {
                CmdArg->HasB = true;
                if ( OptLen > 1 )
                {
                    CmdArg->B = atoff(Opt + 1);
                    CmdArg->HasValueB = true;
                }
                else
                {
                    CmdArg->HasValueB = false;
                }
                break;            }
            case OPT_F:
            {
                CmdArg->HasF = true;
                if ( OptLen > 1 )
                {
                    CmdArg->F = atoff(Opt + 1);
                    CmdArg->HasValueF = true;
                }
                else
                {
                    CmdArg->HasValueF = false;
                }
                break;
            }
            case OPT_R:
            {
                CmdArg->HasR = true;
                if ( OptLen > 1 )
                {
                    CmdArg->R = atoff(Opt + 1);
                    CmdArg->HasValueR = true;

                }
                else
                {
                    CmdArg->HasValueR = false;
                }
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                {
                    CmdArg->S = atoff(Opt + 1);
                    CmdArg->HasValueS = true;
                }
                else
                {
                    CmdArg->HasValueS = false;
                }
                break;
            }
            case OPT_X:
            {
                CmdArg->HasX = true;
                if ( OptLen > 1 )
                {
                    CmdArg->X = atoff(Opt + 1);
                    CmdArg->HasValueX = true;
                }
                else
                {
                    CmdArg->HasValueX = false;
                }
                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Y = atoff(Opt + 1);
                    CmdArg->HasValueY = true;
                }
                else
                {
                    CmdArg->HasValueY = false;
                }
                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Z = atoff(Opt + 1);
                    CmdArg->HasValueZ = true;
                }
                else
                {
                    CmdArg->HasValueZ = false;
                }
                break;
            }
            case OPT_D:
            {
                CmdArg->HasD = true;
                if ( OptLen > 1 )
                {
                    CmdArg->D = atoi(Opt + 1);
                    CmdArg->HasValueD = true;
                }
                else
                {
                    CmdArg->HasValueD = false;
                }
                break;
            }
            case OPT_P:
            {
                CmdArg->HasP = true;
                if ( OptLen > 1 )
                {
                    CmdArg->P = atoi(Opt + 1);
                    CmdArg->HasValueP = true;
                }
                else
                {
                    CmdArg->HasValueP = false;
                }
                break;
            }
            case OPT_I:
            {
                CmdArg->HasI = true;
                if ( OptLen > 1 )
                {
                    CmdArg->I = atoff(Opt + 1);
                    CmdArg->HasValueI = true;
                }
                else
                {
                    CmdArg->HasValueI = false;
                }
                break;
            }
            case OPT_J:
            {
                CmdArg->HasJ = true;
                if ( OptLen > 1 )
                {
                    CmdArg->J = atoff(Opt + 1);
                    CmdArg->HasValueJ = true;
                }
                else
                {
                    CmdArg->HasValueJ = false;
                }
                break;
            }
            default:
                break;
        }
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}
S32 GcodeM240Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
    {
        return 0;
    }

    struct ArgM240_t *Param = (struct ArgM240_t *)ArgPtr;

#ifdef PHOTO_POSITION
    //判断当前是否归零，未归零直接退出
    struct MotionPositionMsg_t CurPosition = {0};
    CurPosition = MotionModel->GetCurPosition();
    if ( CurPosition.HomeX != true || CurPosition.HomeY != true || CurPosition.HomeZ != true )
    {
        return 0;
    }

    //拍照位置信息
    XYZFloat_t OldPos = {0};
    if ( Param->HasValueA )
    {
        OldPos.X = CurPosition.PosX + Param->A;
    }
    else
    {
        OldPos.X = CurPosition.PosX;
    }
    if ( Param->HasValueB )
    {
        OldPos.Y = CurPosition.PosY + Param->B;
    }
    else
    {
        OldPos.Y = CurPosition.PosY;
    }
    OldPos.Z = CurPosition.PosZ;

#ifdef PHOTO_RETRACT_MM
    FLOAT Rval = 0.0f;
    if ( Param-> HasR )
    {
        Rval = Param->R;
    }
    else
    {
        Rval = PHOTO_RETRACT_MM;
    }

    FLOAT Sval = (
#ifdef ADVANCED_PAUSE_FEATURE
            PAUSE_PARK_RETRACT_FEEDRATE
#elif FWRETRACT
            RETRACT_FEEDRATE
#else
            45
#endif
    );

    if ( Param->HasS )
    {
        Sval = Param->S / 60.0f;
    }
    E_Move_M240(-Rval,Sval);
#endif

    FLOAT Fr_mm_s = 0;
    if ( Param->HasValueF )
    {
        Fr_mm_s = Param->F / 60.0f;
    }
    if ( Fr_mm_s && Fr_mm_s < 10.0f )
    {
        Fr_mm_s = 10.0f;
    }


    constexpr struct XYZFloat_t PhotoPosition = PHOTO_POSITION;
    struct XYZFloat_t Raw = {0};
    if ( Param->HasValueX )
    {
        Raw.X = Param->X - MotionModel->GetAxisHomeOffset(X);
    }
    else
    {
        Raw.X = PhotoPosition.X;
    }

    if ( Param->HasValueY )
    {
        Raw.Y = Param->Y - MotionModel->GetAxisHomeOffset(Y);
    }
    else
    {
        Raw.Y = PhotoPosition.Y;
    }

    if ( Param->HasValueZ )
    {
        Raw.Z = Param->Z + CurPosition.PosZ;
    }
    else
    {
        Raw.Z = PhotoPosition.Z + CurPosition.PosZ;
    }

    //将给定的位置约束在软限位开关范围内
    LimitPosBySoftEndstop(Raw);

    //运动到指定位置
    struct MotionCoord_t CurPos = MotionModel->GetLatestCoord();
    CurPos.CoorX = Raw.X;
    CurPos.CoorY = Raw.Y;
    CurPos.CoorZ = Raw.Z;
    struct MotionGcode_t Target = {0};
    Target.Coord = CurPos;
    Target.FeedRate = Fr_mm_s;

    MotionModel->PutMotionCoord(Target);

#ifdef PHOTO_SWITCH_POSITION
    XYZFloat_t PhotoSwitchPosition = PHOTO_SWITCH_POSITION;
    XYZFloat_t SRaw = {0};
    if ( Param->HasI )
    {
        SRaw.X = Param->I - MotionModel->GetAxisHomeOffset(X);
    }
    else
    {
        SRaw.X = PhotoSwitchPosition.X;
    }

    if ( Param->HasJ )
    {
        SRaw.Y = Param->J - MotionModel->GetAxisHomeOffset(Y);
    }
    else
    {
        SRaw.Y = PhotoSwitchPosition.Y;
    }

    //运动到指定位置
    struct FeedrateAttr_t Attr ={0};
    Attr = MotionModel->GetMotionFeedRateAttr();
    Target.Coord.CoorX = SRaw.X;
    Target.Coord.CoorY = SRaw.Y;
    Target.Coord.CoorZ = CurPosition.PosZ;
    Target.Coord.CoorE = CurPosition.PosE;
    Target.FeedRate = Attr.Feedrate;

    MotionModel->PutMotionCoord(Target);

#ifdef PHOTO_SWITCH_MS > 0
    if ( Param->HasD )
    {
        CrU_Sleep(Param->D);
    }
    else
    {
        CrU_Sleep(PHOTO_SWITCH_MS);
    }
#endif

    //运动到指定位置
    CurPos = MotionModel->GetLatestCoord();
    CurPos.CoorX = Raw.X;
    CurPos.CoorY = Raw.Y;
    CurPos.CoorZ = Raw.Z;

    Target.Coord = CurPos;
    Target.FeedRate = Fr_mm_s;
    MotionModel->PutMotionCoord(Target);
#endif
#endif

    //先判断是否存在CHDK引脚,如果存在对目标pin脚写入高电平
    //获取Fd
    S32 Fd = CrGpioOpen(CHDK,0,0);
    if ( IsGpioFdVaild(Fd) != -1 )
    {
        //设置引脚模式
            struct PinInitParam_t Pin = { 0 };
            Pin.Mode = GPIO_MODE_OUTPUT_PP;
            Pin.Pull = GPIO_NOPULL;
            Pin.Speed = GPIO_SPEED_FREQ_HIGH;

            CrGpioIoctl(Fd, GPIO_INIT, (VOID *)&Pin);

            //写入
            struct PinParam_t Status = { (GPIO_PinState)true };
            CrGpioWrite(Fd, (S8 *)&Status, sizeof(Status));

            S64 ChdkTimeout;
            ChdkTimeout = CrGetSystemTimeMilSecs();
            if ( Param->HasD )
            {
                ChdkTimeout += Param->D;
            }
            else
            {
                ChdkTimeout += PHOTO_SWITCH_MS;
            }
    }

#if HAS_PHOTOGRAPH
    SpinPhotoPin();
    CrU_Sleep(7.33);
    SpinPhotoPin();
#endif

#ifdef PHOTO_POSITION
#if PHOTO_DELAY_MS > 0
    S64 Timeout = {0};
    Timeout = CrGetSystemTimeMilSecs();
    if ( Param->HasP )
    {
        Timeout += Param->P;
    }
    else
    {
        Timeout += PHOTO_DELAY_MS;
    }
    while ( (CrGetSystemTimeMilSecs() - Timeout) < 0 )
    {
        CrU_Sleep(5);
    }
#endif

    CurPos = MotionModel->GetLatestCoord();
    CurPos.CoorX = Raw.X;
    CurPos.CoorY = Raw.Y;
    CurPos.CoorZ = Raw.Z;

    Target.Coord = CurPos;
    Target.FeedRate = Fr_mm_s;
    MotionModel->PutMotionCoord(Target);

#ifdef PHOTO_RETRACT_MM
    E_Move_M240(Rval,Sval);
#endif
#endif
    return 1;
}
S32 GcodeM240Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
