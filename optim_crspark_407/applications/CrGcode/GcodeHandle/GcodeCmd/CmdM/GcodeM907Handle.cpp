
#include "GcodeM907Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"

#define CMD_M907  "M907"

VOID SetCurrentFromDigipot(U32 StepperFd, FLOAT Current)    //设置电机的电流
{

    return;
}

VOID SetCurrentFromDac(U32 StepperFd, FLOAT Current)    //设置电机的电流
{

}

VOID SetCurrentFromI2C(U32 StepperFd, FLOAT Current)
{

}

S32 GcodeM907Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M911
    if ( strcmp(Cmd, CMD_M907) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M907) + sizeof(S8); //加1是越过分隔符
    struct ArgM907_t *CmdArg = (struct ArgM907_t *)Arg;

    while ( Offset < Len )  //处理结束
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            //printf(" Cmd get opt failed  cmd = %s\n", CmdBuff);
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_B:
            {
                CmdArg->HasB = true;
                if ( OptLen > 1 ) CmdArg->B = atof( Opt + 1 );
                break;
            }
            case OPT_C:
            {
                CmdArg->HasC = true;
                if ( OptLen > 1 ) CmdArg->C = atof( Opt + 1 );
                break;
            }
            case OPT_D:
            {
                CmdArg->HasD = true;
                if ( OptLen > 1 ) CmdArg->D = atof( Opt + 1 );
                break;
            }
            case OPT_E:
            {
                CmdArg->HasE = true;
                if ( OptLen > 1 ) CmdArg->E = atof( Opt + 1 );
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atof( Opt + 1 );
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
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    return 1;
 }

S32 GcodeM907Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM907_t *CmdArg = (struct ArgM907_t *)ArgPtr;
    U32 StepperFd = 0;
#if HAS_DIGIPOTSS
    if (CmdArg->HasX)
    {
        StepperFd = 0;  //获取X轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDigipot(StepperFd, CmdArg->X);
        }
    }

    if (CmdArg->HasY)
    {
        StepperFd = 0;  //获取Y轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDigipot(StepperFd, CmdArg->Y);
        }
    }

    if (CmdArg->HasZ)
    {
        StepperFd = 0;  //获取Z轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDigipot(StepperFd, CmdArg->Z);
        }
    }

    if (CmdArg->HasE)
    {
        StepperFd = 0;  //获取E轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDigipot(StepperFd, CmdArg->E);
        }
    }

    if (CmdArg->HasB)
    {
        StepperFd = 0;  //获取E1轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDigipot(StepperFd, CmdArg->B);
        }
    }

    if (CmdArg->HasS)
    {
        S32 i = 0;
        for ( i = 0; i < 4; i++ )   //0-4分别对应的轴电机是XYZE和E0
        {
            StepperFd = 0;  //获取轴电机的Fd
            if ( StepperFd )
            {
                SetCurrentFromDigipot(StepperFd, CmdArg->S);
            }
        }
    }

#elif HAS_MOTOR_CURRENT_PWM    //可控电机电流

#if MOTOR_CURRENT_PWM_X_PIN
    if (CmdArg->HasX)
    {
        StepperFd = 0;  //获取X轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDigipot(StepperFd, CmdArg->X);
        }
    }
#endif

#if MOTOR_CURRENT_PWM_Y_PIN
    if (CmdArg->HasY)
    {
        StepperFd = 0;  //获取Y轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDigipot(StepperFd, CmdArg->Y);
        }
    }
#endif

#if MOTOR_CURRENT_PWM_Z_PIN
    if (CmdArg->HasZ)
    {
        StepperFd = 0;  //获取Z轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDigipot(StepperFd, CmdArg->Z);
        }
    }
#endif

#if MOTOR_CURRENT_PWM_E_PIN
    if (CmdArg->HasE)
    {
        StepperFd = 0;  //获取E轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDigipot(StepperFd, CmdArg->E);
        }
    }
#endif


#endif

#if HAS_I2C_DIGIPOT
    if (CmdArg->HasX)
    {
        StepperFd = 0;  //获取X轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromI2C(StepperFd, CmdArg->X);
        }
    }

    if (CmdArg->HasY)
    {
        StepperFd = 0;  //获取Y轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromI2C(StepperFd, CmdArg->Y);
        }
    }

    if (CmdArg->HasZ)
    {
        StepperFd = 0;  //获取Z轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromI2C(StepperFd, CmdArg->Z);
        }
    }

    if (CmdArg->HasE)
    {
        StepperFd = 0;  //获取E轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromI2C(StepperFd, CmdArg->E);
        }
    }

    if (CmdArg->HasB)
    {
        StepperFd = 0;  //获取E1轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromI2C(StepperFd, CmdArg->B);
        }
    }

    if (CmdArg->HasC)
    {
        StepperFd = 0;  //获取E2轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromI2C(StepperFd, CmdArg->C);
        }
    }

    if (CmdArg->HasD)
    {
        StepperFd = 0;  //获取E3轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromI2C(StepperFd, CmdArg->D);
        }
    }


#endif

#ifdef DAC_STEPPER_CURRENT
    if (CmdArg->HasS)
    {
        S32 i = 0;
        for ( i = 0; i < 4; i++ )   //0-4分别对应的轴电机是XYZE和E0
        {
            StepperFd = 0;  //获取轴电机的Fd
            if ( StepperFd )
            {
                SetCurrentFromDac(StepperFd, CmdArg->S);
            }
        }
    }

    if (CmdArg->HasX)
    {
        StepperFd = 0;  //获取X轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDac(StepperFd, CmdArg->X);
        }
    }

    if (CmdArg->HasY)
    {
        StepperFd = 0;  //获取Y轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDac(StepperFd, CmdArg->Y);
        }
    }

    if (CmdArg->HasZ)
    {
        StepperFd = 0;  //获取Z轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDac(StepperFd, CmdArg->Z);
        }
    }

    if (CmdArg->HasE)
    {
        StepperFd = 0;  //获取E轴电机的Fd
        if ( StepperFd )
        {
            SetCurrentFromDac(StepperFd, CmdArg->E);
        }
    }

#endif

    return 1;
}

S32 GcodeM907Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

