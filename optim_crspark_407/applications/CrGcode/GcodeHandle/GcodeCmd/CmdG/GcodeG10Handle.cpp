#include "GcodeG10Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrMotion/CrMotionPlanner.h"
#include "CrMotion/StepperApi.h"
#include "CrModel/CrMotion.h"

#define CMD_G10          "G10"

S8 G10RetraFlag = 0;

S32 GcodeG10Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd, CMD_G10) != 0 )  //传进来的命令不是G0命令
    {
        return 0;
    }

    S32 Offset = strlen(CMD_G10) + sizeof(S8); //加1是越过分隔符
    struct ArgG10_t *CmdArg = (struct ArgG10_t *)Arg;

    while ( Offset < Len )  //处理结束
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        
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

        }
        
        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }
    return 1;
}

S32 GcodeG10Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !MotionModel || G10RetraFlag != 0 )
        return 0;

    struct MotionCoord_t CurCoord = MotionModel->GetLatestCoord();

    struct ArgG10_t *ParamG10 = (struct ArgG10_t *)ArgPtr;
    struct MotionGcode_t Target = {0};

    if ( ParamG10->HasS )
    {
        //FIXME : 多挤出机
    }

    struct FwRetract_t FwRatraSetting = MotionModel->GetFwRetractSetting();

//    if ( FwRatraSetting.FwRetractEnable )
//    {
    Target.RetractFlag = 1;

    Target.Coord.CoorE = CurCoord.CoorE - FwRatraSetting.RetractLength;

    if ( FwRatraSetting.RetractZ_Raise )
    {
        Target.Coord.CoorZ = CurCoord.CoorZ + FwRatraSetting.RetractZ_Raise;
        //FIXME :设置最大旅行加速度抬升Z
    }

    G10RetraFlag++;

    if (MotionModel->PutMotionCoord(Target) >= 0)
    {
        return GCODE_EXEC_OK;
    }
//    }

    return GCODE_EXEC_WAIT;
}

S32 GcodeG10Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


