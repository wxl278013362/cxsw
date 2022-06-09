#include "GcodeM860Handle.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrModel/CrMotion.h"
#include "CrMotion/Bresenham.h"
#include <cmath>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define CMD_M860  "M860"

U8 I2CPEAddr = 0;
U8 I2CPEIdx  = 0;
BOOL I2CPEAnyaxis = false;
S8 AxisCodes[4] = {'X','Y','Z','E'};

struct I2CPositionEncoder_t Encoders[I2CPE_ENCODER_CNT];
static BOOL I2CPositionEncoderInitFlag = false;
FLOAT AxisStepsPerMm[AxisNum];

static VOID I2CPositionEncoderInit()
{
    if ( I2CPositionEncoderInitFlag )
    {
        printf("I2c position encoder has inited\n");
        return;
    }
    for ( U8 i  = 0; i < I2CPE_ENCODER_CNT; i++ )
    {
        Encoders[i] = {
            .EncoderAxis         = I2CPE_DEF_AXIS,
            .I2CAddress          = I2CPE_DEF_ADDR,
            .Type                = I2CPE_DEF_TYPE,
            .EncoderTicksPerUnit = I2CPE_DEF_ENC_TICKS_UNIT,
            .StepperTicks        = I2CPE_DEF_TICKS_REV,
            .ZeroOffset          = 0,
            .ErrorCount          = 0,
            .Position            = 0,
            .ErrorPrev           = 0,
            .H                   = I2CPE_MAG_SIG_NF,// Magnetic field strength
            .Ec                  = true,
            .Active              = false,
            .EcThreshold         = I2CPE_DEF_EC_THRESH
        };
    }

    I2CPositionEncoderInitFlag = true;
}

VOID ChangeModuleAddress(const U8 oldAddr, const U8 newAddr)
{
    // M864 TODO... Marlin函数：change_module_address
/*
    1. First check 'new' address is not in use
    2. Then check that we can find the module on the oldaddr address
    3. Change the modules address
    4. Wait for the module to reset (can probably be improved by polling address with a timeout).
    5. Look for the module at the new address.
    6. Now, if this module is configured, find which encoder instance it's supposed to correspond to
        and enable it (it will likely have failed initialization on power-up, before the address change).
*/
}

VOID TestAxis(const U8 Idx)
{
    if ( !WITHIN(Idx, 0, I2CPE_ENCODER_CNT - 1) )
    {
        return;
    }

    // M862 TODO...
}

VOID ResetErrorCount(const U8 Idx, const enum Axis_t Axis)
{
    if ( !WITHIN(Idx, 0, I2CPE_ENCODER_CNT - 1) )
    {
        return;
    }

    Encoders[Idx].ErrorCount = 0;
    printf("Error count on %c axis is %d\n", AxisCodes[Idx], Encoders[Idx].ErrorCount);
}

VOID ReportErrorCount(const U8 Idx, const enum Axis_t Axis)
{
    if ( !WITHIN(Idx, 0, I2CPE_ENCODER_CNT - 1) )
    {
        return;
    }

    printf("Error count on %c axis is %d\n", AxisCodes[Idx], Encoders[Idx].ErrorCount);
}

VOID ReportModuleFirmware(U8 Addr)
{
    //M865 TODO
    /* 1. First check there is a module
     * 2. Read value
     * 3. Set module back to normal (distance) mode
     */
}

S32 GetRawCount(I2CPositionEncoder_t Encoders)  //计算获取Encoder原始的count值
{

}

FLOAT MmFromCount(const S32 Count,S8 Idx)
{
    struct StepperAttr_t MotionAttr = {0};
    StepperGetAttr(&MotionAttr,(Axis_t)Encoders[Idx].EncoderAxis);
    switch (Encoders[Idx].Type)
    {
        case I2CPE_ENC_TYPE_LINEAR:
          return Count / Encoders[Idx].EncoderTicksPerUnit;
        case I2CPE_ENC_TYPE_ROTARY:
          return (Count * Encoders[Idx].StepperTicks) / (Encoders[Idx].EncoderTicksPerUnit * MotionAttr.Perimeter);
        default: return -1;
    }
}

