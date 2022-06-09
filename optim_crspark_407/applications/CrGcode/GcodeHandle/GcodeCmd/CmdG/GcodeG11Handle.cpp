#include "GcodeG11Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrMotion/CrMotionPlanner.h"
#include "CrMotion/StepperApi.h"
#include "CrModel/CrMotion.h"

#define CMD_G11            "G11"

extern S8 G10RetraFlag;


S32 GcodeG11Parse(const S8 *CmdBuff, VOID *Arg)
{
    /*G1 的操作有 E F X Y E */
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_G) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_G11) != 0 )  //传进来的命令不是G1命令
        return 0;

    return 1;
}

S32 GcodeG11Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !MotionModel || G10RetraFlag  != 1 )
        return 0;
    
    struct MotionGcode_t Target = {0};
    struct MotionCoord_t CurCoord = MotionModel->GetLatestCoord();

    struct FwRetract_t FwRatraSetting = MotionModel->GetFwRetractSetting();

//    if ( FwRatraSetting.FwRetractEnable )
//    {
    Target.RetractFlag = 1;
    Target.Coord.CoorE = CurCoord.CoorE + FwRatraSetting.RetractLength;

    if ( FwRatraSetting.RetractZ_Raise )
    {
        Target.Coord.CoorZ = CurCoord.CoorZ - FwRatraSetting.RetractZ_Raise;
        //FIXME :设置最大旅行加速度抬升Z
    }

    G10RetraFlag--;

    if ( MotionModel->PutMotionCoord(Target) >= 0 )
    {
        return GCODE_EXEC_OK;
    }

    return GCODE_EXEC_WAIT;
}

S32 GcodeG11Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


