#include "GcodeG27Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrMotion/StepperApi.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrSleep.h"

#define CMD_G27            "G27"

#define PARK_X_POS (X_AXIS_MIN_SIZE + 10)
#define PARK_Y_POS (Y_AXIS_MAX_SIZE - 10)
#define PARK_Z_POS (20)

#ifndef NOZZLE_PARK_XY_FEEDRATE
#define NOZZLE_PARK_XY_FEEDRATE  50    // (mm/s) X and Y axes feedrate (also used for delta Z axis)
#endif

#define NOZZLE_PARK_Z_FEEDRATE   5     // (mm/s) Z axis feedrate (not used for delta printers)

S32 GcodeG27Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_G) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_G27) != 0 )
        return 0;
    
    S32 Offset = strlen(CMD_G27) + sizeof(S8); //加1是越过分隔符
    struct ArgG27_t *CmdArg = (struct ArgG27_t *)Arg;

    while ( Offset < Len )  //处理结束
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
            break;    //获取参数失败

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
            printf("Gcode G27 has no param opt = %s\n", CmdBuff);

        switch ( Opt[0] )  //判断命令
        {
            case OPT_P:
             {
                 CmdArg->HasP = 1;
                 if ( OptLen > 1 )
                 {
                     CmdArg->P = atoi(Opt + 1);
                 }
                 break;
             }
        }
        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}

/**
 * Nozzle Park
 *
 * Park the nozzle at the given XYZ position on idle or G27.
 *
 * The "P" parameter controls the action applied to the Z axis:
 *
 *    P0  (Default) If Z is below park Z raise the nozzle.
 *    P1  Raise the nozzle always to Z-park height.
 *    P2  Raise the nozzle by Z-park amount, limited to Z_MAX_POS.
 */
S32 GcodeG27Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
        return 0;

    struct ArgG27_t *CmdArg = (struct ArgG27_t *)ArgPtr;

    struct MotionPositionMsg_t CurPosition = {0};

    CurPosition = MotionModel->GetCurPosition();
    /*Don't allow nozzle parking without homing first*/
    if ( !CurPosition.HomeX || !CurPosition.HomeY || !CurPosition.HomeZ )
        return 1;

    struct MotionCoord_t LatestCoord = MotionModel->GetLatestCoord();
    struct MotionGcode_t Target = {0};

    switch ( CmdArg->P )
    {
        case 1:
        {
            Target.Coord.CoorZ = PARK_Z_POS;
            break;
        }
        case 2:
        {
            Target.Coord.CoorZ = ((Y_AXIS_MAX_SIZE - LatestCoord.CoorZ) > PARK_Z_POS) \
                                 ? (LatestCoord.CoorZ + PARK_Z_POS) : LatestCoord.CoorZ;
            break;
        }
        default:
        {
            if ( LatestCoord.CoorZ < PARK_Z_POS )
                 Target.Coord.CoorZ = PARK_Z_POS;
            break;
        }
    }

    Target.FeedRate = NOZZLE_PARK_Z_FEEDRATE * 60;  // 5mm/s
    Target.Coord.CoorX = LatestCoord.CoorX;
    Target.Coord.CoorY = LatestCoord.CoorY;
    Target.Coord.CoorE = LatestCoord.CoorE;
    MotionModel->PutMotionCoord(Target);

    LatestCoord = MotionModel->GetLatestCoord();

    Target.FeedRate = NOZZLE_PARK_XY_FEEDRATE * 60;  // 50mm/s
    Target.Coord.CoorZ = LatestCoord.CoorZ;
    Target.Coord.CoorX = PARK_X_POS;
    Target.Coord.CoorY = PARK_Y_POS;
    Target.Coord.CoorE = LatestCoord.CoorE;
    MotionModel->PutMotionCoord(Target);

    return GCODE_EXEC_OK;
}

S32 GcodeG27Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


