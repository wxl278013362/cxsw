#include "GcodeM200Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrInc/CrConfig.h"

#define CMD_M200 "M200"
#ifndef M_PI
  #define M_PI 3.14159265358979323846f
#endif

#define sq(x) ((x)*(x))
#define CIRCLE_AREA(R) (float(M_PI) * sq(float(R)))
#define CIRCLE_CIRC(R) (2 * float(M_PI) * float(R))

static CrMotion *Motion = NULL;

VOID GcodeM200Init(VOID *MotionModel)
{
    Motion = (CrMotion *)MotionModel;
}


S32 GcodeM200Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M200
    if ( strcmp(Cmd, CMD_M200) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M200) + sizeof(S8);
    struct ArgM200_t *CmdArg = (struct ArgM200_t *)Arg;

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
            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                    CmdArg->S = atoi(Opt + 1);
                else
                    CmdArg->S = 0;
                break;

            case OPT_D:
                CmdArg->HasD = true;
                if ( OptLen > 1 )
                    CmdArg->D = atof(Opt + 1);
                else
                    CmdArg->D = 0.0f;
                break;

            case OPT_L:
                CmdArg->HasL = true;
                if ( OptLen > 1 )
                    CmdArg->L = atof(Opt + 1);
                else
                    CmdArg->L = 0.0f;
                break;

            case OPT_T:
                CmdArg->HasT = true;
                if ( OptLen > 1 )
                    CmdArg->T = atoi(Opt + 1);
                else
                    CmdArg->T = -1;
                break;

            default:
                printf("Warn:unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
 }


S32 GcodeM200Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM200_t *Param = (struct ArgM200_t *)ArgPtr;
    S32 Index = -1;
    if ( Param->HasT )
    {
        Index = Param->T;
    }

    if ( Index < 0 )  //设置当前挤出机的容积模式
    {

    }
    else   //设置给定挤出机的容积模式
    {

    }

    BOOL EnableVolume = MotionModel->IsVolumePatternEnable();
    if ( Param->HasD )
    {
        if( Param->D )
        {
            MotionModel->SetDiameter(Param->D);
        }
        else
        {
            EnableVolume = 0;
        }
    }

    if ( Param->HasL )
    {
        //设置limit（设置E轴的最大速度）
        if ( Param->L )
        {
            //转换为长度
            FLOAT Diameter = MotionModel->GetDiameter();
            FLOAT Area = CIRCLE_AREA(Diameter * 0.5);
            FLOAT Lenght = Param->L / Area;   //1秒钟挤出的最大长度
            if ( Lenght > VELOCITY_V_MAX_X )
                Lenght = VELOCITY_V_MAX_X;

            struct StepperMotionAttr_t Attr = MotionModel->GetSetpperMotion(StepperE);
            Attr.MaxVelocity = Lenght;
            MotionModel->SetStepperMotion(Attr, StepperE);
        }
        else
        {
            EnableVolume = 0;
        }
    }

    if ( Param->HasS )
    {
        if ( Param->S )
            EnableVolume = 1;
        else
            EnableVolume = 0;
    }

    MotionModel->SetVolumePattern(EnableVolume);

    return 1;
}

S32 GcodeM200Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

