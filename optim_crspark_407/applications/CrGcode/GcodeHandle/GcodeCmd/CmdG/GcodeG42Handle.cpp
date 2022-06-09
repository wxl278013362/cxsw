#include "GcodeG42Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrSleep.h"

#define CMD_G42  "G42"

// 调平的点数
#define GRID_MAX_POINTS_X (3)
#define GRID_MAX_POINTS_Y GRID_MAX_POINTS_X

#define STR_ERR_MESH_XY "Mesh point out of range"

#define BED_X_SIZE  200.0
#define BED_Y_SIZE  200

#define IS_SCARA 1

FLOAT GetMeshX(S32 Index)
{
    if ( Index < 0 || Index >= GRID_MAX_POINTS_X )
        return 0;

    if ( GRID_MAX_POINTS_X <= 0 )
        return 0;


    return Index * (((FLOAT)BED_X_SIZE) /GRID_MAX_POINTS_X );
}

FLOAT GetMeshY(S32 Index)
{
    if ( Index < 0 || Index >= GRID_MAX_POINTS_Y )
        return 0;

    if ( GRID_MAX_POINTS_Y <= 0 )
        return 0;


    return Index * (((FLOAT)BED_Y_SIZE) /GRID_MAX_POINTS_Y );
}

S32 GcodeG42Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_G) )   //Gcode命令的长度大于1,且是M
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_G42) != 0 )  //传进来的命令不是M18或M84命令
    {
        return 0;
    }

    S32 Offset = strlen(CMD_G42) + sizeof(S8); //加1是越过分隔符
    struct ArgG42_t *CmdArg = (struct ArgG42_t *)Arg;

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
            case OPT_F:
            {
                CmdArg->HasF = true;
                if ( OptLen > 1 ) CmdArg->F = atof( Opt + 1 );
                break;
            }
            case OPT_I:
            {
                CmdArg->HasI = true;
                if ( OptLen > 1 ) CmdArg->I = atoi( Opt + 1 );

                break;
            }
            case OPT_J:
            {
                CmdArg->HasJ = true;
                if ( OptLen > 1 ) CmdArg->J = atoi( Opt + 1 );

                break;
            }
            default:
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    if ( !(CmdArg->HasI) )
    {
        CmdArg->HasI = true;
        CmdArg->I = 0;
    }

    if ( !(CmdArg->HasJ) )
    {
        CmdArg->HasJ = true;
        CmdArg->J = 0;
    }

    return 1;
}

S32 GcodeG42Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgG42_t *Arg = (struct ArgG42_t *)ArgPtr;

    if ( !(Arg->HasI) || ((Arg->I < 0) && (Arg->I >= GRID_MAX_POINTS_X)) )
    {
        printf("%s\n", STR_ERR_MESH_XY);
        return 1;
    }

    if ( !(Arg->HasJ) || ((Arg->J < 0) && (Arg->J >= GRID_MAX_POINTS_Y)) )
    {
        printf("%s\n", STR_ERR_MESH_XY);
        return 1;
    }

    struct MotionPositionMsg_t CurPos = MotionModel->GetCurPosition();
    struct MotionCoord_t DestCoor;
    DestCoor.CoorX = CurPos.PosX;
    DestCoor.CoorY = CurPos.PosY;
    DestCoor.CoorZ = CurPos.PosZ;
    DestCoor.CoorE = CurPos.PosE;
    if ( Arg->HasI )
    {
        DestCoor.CoorX = GetMeshX(Arg->I);
    }

    if ( Arg->HasJ )
    {
        DestCoor.CoorY = GetMeshY(Arg->J);
    }

    //此部分的内容暂时没有
//    #if HAS_PROBE_XY_OFFSET
//      if (parser.boolval('P')) {
//        if (hasI) destination.x -= probe.offset_xy.x;
//        if (hasJ) destination.y -= probe.offset_xy.y;
//      }
//    #endif

    //设置速度(将速度转换为mm/s)
    struct FeedrateAttr_t Feedrate = MotionModel->GetMotionFeedRateAttr();
    FLOAT OldFr = Feedrate.Feedrate;
    const S16 OldPct = Feedrate.FeedrateRatio;

    #if IS_SCARA
        Feedrate.FeedrateRatio = 100;
    #endif

    if ( Arg->HasF )
    {
        if(Arg->F > 0)
            Feedrate.Feedrate = Arg->F;

    }

    MotionModel->SetMotionFeedRateAttr(Feedrate);

    //加入到运动队列
    MotionModel->SetLatestCoord(DestCoor);

    //
    CrM_Sleep(1000);

    //恢复原来的打印速率
    Feedrate.Feedrate = OldFr;
    Feedrate.FeedrateRatio = OldPct;
    MotionModel->SetMotionFeedRateAttr(Feedrate);

    return 1;
}

S32 GcodeG42Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
