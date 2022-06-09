#include "GcodeG53Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeHandle.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "Common/Include/CrEepromApi.h"
#include "CrModel/CrMotion.h"

#define CMD_G53  "G53"

S8 ActiveCoordinateSystem = -1; // machine space
struct XYZFloat_t PositionShift;
struct XYZFloat_t WorkspaceOffset;

void UpdateWorkspaceOffset(Axis_t Axis)
{
    switch(Axis)
    {
        case X:
            WorkspaceOffset.X = SettingData.HomeOffset.X + PositionShift.X;
            break;

        case Y:
            WorkspaceOffset.Y = SettingData.HomeOffset.Y + PositionShift.Y;
            break;

        case Z:
            WorkspaceOffset.Z = SettingData.HomeOffset.Z + PositionShift.Z;
            break;

        default:
            break;
    }
}

S8 GetActiveCoordinateSystem(void)
{
    return ActiveCoordinateSystem;
}

static VOID ReportCurrentPosition()
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

/**
 * Select a coordinate system and update the workspace offset.
 * System index -1 is used to specify machine-native.
 */
bool SelectCoordinateSystem(const S8 New)
{
    if ( ActiveCoordinateSystem == New )
    {
        return false;
    }

    ActiveCoordinateSystem = New;
    struct XYZFloat_t NewOffset = {0};
    if ( New >= 0 && New <= MAX_COORDINATE_SYSTEMS - 1 )
    {
        NewOffset.X = SettingData.CoordinateSystem[New].X;
        NewOffset.Y = SettingData.CoordinateSystem[New].Y;
        NewOffset.Z = SettingData.CoordinateSystem[New].Z;
    }

    if ( PositionShift.X != NewOffset.X )
    {
        PositionShift.X = NewOffset.X;
        UpdateWorkspaceOffset(X);
    }

    if ( PositionShift.Y != NewOffset.Y )
    {
        PositionShift.Y = NewOffset.Y;
        UpdateWorkspaceOffset(Y);
    }

    if ( PositionShift.Z != NewOffset.Z )
    {
        PositionShift.Z = NewOffset.Z;
        UpdateWorkspaceOffset(Z);
    }

    return true;
}

S32 GcodeG53Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_G53, strlen(CMD_G53)) != 0 )
    {
        return CrErr;
    }

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( !GetGcodeCmd(CmdBuff, Cmd) )  //获取命令失败
    {
        return CrErr;
    }

    S32 Offset = strlen(CMD_G53) + sizeof(S8); //加1是越过分隔符
    struct ArgG53_t *ArgPtr = (struct ArgG53_t *)Arg;
    if ( Offset >= Len )    //G53命令后可能会接其他的命令
    {
        ArgPtr->HasCmd = false;
    }
    else
    {
        ArgPtr->HasCmd = true;
        strcpy(ArgPtr->Cmd, CmdBuff + Offset);
    }

    return GCODE_EXEC_OK;
}

S32 GcodeG53Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return CrErr;
    }

    const S8 OldSystem = ActiveCoordinateSystem;
    SelectCoordinateSystem(-1);
#ifdef DEBUG_M53
    printf("Go to native space\n");
    ReportCurrentPosition();
#endif

    struct ArgG53_t *Arg = (struct ArgG53_t *)ArgPtr;
    if ( Arg->HasCmd )
    {
        //解析执行跟随在G53后面的Gcode命令
        S8 Replay[128] = {0};
        GcodeHandle(Arg->Cmd, strlen(Arg->Cmd), Replay);

        //从本机空间恢复到工作空间
        SelectCoordinateSystem(OldSystem);
    #ifdef DEBUG_M53
        printf("Go to native space\n");
        ReportCurrentPosition();
    #endif
    }

    return GCODE_EXEC_OK;
}

S32 GcodeG53Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