VOID ReportPosition(const S8 Idx, const BOOL Units, const BOOL NoOffset)
{
    if ( !(Idx >= 0 && Idx <= I2CPE_ENCODER_CNT - 1) )
        return;

    if ( Units )
    {
        if ( NoOffset )
        {
            printf("%f\n",MmFromCount(GetRawCount(Encoders[Idx]),Idx));
        }
        else
        {
            printf("%f\n",MmFromCount(GetRawCount(Encoders[Idx]),Idx) - Encoders[Idx].ZeroOffset);
        }
    }
    else
    {
        if ( NoOffset )
        {
            S32 RawCount = GetRawCount(Encoders[Idx]);
            printf("%d ",Encoders[Idx].EncoderAxis);

            for(U8 j = 31; j > 0; j-- )
            {
                printf("%d",(BOOL)(0x00000001 & (RawCount >> j)));
            }
            printf("%d %d\n",(BOOL)(0x00000001 & RawCount),RawCount);
        }
        else
        {
            printf("%d\n",GetRawCount(Encoders[Idx]) - Encoders[Idx].ZeroOffset);
        }
    }
}

S8 IdxFromAddr(const U8 Addr)
{
    for ( int i = 0; i < I2CPE_ENCODER_CNT; i++ )
    {
        if ( Encoders[i].I2CAddress == Addr )
        {
            return i;
        }
    }
    return -1;
}

U8 IdxFromAxis(enum Axis_t Axis)
{
    for ( int i = 0; i < I2CPE_ENCODER_CNT; i++ )
    {
        if ( Encoders[i].EncoderAxis == Axis )
        {
            return i;
        }
    }
    return -1;
}

U8 EncodersGetAddress(U8 Index)
{
    return Encoders[Index].I2CAddress;
}

enum Axis_t EncoderGetAxis(U8 Index)
{
    return Encoders[Index].EncoderAxis;
}

VOID EnableEc(U8 Idx, const BOOL Enabled, enum Axis_t Axis)
{
    if ( !WITHIN(Idx, 0, I2CPE_ENCODER_CNT - 1) )
    {
        return;
    }

    Encoders[Idx].Ec = Enabled;
}

S32 GetAxisErrorSteps(const U8 Idx, const BOOL Report)
{
    if ( !Encoders[Idx].Active )
    {
        if ( Report )
        {
            printf("%c axis encoder not active!", AxisCodes[Idx]);
        }

        return 0;
    }

    FLOAT stepperTicksPerUnit;
    S32 encoderTicks = Encoders[Idx].Position;
    S32 encoderCountInStepperTicksScaled;

    stepperTicksPerUnit = (Encoders[Idx].Type == I2CPE_ENC_TYPE_ROTARY) ? Encoders[Idx].StepperTicks : AxisStepsPerMm[Encoders[Idx].EncoderAxis];
    encoderCountInStepperTicksScaled = lroundf((stepperTicksPerUnit * encoderTicks) / Encoders[Idx].EncoderTicksPerUnit);
    FLOAT Target = 0;
    switch ( Idx )
    {
        case 0:
            Target = MotionModel->GetCurPosition().PosX;
            break;
        case 1:
            Target = MotionModel->GetCurPosition().PosY;
            break;
        case 2:
            Target = MotionModel->GetCurPosition().PosZ;
            break;
        case 3:
            Target = MotionModel->GetCurPosition().PosE;
            break;
        default:
            break;
    }
    int32_t Error = encoderCountInStepperTicksScaled - Target;

    const bool suppressOutput = (ABS(Error - Encoders[Idx].ErrorPrev) > 100);

    Encoders[Idx].ErrorPrev = Error;

    if ( Report )
    {
        printf("%c axis target=%4f; actual=%4f; err=%4f", AxisCodes[Idx], Target, encoderCountInStepperTicksScaled, Error);
    }

    if ( suppressOutput )
    {
        if ( Report )
        {
            printf("!Discontinuity. Suppressing error.");
            Error = 0;
        }
    }

    return Error;
}

VOID ReportError(const U8 Idx)
{
    if ( !WITHIN(Idx, 0, I2CPE_ENCODER_CNT - 1) )
    {
        return;
    }

    GetAxisErrorSteps(Idx, true);
}

