#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "GcodeM851Handle.h"
#include "CrInc/CrMsgType.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "CrMotion/CrMotionPlanner.h"
#include "Common/Include/CrGlobal.h"
#include "Common/Include/CrTime.h"
#include "Common/Include/CrEepromApi.h"
#include "CrMotion/CrStepperRun.h"


#define CMD_M852  "M852"

/******额外定义的变量和函数***********/
#define SKEW_FACTOR_MIN -1
#define SKEW_FACTOR_MAX 1

SkewFactor_t PlannerSkewfactor;
XYZFloat_t Cartes = {0};
XYZEFloat_t CurrentPosition = {0};
#if HAS_POSITION_MODIFIERS
VOID UnapplyModifiers()
{
    //在该函数中，去掉倾斜因子、Z轴的hop、自动调平三个因素导致的位置修改
}
#endif

struct XYZFloat_t GetCartesianFromStepper()  //应该由电机类提供函数，目前只写接口
{
    if ( !MotionModel )
    {
        return {0,0,0};
    }

    struct StepperPos_t StepperPos[3];
    struct StepperAttr_t StepperAttr[3];
    for ( int i = 0; i < 3; i++ )
    {
        StepperGetPos(&StepperPos[i],(enum Axis_t)i);
        StepperGetAttr(&StepperAttr[i],(enum Axis_t)i);
    }
    Cartes.X = StepperPos[0].CurPosition / (1/StepperAttr[0].StepsPerMm);
    Cartes.Y = StepperPos[1].CurPosition / (1/StepperAttr[1].StepsPerMm);
    Cartes.Z = StepperPos[2].CurPosition / (1/StepperAttr[2].StepsPerMm);

    return Cartes;
}

//CurrentPosition Used to track the native machine position as moves are queued.
//CurrentPosition Used by 'line_to_current_position' to do a move after changing it.
//CurrentPosition Used by 'sync_plan_position' to update 'planner.position'.
VOID SetCurrentFromStepperForAxis(const Axis_t Axis)
{
    GetCartesianFromStepper();
    XYZEFloat_t Pos;
    Pos.X = Cartes.X;
    Pos.Y = Cartes.Y;
    Pos.Z = Cartes.Z;
    if ( MotionModel )
    {
        Pos.E = MotionModel->GetCurPosition().PosE;
    }

#if HAS_POSITION_MODIFIERS
    UnapplyModifiers();
#endif
    if ( Axis == ALL_AXES)
    {
        CurrentPosition = Pos;
    }
    else
    {
        switch(Axis)
        {
            case X:
            {
                CurrentPosition.X = Pos.X;
                break;
            }
            case Y:
            {
                CurrentPosition.Y = Pos.Y;
                break;
            }
            case Z:
            {
                CurrentPosition.Z = Pos.Z;
                break;
            }
            case E:
            {
                CurrentPosition.E = Pos.E;
                break;
            }
            default:
                break;
        }
    }

}

VOID SyncPlanPositon()  //算出电机实际位置
{
    XYZEFloat_t MachinePosition = {0};
    struct StepperAttr_t StepperAttr[3];
    for ( int i = 0; i < 3; i++ )
    {
        StepperGetAttr(&StepperAttr[i],(enum Axis_t)i);
    }
    MachinePosition.X = CurrentPosition.X * StepperAttr[0].StepsPerMm;
    MachinePosition.Y = CurrentPosition.Y * StepperAttr[1].StepsPerMm;
    MachinePosition.Z = CurrentPosition.Z * StepperAttr[2].StepsPerMm;
    MachinePosition.E = CurrentPosition.E * StepperAttr[3].StepsPerMm;
}

