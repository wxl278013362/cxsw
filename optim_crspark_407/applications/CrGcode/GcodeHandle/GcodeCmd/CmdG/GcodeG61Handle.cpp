#include "GcodeG61Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrModel/CrMotion.h"
#include "CrMotion/CrMotionPlanner.h"
#include "CrMotion/StepperApi.h"
#include "Common/Include/CrGlobal.h"

#define CMD_G61          "G61"

S32 GcodeG61Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd, CMD_G61) != 0 )  //传进来的命令不是G0命令
    {
        return 0;
    }

    S32 Offset = strlen(CMD_G61) + sizeof(S8); //加1是越过分隔符
    struct ArgG61_t *CmdArg = (struct ArgG61_t *)Arg;

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
            case OPT_S:
            {
                CmdArg->HasS = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->S = atoi(Opt + 1);
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
                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = 1;
                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = 1;
                break;
            }
            default:
            {
                printf("Gcode G61 has no define opt = %s\n", CmdBuff);
                break;
            }
        }
        
        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }
    return 1;
}


S32 GcodeG61Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !MotionModel)
        return 0;

    rt_kprintf("%s, MotionResume\n", __FUNCTION__);
    struct ArgG61_t *CmdArg = (struct ArgG61_t *)ArgPtr;
    if ( (CmdArg->HasX == 0) && (CmdArg->HasY == 0) && (CmdArg->HasZ == 0) )
        return 0;

    S32 Index = 0;
    if ( CmdArg->HasS)
        Index = CmdArg->S;

    //判断index的范围是否合法
    struct MotionCoord_t Cood = MotionModel->GetLatestCoord();      //获取当前的运动坐标
#ifdef SLOT_COUNT
    if ( Index >= SLOT_COUNT )
        return 1;

    struct MotionCoord_t TempCood = StoredPos[Index];  //获取保存的坐标
    if ( CmdArg->HasX )
        Cood.CoorX = TempCood.CoorX;

    if ( CmdArg->HasY )
        Cood.CoorY = TempCood.CoorY;

    if ( CmdArg->HasZ )
        Cood.CoorZ = TempCood.CoorZ;
#endif

#if 0
    BOOL Relative = 0, RelativeE = 0;
    if ( MotionModel->IsRelativeExtrudeMode() ) //是否为相对模式
    {
        RelativeE = 1;
    }

    if ( MotionModel->IsRelativeMotionMode() )  //是否为相对模式
        Relative = 1;

    if ( Relative )
        MotionModel->SetAbsluteMotion();
#endif

    //设置进给速率
    if ( CmdArg->HasF )
    {
        struct FeedrateAttr_t Feedrate = MotionModel->GetMotionFeedRateAttr();
        Feedrate.Feedrate = CmdArg->F;
        MotionModel->SetMotionFeedRateAttr(Feedrate);
    }

    //设置目的坐标
    struct MotionGcode_t Coord = {0};
    Coord.Coord = Cood;
    MotionModel->PutMotionCoord(Coord);

#if 0
    if ( Relative )
        MotionModel->SetRelativeMotion();

    if ( RelativeE )
    {
        MotionModel->SetRelativeExtrude();
    }
    else
    {
        MotionModel->SetAbsluteExtrude();
    }
#endif

    return 1;
}

S32 GcodeG61Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