S8 Parse(VOID *ArgPtr)
{
    if ( !ArgPtr )
    {
        return 0;
    }

    I2CPositionEncoderInit();
    struct ArgM86x_t *Param = (struct ArgM86x_t *)ArgPtr;
    I2CPEAddr = 0;
    if ( Param->HasA )
    {
        if ( !Param->HasValueA )
        {
            printf("?A seen, but no address specified! [30-200]\n");
            return I2CPE_PARSE_ERR;
        }
        I2CPEAddr = Param->A;

        if ( !WITHIN(I2CPEAddr, I2CPEADDR_MIN , I2CPEADDR_MIN) )
        {
            printf("?Address out of range. [30-200]\n");
            return I2CPE_PARSE_ERR;
        }
        I2CPEIdx = IdxFromAddr(I2CPEAddr);

        if ( I2CPEIdx >= I2CPE_ENCODER_CNT )
        {
            printf("?No device with this address!\n");
            return I2CPE_PARSE_ERR;
        }
    }
    else if ( Param->HasI )
    {
        if ( !Param->HasValueI )
        {
            printf("?I seen, but no index specified! [0-%d]\n",I2CPE_ENCODER_CNT - 1);
            return I2CPE_PARSE_ERR;
        }
        I2CPEIdx = Param->I;

        if ( I2CPEIdx >= I2CPE_ENCODER_CNT )
        {
            printf("?Index out of range. [0-%d]\n",I2CPE_ENCODER_CNT - 1);
            return I2CPE_PARSE_ERR;
        }
        I2CPEAddr = I2CPE_DEF_ADDR;//
    }
    else
    {
        I2CPEIdx = 0xFF;
    }

    if ( Param->HasX || Param->HasY || Param->HasZ || Param->HasE )
    {
        I2CPEAnyaxis = true;
    }
    else
    {
        I2CPEAnyaxis = false;
    }

    return I2CPE_PARSE_OK;

}
/*********额外定义的变量和函数************/

S32 GcodeM860Parse(const S8 *CmdBuff, VOID *Arg)
{
    //1、判空
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    //2、判断Gcode命令长度是否大于1且是否为M命令
    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || CmdBuff[0] != GCODE_CMD_M )
    {
        return CrErr;
    }

    //3、获取命令
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )   //防止获取命令失败
    {
        return CrErr;
    }

    if ( strcmp(Cmd,CMD_M860) != 0 )    //判断是否是M860命令
    {
        return CrErr;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M860) + sizeof(S8);
    struct ArgM860_t *CmdArg = (struct ArgM860_t *)Arg;

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
            printf("Gcode M860 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_A:
            {
                CmdArg->Common->A = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Common->A = atoi(Opt + 1);
                    CmdArg->Common->HasValueA = true;
                }
                else
                {
                    CmdArg->Common->HasValueA = false;
                }
                break;
            }
            case OPT_E:
            {
                CmdArg->Common->HasE = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Common->E = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->Common->E = 0;
                }
                break;
            }
            case OPT_I:
            {
                CmdArg->Common->HasI = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Common->I = atoi(Opt + 1);
                    CmdArg->Common->HasValueI = true;
                }
                else
                {
                    CmdArg->Common->HasValueI = false;
                }
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
                    CmdArg->O = false;
                }
                break;
            }
            case OPT_U:
            {
                CmdArg->HasU = true;
                if ( OptLen > 1 )
                {
                    CmdArg->U = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->U = false;
                }
                break;
            }
            case OPT_X:
            {
                CmdArg->Common->HasX = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Common->X = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->Common->X = 0;
                }
                break;
            }
            case OPT_Y:
            {
                CmdArg->Common->HasY = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Common->Y = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->Common->Y = 0;
                }
                break;
            }
            case OPT_Z:
            {
                CmdArg->Common->HasZ = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Common->Z = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->Common->Z = 0;
                }
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM860Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return CrErr;
    }

    struct ArgM860_t *Param = (struct ArgM860_t *)ArgPtr;
    if ( Parse(Param->Common) )
    {
        return CrErr;
    }

    BOOL HasU = false, HasO = false;
    if ( Param->HasU )
        HasU = true;
    if ( Param->HasO )
        HasO = true;

    if ( I2CPEIdx == 0xFF )
    {
        for ( U8 i = X; i <= E; i++ )   //从0到3分别代表X,Y,Z,E四个轴
        {
            BOOL HasAxis = false;
            switch( i )
            {
                case X:
                {
                    if ( Param->Common->HasX )
                        HasAxis = true;
                    break;
                }
                case Y:
                {
                    if ( Param->Common->HasY )
                        HasAxis = true;
                    break;
                }
                case Z:
                {
                    if ( Param->Common->HasZ )
                        HasAxis = true;
                    break;
                }
                case E:
                {
                    if ( Param->Common->HasE )
                        HasAxis = true;
                    break;
                }
                default:
                    break;
            }
            if( !I2CPEAnyaxis || HasAxis )
            {
                U8 Idx = IdxFromAxis((enum Axis_t)i);
                if ( (S8)Idx >= 0 )
                {
                    ReportPosition(Idx, HasU, HasO);
                }
            }
        }
    }
    else
    {
        ReportPosition(I2CPEIdx,HasU,HasO);
    }
    return GCODE_EXEC_OK;
}

S32 GcodeM860Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
