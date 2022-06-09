#include "GcodeM350Handle.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"

#define CMD_M350 ("M350")


#define PIN_NAME_EXISTS(PN)  defined(PN##_PIN_NAEM)

static U8 ReadPinLevel(S8 *PinName)
{
    S32 Fd = CrGpioOpen(PinName, 0, 0);
    if ( Fd <= 65535)
        return 0;

    struct PinParam_t Status;
    CrGpioRead(Fd, (S8 *)&Status, sizeof(Status));

    return Status.PinState;
}

static VOID SetPinLevel(S8 *PinName, U8 Level)
{
    S32 Fd = CrGpioOpen(PinName, 0, 0);
    if ( Fd <= 65535)
        return;

    struct PinParam_t Status = {(GPIO_PinState)Level};
    CrGpioWrite(Fd, (S8 *)&Status, sizeof(Status));

//    CrGpioClose(Fd);

    return ;
}

//设置电机微步进
VOID SteppersMicrostepReadings()
{
    printf("MS1|2|3 Pins");
#if HAS_X_MS_PINS
    printf(" X:");
#if PIN_NAME_EXISTS(X_MS1)
    printf("%c", '0' + ReadPinLevel(X_MS1_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(X_MS2)
    printf("%c", '0' + ReadPinLevel(X_MS2_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(X_MS3)
    printf("%c", '0' + ReadPinLevel(X_MS3_PIN_NAME));
#endif

#endif
#if HAS_Y_MS_PINS
    printf(" Y:");
#if PIN_NAME_EXISTS(Y_MS1)
    printf("%c", '0' + ReadPinLevel(Y_MS1_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(Y_MS2)
    printf("%c", '0' + ReadPinLevel(Y_MS2_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(Y_MS3)
    printf("%c", '0' + ReadPinLevel(Y_MS3_PIN_NAME));
#endif
#endif
#if HAS_Z_MS_PINS
    printf(" Z:");
#if PIN_NAME_EXISTS(Z_MS1)
    printf("%c", '0' + ReadPinLevel(Z_MS1_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(Z_MS2)
    printf("%c", '0' + ReadPinLevel(Z_MS2_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(Z_MS3)
    printf("%c", '0' + ReadPinLevel(Z_MS3_PIN_NAME));
#endif
#endif
#if HAS_E0_MS_PINS
    printf(" E0:");
#if PIN_NAME_EXISTS(E0_MS1)
    printf("%c", '0' + ReadPinLevel(E0_MS1_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E0_MS2)
    printf("%c", '0' + ReadPinLevel(E0_MS2_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E0_MS3)
    printf("%c", '0' + ReadPinLevel(E0_MS3_PIN_NAME));
#endif
#endif
#if HAS_E1_MS_PINS
    printf(" E1:");
#if PIN_NAME_EXISTS(E1_MS1)
    printf("%c", '0' + ReadPinLevel(E1_MS1_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E1_MS2)
    printf("%c", '0' + ReadPinLevel(E1_MS2_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E1_MS3)
    printf("%c", '0' + ReadPinLevel(E1_MS3_PIN_NAME));
#endif
#endif

#if HAS_E2_MS_PINS
    printf(" E2:");
#if PIN_NAME_EXISTS(E2_MS1)
    printf("%c", '0' + ReadPinLevel(E2_MS1_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E2_MS2)
    printf("%c", '0' + ReadPinLevel(E2_MS2_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E2_MS3)
    printf("%c", '0' + ReadPinLevel(E2_MS3_PIN_NAME));
#endif
#endif
#if HAS_E3_MS_PINS
    printf(" E3:");
#if PIN_NAME_EXISTS(E3_MS1)
    printf("%c", '0' + ReadPinLevel(E3_MS1_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E3_MS2)
    printf("%c", '0' + ReadPinLevel(E3_MS2_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E3_MS3)
    printf("%c", '0' + ReadPinLevel(E3_MS3_PIN_NAME));
#endif
#endif

#if HAS_E4_MS_PINS
    printf(" E4:");
#if PIN_NAME_EXISTS(E4_MS1)
    printf("%c", '0' + ReadPinLevel(E4_MS1_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E4_MS2)
    printf("%c", '0' + ReadPinLevel(E4_MS2_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E4_MS3)
    printf("%c", '0' + ReadPinLevel(E4_MS3_PIN_NAME));
#endif
#endif

#if HAS_E5_MS_PINS
    printf(" E5:");
#if PIN_NAME_EXISTS(E5_MS1)
    printf("%c", '0' + ReadPinLevel(E5_MS1_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E5_MS2)
    printf("%c", '0' + ReadPinLevel(E5_MS2_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E5_MS3)
    printf("%c", '0' + ReadPinLevel(E5_MS3_PIN_NAME));
#endif
#endif

#if HAS_E6_MS_PINS
    printf(" E6:");
#if PIN_NAME_EXISTS(E6_MS1)
    printf("%c", '0' + ReadPinLevel(E6_MS1_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E6_MS2)
    printf("%c", '0' + ReadPinLevel(E6_MS2_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E6_MS3)
    printf("%c", '0' + ReadPinLevel(E6_MS3_PIN_NAME));
#endif
#endif

#if HAS_E7_MS_PINS
    printf(" E7:");
#if PIN_NAME_EXISTS(E7_MS1)
    printf("%c", '0' + ReadPinLevel(E7_MS1_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E7_MS2)
    printf("%c", '0' + ReadPinLevel(E7_MS2_PIN_NAME));
#endif
#if PIN_NAME_EXISTS(E7_MS3)
    printf("%c", '0' + ReadPinLevel(E7_MS3_PIN_NAME));
#endif
#endif

    printf("\n");
}

VOID MicrostepMs(enum Axis_t  AxisType, U8 Ms1Level, U8 Ms2Level, U8 Ms3Level)
{
    if ( (AxisType >= AxisNum) || (AxisType < 0) )
        return ;
    if (Ms1Level >= 0) switch (AxisType)
    {
      #if HAS_X_MS_PINS || HAS_X2_MS_PINS
        case 0:
          #if HAS_X_MS_PINS
            //WRITE(X_MS1_PIN, Ms1Level);  //将对应的管脚设置为高电平还是低电平
            SetPinLevel(X_MS1_PIN_NAME, Ms1Level);
          #endif
          #if HAS_X2_MS_PINS
            //WRITE(X2_MS1_PIN, Ms1Level);
            SetPinLevel(X2_MS1_PIN_NAME, Ms1Level);
          #endif
          break;
      #endif
      #if HAS_Y_MS_PINS || HAS_Y2_MS_PINS
        case 1:
          #if HAS_Y_MS_PINS
            //WRITE(Y_MS1_PIN, Ms1Level);
            SetPinLevel(Y_MS1_PIN_NAME, Ms1Level);
          #endif
          #if HAS_Y2_MS_PINS
            //WRITE(Y2_MS1_PIN, Ms1Level);
            SetPinLevel(Y2_MS1_PIN_NAME, Ms1Level);
          #endif
          break;
      #endif
      #if HAS_SOME_Z_MS_PINS
        case 2:
          #if HAS_Z_MS_PINS
            //WRITE(Z_MS1_PIN, Ms1Level);
            SetPinLevel(Z_MS1_PIN_NAME, Ms1Level);
          #endif
          #if HAS_Z2_MS_PINS
            //WRITE(Z2_MS1_PIN, Ms1Level);
            SetPinLevel(Z2_MS1_PIN_NAME, Ms1Level);
          #endif
          #if HAS_Z3_MS_PINS
            //WRITE(Z3_MS1_PIN, Ms1Level);
            SetPinLevel(Z3_MS1_PIN_NAME, Ms1Level);
          #endif
          #if HAS_Z4_MS_PINS
            //WRITE(Z4_MS1_PIN, Ms1Level);
            SetPinLevel(Z4_MS1_PIN_NAME, Ms1Level);
          #endif
          break;
      #endif
      #if HAS_E0_MS_PINS
        case  3:
            //WRITE(E0_MS1_PIN, Ms1Level);
            SetPinLevel(E0_MS1_PIN_NAME, Ms1Level);
            break;
      #endif
      #if HAS_E1_MS_PINS
        case  4:
            //WRITE(E1_MS1_PIN, Ms1Level);
            SetPinLevel(E1_MS1_PIN_NAME, Ms1Level);
            break;
      #endif
      #if HAS_E2_MS_PINS
        case  5:
            //WRITE(E2_MS1_PIN, Ms1Level);
            SetPinLevel(E2_MS1_PIN_NAME, Ms1Level);
            break;
      #endif
      #if HAS_E3_MS_PINS
        case  6:
            //WRITE(E3_MS1_PIN, Ms1Level);
            SetPinLevel(E3_MS1_PIN_NAME, Ms1Level);
            break;
      #endif
      #if HAS_E4_MS_PINS
        case  7:
            //WRITE(E4_MS1_PIN, Ms1Level);
            SetPinLevel(E4_MS1_PIN_NAME, Ms1Level);
            break;
      #endif
      #if HAS_E5_MS_PINS
        case  8:
            //WRITE(E5_MS1_PIN, Ms1Level);
            SetPinLevel(E5_MS1_PIN_NAME, Ms1Level);
            break;
      #endif
      #if HAS_E6_MS_PINS
        case  9:
            //WRITE(E6_MS1_PIN, Ms1Level);
            SetPinLevel(E6_MS1_PIN_NAME, Ms1Level);
            break;
      #endif
      #if HAS_E7_MS_PINS
        case 10:
            //WRITE(E7_MS1_PIN, Ms1Level);
            SetPinLevel(E7_MS1_PIN_NAME, Ms1Level);
            break;
      #endif
    }
    if (Ms2Level >= 0) switch (AxisType)
    {
      #if HAS_X_MS_PINS || HAS_X2_MS_PINS
        case 0:
          #if HAS_X_MS_PINS
            //WRITE(X_MS2_PIN, Ms2Level);
            SetPinLevel(X_MS2_PIN_NAME, Ms2Level);
          #endif
          #if HAS_X2_MS_PINS
//            WRITE(X2_MS2_PIN, Ms2Level);
            SetPinLevel(X2_MS2_PIN_NAME, Ms2Level);
          #endif
          break;
      #endif
      #if HAS_Y_MS_PINS || HAS_Y2_MS_PINS
        case 1:
          #if HAS_Y_MS_PINS
//            WRITE(Y_MS2_PIN, Ms2Level);
            SetPinLevel(Y_MS2_PIN_NAME, Ms2Level);
          #endif
          #if HAS_Y2_MS_PINS
//            WRITE(Y2_MS2_PIN, Ms2Level);
            SetPinLevel(Y2_MS2_PIN_NAME, Ms2Level);
          #endif
          break;
      #endif
      #if HAS_SOME_Z_MS_PINS
        case 2:
          #if HAS_Z_MS_PINS
//            WRITE(Z_MS2_PIN, Ms2Level);
            SetPinLevel(Z_MS2_PIN_NAME, Ms2Level);
          #endif
          #if HAS_Z2_MS_PINS
//            WRITE(Z2_MS2_PIN, Ms2Level);
            SetPinLevel(Z2_MS2_PIN_NAME, Ms2Level);
          #endif
          #if HAS_Z3_MS_PINS
            //WRITE(Z3_MS2_PIN, Ms2Level);
            SetPinLevel(Z3_MS2_PIN_NAME, Ms2Level);
          #endif
          #if HAS_Z4_MS_PINS
//            WRITE(Z4_MS2_PIN, Ms2Level);
            SetPinLevel(Z4_MS2_PIN_NAME, Ms2Level);
          #endif
          break;
      #endif
      #if HAS_E0_MS_PINS
        case  3:
            //WRITE(E0_MS2_PIN, Ms2Level);
            SetPinLevel(E0_MS2_PIN_NAME, Ms2Level);
            break;
      #endif
      #if HAS_E1_MS_PINS
        case  4:
            //WRITE(E1_MS2_PIN, Ms2Level);
            SetPinLevel(E1_MS2_PIN_NAME, Ms2Level);
            break;
      #endif
      #if HAS_E2_MS_PINS
        case  5:
            //WRITE(E2_MS2_PIN, Ms2Level);
            SetPinLevel(E2_MS2_PIN_NAME, Ms2Level);
            break;
      #endif
      #if HAS_E3_MS_PINS
        case  6:
            //WRITE(E3_MS2_PIN, Ms2Level);
            SetPinLevel(E3_MS2_PIN_NAME, Ms2Level);
            break;
      #endif
      #if HAS_E4_MS_PINS
        case  7:
            //WRITE(E4_MS2_PIN, Ms2Level);
            SetPinLevel(E4_MS2_PIN_NAME, Ms2Level);
            break;
      #endif
      #if HAS_E5_MS_PINS
        case  8:
            //WRITE(E5_MS2_PIN, Ms2Level);
            SetPinLevel(E5_MS2_PIN_NAME, Ms2Level);
            break;
      #endif
      #if HAS_E6_MS_PINS
        case  9:
            //WRITE(E6_MS2_PIN, Ms2Level);
            SetPinLevel(E6_MS2_PIN_NAME, Ms2Level);
            break;
      #endif
      #if HAS_E7_MS_PINS
        case 10:
            //WRITE(E7_MS2_PIN, Ms2Level);
            SetPinLevel(E7_MS2_PIN_NAME, Ms2Level);
            break;
      #endif
    }
    if (Ms3Level >= 0) switch (AxisType)
    {
      #if HAS_X_MS_PINS || HAS_X2_MS_PINS
        case 0:
          #if HAS_X_MS_PINS && PIN_NAME_EXISTS(X_MS3)
            //WRITE(X_MS3_PIN, Ms3Level);
            SetPinLevel(X_MS3_PIN_NAME, Ms3Level);
          #endif
          #if HAS_X2_MS_PINS && PIN_NAME_EXISTS(X2_MS3)
            //WRITE(X2_MS3_PIN, Ms3Level);
            SetPinLevel(X2_MS3_PIN_NAME, Ms3Level);
          #endif
          break;
      #endif
      #if HAS_Y_MS_PINS || HAS_Y2_MS_PINS
        case 1:
          #if HAS_Y_MS_PINS && PIN_NAME_EXISTS(Y_MS3)
//            WRITE(Y_MS3_PIN, Ms3Level);
            SetPinLevel(Y_MS3_PIN_NAME, Ms3Level);
          #endif
          #if HAS_Y2_MS_PINS && PIN_NAME_EXISTS(Y2_MS3)
            //WRITE(Y2_MS3_PIN, Ms3Level);
            SetPinLevel(Y2_MS3_PIN_NAME, Ms3Level);
          #endif
          break;
      #endif
      #if HAS_SOME_Z_MS_PINS
        case 2:
          #if HAS_Z_MS_PINS && PIN_NAME_EXISTS(Z_MS3)
            //WRITE(Z_MS3_PIN, Ms3Level);
            SetPinLevel(Z_MS3_PIN_NAME, Ms3Level);
          #endif
          #if HAS_Z2_MS_PINS && PIN_NAME_EXISTS(Z2_MS3)
            //WRITE(Z2_MS3_PIN, Ms3Level);
            SetPinLevel(Z2_MS3_PIN_NAME, Ms3Level);
          #endif
          #if HAS_Z3_MS_PINS && PIN_NAME_EXISTS(Z3_MS3)
//            WRITE(Z3_MS3_PIN, Ms3Level);
            SetPinLevel(Z3_MS3_PIN_NAME, Ms3Level);
          #endif
          #if HAS_Z4_MS_PINS && PIN_NAME_EXISTS(Z4_MS3)
//            WRITE(Z4_MS3_PIN, Ms3Level);
            SetPinLevel(Z4_MS3_PIN_NAME, Ms3Level);
          #endif
          break;
      #endif
      #if HAS_E0_MS_PINS && PIN_NAME_EXISTS(E0_MS3)
        case  3:
            //WRITE(E0_MS3_PIN, Ms3Level);
            SetPinLevel(E0_MS3_PIN_NAME, Ms3Level);
            break;
      #endif
      #if HAS_E1_MS_PINS && PIN_NAME_EXISTS(E1_MS3)
        case  4:
            //WRITE(E1_MS3_PIN, Ms3Level);
            SetPinLevel(E1_MS3_PIN_NAME, Ms3Level);
            break;
      #endif
      #if HAS_E2_MS_PINS && PIN_NAME_EXISTS(E2_MS3)
        case  5:
            //WRITE(E2_MS3_PIN, Ms3Level);
            SetPinLevel(E2_MS3_PIN_NAME, Ms3Level);
            break;
      #endif
      #if HAS_E3_MS_PINS && PIN_NAME_EXISTS(E3_MS3)
        case  6:
            //WRITE(E3_MS3_PIN, Ms3Level);
            SetPinLevel(E3_MS3_PIN_NAME, Ms3Level);
            break;
      #endif
      #if HAS_E4_MS_PINS && PIN_NAME_EXISTS(E4_MS3)
        case  7:
            WRITE(E4_MS3_PIN, Ms3Level);
            SetPinLevel(E4_MS3_PIN_NAME, Ms3Level);
            break;
      #endif
      #if HAS_E5_MS_PINS && PIN_NAME_EXISTS(E5_MS3)
        case  8:
            //WRITE(E5_MS3_PIN, Ms3Level);
            SetPinLevel(E5_MS3_PIN_NAME, Ms3Level);
            break;
      #endif
      #if HAS_E6_MS_PINS && PIN_NAME_EXISTS(E6_MS3)
        case  9:
            //WRITE(E6_MS3_PIN, Ms3Level);
            SetPinLevel(E6_MS3_PIN_NAME, Ms3Level);
            break;
      #endif
      #if HAS_E7_MS_PINS && PIN_NAME_EXISTS(E7_MS3)
        case 10:
            //WRITE(E7_MS3_PIN, Ms3Level);
            SetPinLevel(E7_MS3_PIN_NAME, Ms3Level);
            break;
      #endif
    }

    return ;
}

static VOID SetStepperMicrostepMode(enum Axis_t  AxisType, U8 Mode )
{
      switch ( Mode )
      {
        #if HAS_MICROSTEP1
            case 1: MicrostepMs(AxisType, MICROSTEP1); break;
        #endif
        #if HAS_MICROSTEP2
            case 2: MicrostepMs(AxisType, MICROSTEP2); break;
        #endif
        #if HAS_MICROSTEP4
            case 4: MicrostepMs(AxisType, MICROSTEP4); break;
        #endif
        #if HAS_MICROSTEP8
            case 8: MicrostepMs(AxisType, MICROSTEP8); break;
        #endif
        #if HAS_MICROSTEP16
            case 16: MicrostepMs(AxisType, MICROSTEP16); break;
        #endif
        #if HAS_MICROSTEP32
            case 32: MicrostepMs(AxisType, MICROSTEP32); break;
        #endif
        #if HAS_MICROSTEP64
            case 64: MicrostepMs(AxisType, MICROSTEP64); break;
        #endif
        #if HAS_MICROSTEP128
            case 128: MicrostepMs(AxisType, MICROSTEP128); break;
        #endif

        default: printf("Microsteps unavailable\n"); break;
    }

    return;
}

S32 GcodeM350Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S8 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M350) != 0 )  //传进来的命令不是G0命令
        return 0;

    S8 Offset = strlen(CMD_M350) + sizeof(S8); //加1是越过分隔符
    struct ArgM350_t *CmdArg = (struct ArgM350_t *)Arg;

    S8 Opt[20] = {0};
    S8 OptLen = 0;

    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) ) break;

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )
        {
            case OPT_B:
            {
                CmdArg->HasB = true;
                if ( OptLen > 1 ) CmdArg->B = atoi( Opt + 1 );
                break;
            }
            case OPT_E:
            {
                CmdArg->HasE = true;
                if ( OptLen > 1 ) CmdArg->E = atoi( Opt + 1 );
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atoi( Opt + 1 );
                break;
            }
            case OPT_X:
            {
                CmdArg->HasX = true;
                if ( OptLen > 1 ) CmdArg->X = atof( Opt + 1 );
                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = true;
                if ( OptLen > 1 ) CmdArg->Y = atof( Opt + 1 );
                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = true;
                if ( OptLen > 1 ) CmdArg->Z = atof( Opt + 1 );
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}

S32 GcodeM350Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM350_t *Param = (struct ArgM350_t *)ArgPtr;
    if ( Param->HasS )
    {
        //给5个电机都设置模式

        S8 i = 0, Count = 5;
        if ( Count > AxisNum )
            Count = AxisNum;

        for ( i = 0; i < Count; i++ )
            SetStepperMicrostepMode((Axis_t)i, Param->S);
    }

    if ( Param->HasX )
    {
        //给X轴电机设置模式
        SetStepperMicrostepMode(X, Param->X);
    }

    if ( Param->HasY )
    {
        //给Y轴电机设置模式
        SetStepperMicrostepMode(Y, Param->Y);
    }

    if ( Param->HasZ )
    {
        //给Z轴电机设置模式
        SetStepperMicrostepMode(Z, Param->Z);
    }

    if ( Param->HasB )
    {
        //给第5个电机设置模式
        if ( AxisNum >= 5 )
            SetStepperMicrostepMode((Axis_t)4, Param->B);
    }

    SteppersMicrostepReadings();

    return 1;
}

S32 GcodeM350Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;



    return Len;
}
