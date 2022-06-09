#include "GcodeG0Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrMotion/CrMotionPlanner.h"
#include "CrMotion/StepperApi.h"
#include "CrModel/CrMotion.h"

#define CMD_G0          "G0"

extern struct Coordinate3d_t CurCoordination;

extern U8 IsRelativeMode;

S32 GcodeG0Parse(const S8 *CmdBuff, VOID *Arg)
{
    /*G0 的操作有 E F X Y E */
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_G) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_G0) != 0 )  //传进来的命令不是G0命令
    {
        return 0;
    }

    S32 Offset = strlen(CMD_G0) + sizeof(S8); //加1是越过分隔符
    struct ArgG1AndG0_t *CmdArg = (struct ArgG1AndG0_t *)Arg;

    while ( Offset < Len )  //处理结束
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode G0 has no param opt = %s\n", CmdBuff);
        }
        
        switch ( Opt[0] )  //判断命令
        {
            case OPT_E:
            {
                CmdArg->HasE = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->E = atof(Opt + 1);
                }
                break;
            }
            case OPT_F:
            {
                CmdArg->HasF = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->F = atof(Opt + 1);
                }
                break;
            }
            case OPT_X:
            {
                CmdArg->HasX = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->X = atof(Opt + 1);
                }
                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->Y = atof(Opt + 1);
                }
                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->Z = atof(Opt + 1);
                }

                break;
            }
            default:
            {
                printf("Gcode G0 has no define opt = %s\n", CmdBuff);
                break;
            }
        }
        
        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }
    return 1;
}


S32 GcodeG0Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    //printf("G0 Cmd Argptr = %p\n", ArgPtr);
    
    if ( !ArgPtr || !MotionModel)
    {
        return 0;
    }
    struct MotionCoord_t CurCoord = MotionModel->GetLatestCoord();
    
    struct ArgG1AndG0_t *ParamG0 = (struct ArgG1AndG0_t *)ArgPtr;
    struct MotionGcode_t Target = {0};
    Target.FeedRate = 0.0f;
    
    if ( ParamG0->HasE )
    {
        if ( MotionModel->IsRelativeExtrudeMode() )
        {
            Target.Coord.CoorE = ParamG0->E + CurCoord.CoorE;
        }
        else
        {
            Target.Coord.CoorE = ParamG0->E;
        }
    }
    else
    {
        Target.Coord.CoorE = CurCoord.CoorE;
    }

    if ( ParamG0->HasF )
    {
        Target.FeedRate = ParamG0->F;
    }
    else
    {
        struct FeedrateAttr_t Attr = MotionModel->GetMotionFeedRateAttr();
        Target.FeedRate = Attr.Feedrate * 60;   // 速度设置为mm/min
    }
    
    if ( ParamG0->HasX )
    {
        if ( MotionModel->IsRelativeExtrudeMode() )
        {
            Target.Coord.CoorX = ParamG0->X + CurCoord.CoorX;
        }
        else
        {
            Target.Coord.CoorX = ParamG0->X;
        }
    }
    else
    {
        Target.Coord.CoorX = CurCoord.CoorX;
    }

    if ( ParamG0->HasY )
    {
        if ( MotionModel->IsRelativeExtrudeMode() )
        {
            Target.Coord.CoorY = ParamG0->Y + CurCoord.CoorY;
        }
        else
        {
            Target.Coord.CoorY = ParamG0->Y;
        }
    }
    else
    {
        Target.Coord.CoorY = CurCoord.CoorY;
        
    }

    if ( ParamG0->HasZ )
    {
        if ( MotionModel->IsRelativeExtrudeMode() )
        {
            Target.Coord.CoorZ = ParamG0->Z + CurCoord.CoorZ;
        }
        else
        {
            Target.Coord.CoorZ = ParamG0->Z;
        }
    }
    else
    {
        Target.Coord.CoorZ = CurCoord.CoorZ;
    }

    if (ParamG0->HasZ || ParamG0->HasX || ParamG0->HasY)
    {
        Target.RetractFlag = 0;
    }
    else
    {
        Target.RetractFlag = 1;
    }

    if (MotionModel->PutMotionCoord(Target) >= 0)
    //CrPlanCalc(&CurCoordination,&Target);
    {
        return GCODE_EXEC_OK;
    }

    return GCODE_EXEC_WAIT;
}

S32 GcodeG0Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