VOID ReportCurrentPosition()
{
    struct MotionPositionMsg_t Pos = {0};
    if ( !MotionModel )
    {
        return ;
    }
    Pos = MotionModel->GetCurPosition();
    //报告本地坐标（加上偏移值的坐标）
    printf("X: %f Y: %f Z: %f E: %f",Pos.PosX,Pos.PosY,Pos.PosZ,Pos.PosE);//未加上偏移值

    //报告当前坐标
    printf("X: %f Y: %f Z: %f E: %f",Pos.PosX,Pos.PosY,Pos.PosZ,Pos.PosE);

}
/******额外定义的变量和函数***********/

S32 GcodeM852Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd,CMD_M852) != 0 )    //判断是否是M852命令
    {
        return 0;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M852) + sizeof(S8);
    struct ArgM852_t *CmdArg = (struct ArgM852_t *)Arg;

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
            printf("Gcode M852 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_I:
            {
                CmdArg->HasI = true;
                if ( OptLen > 1 )
                {
                    CmdArg->I = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->I = 0.0f;
                }
                break;
            }
            case OPT_J:
            {
                CmdArg->HasJ = true;
                if ( OptLen > 1 )
                {
                    CmdArg->J = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->J = 0.0f;
                }
                break;
            }
            case OPT_K:
            {
                CmdArg->HasK = true;
                if ( OptLen > 1 )
                {
                    CmdArg->K = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->K = 0.0f;
                }
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                {
                    CmdArg->S = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->S = 0.0f;
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

S32 GcodeM852Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    U8 IJK = 0;
    U8 Badval = 0;
    U8 Setval = 0;

    struct ArgM852_t *Param = (struct ArgM852_t *)ArgPtr;
    if ( Param->HasI || Param->HasS )
    {
        ++IJK;
        FLOAT Value = 0.0f;
        if ( Param->HasI )
        {
            Value = Param->I;
        }
        else if ( Param->HasS )
        {
            Value = Param->S;
        }

        if ( Value >= SKEW_FACTOR_MIN && Value <= SKEW_FACTOR_MAX )
        {
            if ( PlannerSkewfactor.XY != Value )
            {
                PlannerSkewfactor.XY = Value;
                ++Setval;
            }
        }
        else
        {
            ++Badval;
        }
    }

#ifdef SKEW_CORRECTION_FOR_Z
    if ( Param->HasJ )
    {
        ++IJK;
        FLOAT Value = Param->J;
        if ( Value >= SKEW_FACTOR_MIN && Value <= SKEW_FACTOR_MAX )
        {
            if ( PlannerSkewfactor.XZ != Value )
            {
                PlannerSkewfactor.XZ = Value;
                ++Setval;
            }
        }
        else
        {
            ++Badval;
        }
    }

    if ( Param->HasK )
    {
        ++IJK;
        FLOAT Value = Param->K;
        if ( Value >= SKEW_FACTOR_MIN && Value <= SKEW_FACTOR_MAX )
        {
            if ( PlannerSkewfactor.YZ != Value )
            {
                PlannerSkewfactor.YZ = Value;
                ++Setval;
            }
        }
        else
        {
            ++Badval;
        }
    }
#endif

    //Badval有值
    if ( Badval )
    {
        printf("min_skew_factor: %d max_skew_factor: %d",SKEW_FACTOR_MIN,SKEW_FACTOR_MAX);
    }

    //SetVal有值说明发生了改变
    if ( Setval )
    {
        SetCurrentFromStepperForAxis(ALL_AXES);
        SyncPlanPositon();
        ReportCurrentPosition();
    }

    //输出改变后的PlannerSkewfactor值
    if ( !IJK )
    {
#ifdef SKEW_CORRECTION_FOR_Z
        printf("echo:Skew Factor XY: %.6f XZ: %.6f YZ: %.6f\n.",PlannerSkewfactor.XY,PlannerSkewfactor.XZ,PlannerSkewfactor.YZ);
#endif
        printf("echo:Skew Factor XY: %.6f\n.",PlannerSkewfactor.XY);
    }
    return 1;
}

S32 GcodeM852Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
