#include "GcodeG92Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrMotion/StepperApi.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrSleep.h"
#include "Common/Include/CrCommon.h"

#define CMD_G92            "G92"

S32 GcodeG92Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_G) )   //Gcode命令的长度大于1,且是G
        return 0;

    //
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strncmp(Cmd, CMD_G92, strlen(CMD_G92)) != 0 )  //传进来的命令不是G92命令
    {
        printf("current cmd is not G92 cmd , is : %s\n", CmdBuff);
        return 0;
    }
    
    S32 Offset = strlen(CMD_G92);
    struct ArgG92_t *CmdArg = (struct ArgG92_t *)Arg;
    if ( Offset < strlen(Cmd) )
    {
        if ( CmdBuff[Offset + 1] == '0' )
        {
            CmdArg->SubCode = 0;
        }
        else if ( CmdBuff[Offset + 1] == '1' )
        {
            CmdArg->SubCode = 1;
        }
        else if (CmdBuff[Offset + 1] == '9')
        {
            CmdArg->SubCode = 9;
        }
        else
        {
            CmdArg->SubCode = 0;
        }
        Offset = strlen(Cmd);
    }
    else
    {
        CmdArg->SubCode = 0;
    }

    Offset += sizeof(S8); //加1是越过分隔符

    
    S8 Opt[20] = {0};
    S32 OptLen = 0;
    
    while (Offset < Len)
    {
        memset(Opt,0,sizeof(Opt));
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            //printf(" Cmd get opt failed  cmd = %s\n", CmdBuff);
            break;    //获取参数失败
        }

        OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode G92 has no param opt = %s\n", Opt);
        }
        switch (Opt[0])
        {
            case OPT_E:
                
                CmdArg->HasE = 1;
                
                if (OptLen > 1)
                {
                    CmdArg->E = atof(Opt + 1);
                }

            break;
            case OPT_X:

                CmdArg->HasX = 1;
                if (OptLen > 1)
                {
                    CmdArg->X = atof(Opt + 1);
                }
                
            break;
            case OPT_Y:
                
                CmdArg->HasY = 1;
                
                if (OptLen > 1)
                {
                    CmdArg->Y = atof(Opt + 1);
                }
                
            break;
            case OPT_Z:

                CmdArg->HasZ = 1;

                if (OptLen > 1)
                {
                    CmdArg->Z = atof(Opt + 1);
                }
            break;
            default:
            {
                printf("Gcode G92 has no define opt = %s\n", Opt);
                break;
            }
        }
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}
extern struct Coordinate3d_t CurCoordination;
S32 GcodeG92Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgG92_t *Param = (struct ArgG92_t *)ArgPtr;
    //设置工作空间的原点   （缺少位置偏移，原点偏移，工作空间和本机空间的区分）
    if ( Param->SubCode == 0 )
    {

    }
    else if ( Param->SubCode == 1 )
    {

    }
    else if (Param->SubCode == 9)
    {

    }

    struct MotionCoord_t Coord = {0};

    struct MotionCoord_t CurCoor = MotionModel->GetLatestCoord();
    if( Param->HasX )
    {
        Coord.CoorX = Param->X;
    }
    else
    {
        Coord.CoorX = CurCoor.CoorX;
    }
    if( Param->HasY )
    {
        Coord.CoorY = Param->Y;
    }
    else
    {
        Coord.CoorY = CurCoor.CoorY;
    }
    if( Param->HasZ )
    {
        Coord.CoorZ = Param->Z;
    }
    else
    {
        Coord.CoorZ = CurCoor.CoorZ;
    }
    if( Param->HasE )
    {
        S32 Pos = Param->E;
        S32 Fd = MotionModel->GetStepperFd(StepperE);
        CrStepperIoctl(Fd, CRSTEPPER_SET_POS, &Pos);
        Coord.CoorE = Param->E;
//        Coord.CoorE = CurCoor.CoorE - Param->E;
//        MotionModel->SetRelativeExtrude();  //
    }
    else
    {
        Coord.CoorE = CurCoor.CoorE;
    }
    
    /* waitting for motion finish.*/
//    while ( !MotionModel->IsMotionMoveFinished() )
//    {
//        CrM_Sleep(5);
//    }

    MotionModel->SetLatestCoord(Coord);

    //逻辑位置（计算空间偏移/原点偏移/位置偏移）
    struct MotionCoord_t Coord1 =  MotionModel->GetLatestCoord();
    S8 Data[20] = {0};
    PrintFloat(Coord1.CoorX, Data);
    printf("X:%s ", Data);
    memset(Data, 0, sizeof(Data));
    PrintFloat(Coord1.CoorY, Data);
    printf("Y:%s ", Data);
    memset(Data, 0, sizeof(Data));
    PrintFloat(Coord1.CoorZ, Data);
    printf("Z:%s ", Data);
    memset(Data, 0, sizeof(Data));
    PrintFloat(Coord1.CoorE, Data);
    printf("E:%s ", Data);  //逻辑位置

    struct StepperPos_t PosX = {0};
    struct StepperPos_t PosY = {0};
    struct StepperPos_t PosZ = {0};
    StepperGetPos(&PosX,X);
    StepperGetPos(&PosY,Y);
    StepperGetPos(&PosZ,Z);

    printf("Count X:%d Y:%d Z:%d\n", PosX.CurPosition, PosY.CurPosition, PosZ.CurPosition);  //步数统计

    return 1;
}

S32 GcodeG92Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
