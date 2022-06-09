/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     cx2470       the first version
 */

#include "GcodeM114Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrSleep.h"
#include "Common/Include/CrCommon.h"

#define CMD_M114  "M114"

S32 GcodeM114Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
     if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
         return 0;

     S8 Cmd[CMD_BUFF_LEN] = {0};
     if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
         return 0;

     if ( strcmp(Cmd, CMD_M114) != 0 )
         return 0;

     S32 Offset = strlen(CMD_M114) + sizeof(S8); //加1是越过分隔符
     struct ArgM114_t *CmdArg = (struct ArgM114_t *)Arg;

     while ( Offset < Len )  //处理结束
     {
         S8 Opt[20] = {0};
         if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
             break;

         S32 OptLen = strlen(Opt);
//         if ( OptLen < 1 )
//         {
//             rt_kprintf("Gcode M114 has no param opt = %s\n", CmdBuff);
//         }

         switch ( Opt[0] )  //判断命令
         {
             case OPT_D:
             {
                 CmdArg->HasD = true;
//                 if ( OptLen > 1 )
//                     CmdArg->D = atoi(Opt + 1);
                 break;
             }
             case OPT_E:
             {
                 CmdArg->HasE = true;
                 break;
             }
             case OPT_R:
             {
                 CmdArg->HasR = true;
                 break;
             }
             default:
             {
                 //*没有多个喷头*//
                 break;
             }
         }

         Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
     }

     return 1;
 }

S32 GcodeM114Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM114_t *CmdArg = (struct ArgM114_t *)ArgPtr;
    S8 BuffX[20] = {0}, BuffY[20] = {0}, BuffZ[20] = {0}, BuffE[20] = {0};
    if ( CmdArg->HasD )
    {
        while ( !MotionModel->IsMotionMoveFinished() )
        {
            CrM_Sleep(5);
        }

        //逻辑位置（计算空间偏移/原点偏移/位置偏移）
        //struct MotionPositionMsg_t Pos = MotionModel->GetCurPosition();
        //printf("X:%f Y:%f Z:%f E:%f ", Pos.PosX, Pos.PosY, Pos.PosZ, Pos.PosE);
        struct MotionCoord_t Coord1 =  MotionModel->GetLatestCoord();
        PrintFloat(Coord1.CoorX, BuffX);
        PrintFloat(Coord1.CoorY, BuffY);
        PrintFloat(Coord1.CoorZ, BuffZ);
        PrintFloat(Coord1.CoorE, BuffE);
        printf("X:%s Y:%s Z:%s E:%s ", BuffX, BuffY, BuffZ, BuffE);   //逻辑位置

        struct StepperPos_t PosX = {0};
        struct StepperPos_t PosY = {0};
        struct StepperPos_t PosZ = {0};
        StepperGetPos(&PosX,X);
        StepperGetPos(&PosY,Y);
        StepperGetPos(&PosZ,Z);

        printf("Count X:%d Y:%d Z:%d\n", PosX.CurPosition, PosY.CurPosition, PosZ.CurPosition);  //部署统计

        //详细信息
        // Gcode命令种的最新的点的坐标位置（逻辑位置）
        struct MotionCoord_t Coord =  MotionModel->GetLatestCoord();
        printf("\nLogical:");
        PrintFloat(Coord.CoorX, BuffX);
        PrintFloat(Coord.CoorY, BuffY);
        PrintFloat(Coord.CoorZ, BuffZ);
        PrintFloat(Coord.CoorE, BuffE);
        printf(" X:%s Y:%s Z:%s E:%s ", BuffX, BuffY, BuffZ, BuffE);

        // 在本机空间种坐标位置
        struct MotionPositionMsg_t Pos = MotionModel->GetCurPosition();
        PrintFloat(Pos.PosX, BuffX);
        PrintFloat(Pos.PosY, BuffY);
        PrintFloat(Pos.PosZ, BuffZ);
        PrintFloat(Pos.PosE, BuffE);
        printf("Raw:    X:%s Y:%s Z:%s E:%s\n", BuffX, BuffY, BuffZ, BuffE);

        struct MotionPositionMsg_t Level = Pos;

        //进行调平或Kinematics


        //让运动完成
        while ( !MotionModel->IsMotionMoveFinished() )
        {
            CrM_Sleep(5);
        }

        struct StepperPos_t PosX1 = {0};
        struct StepperPos_t PosY1 = {0};
        struct StepperPos_t PosZ1 = {0};
        struct StepperPos_t PosE1 = {0};
        StepperGetPos(&PosX1, X);
        StepperGetPos(&PosY1, Y);
        StepperGetPos(&PosZ1, Z);
        StepperGetPos(&PosE1, E);

        printf("Stepper: X:%d Y:%d Z:%d E:%d\n", PosX1.CurPosition, PosY1.CurPosition, PosZ1.CurPosition, PosE1.CurPosition);

        //获取实际的坐标
        struct MotionPositionMsg_t Pos2 = MotionModel->GetCurPosition();
        PrintFloat(Pos2.PosX, BuffX);
        PrintFloat(Pos2.PosY, BuffY);
        PrintFloat(Pos2.PosZ, BuffZ);
        PrintFloat(Pos2.PosE, BuffE);
        printf("FromStp: X:%.3f Y:%.3f Z:%.3f E:%.3f\n", BuffX, BuffY, BuffZ, BuffE);

        //距离
        PrintFloat((Pos2.PosX - Level.PosX), BuffX);
        PrintFloat((Pos2.PosY - Level.PosY), BuffY);
        PrintFloat((Pos2.PosZ - Level.PosZ), BuffZ);
        PrintFloat((Pos2.PosE - Level.PosE), BuffE);
        printf("Diff:   X:%s Y:%s Z:%s E:%s\n", BuffX, BuffY, BuffZ, BuffE);

    }
    else if ( CmdArg->HasE )
    {
        struct StepperPos_t Pos = {0};
        StepperGetPos(&Pos,E);
        printf("Count E:%d\n", Pos.CurPosition);
    }
    else if ( CmdArg->HasR )
    {
        //进行detla或scara转换成真实的坐标


        //将转换后的坐标内容处理完
        struct MotionPositionMsg_t Pos = MotionModel->GetCurPosition();
        PrintFloat(Pos.PosX, BuffX);
        PrintFloat(Pos.PosY, BuffY);
        PrintFloat(Pos.PosZ, BuffZ);
        PrintFloat(Pos.PosE, BuffE);
        printf("X:%s Y:%s Z:%s E:%s ", BuffX, BuffY, BuffZ, BuffE);
        struct StepperPos_t PosX = {0};
        struct StepperPos_t PosY = {0};
        struct StepperPos_t PosZ = {0};
        StepperGetPos(&PosX,X);
        StepperGetPos(&PosY,Y);
        StepperGetPos(&PosZ,Z);

        printf("Count X:%d Y:%d Z:%d\n", PosX.CurPosition, PosY.CurPosition, PosZ.CurPosition);  //步数统计

    }
    else
    {
        //报告当前位置
        while ( !MotionModel->IsMotionMoveFinished() )
        {
            CrM_Sleep(5);
        }

        //struct MotionPositionMsg_t Pos = MotionModel->GetCurPosition();
        //printf("X:%f Y:%f Z:%f E:%f ", Pos.PosX, Pos.PosY, Pos.PosZ);   //逻辑位置
        struct MotionCoord_t Coord =  MotionModel->GetLatestCoord();
        PrintFloat(Coord.CoorX, BuffX);
        PrintFloat(Coord.CoorY, BuffY);
        PrintFloat(Coord.CoorZ, BuffZ);
        PrintFloat(Coord.CoorE, BuffE);
        printf("X:%f Y:%f Z:%f E:%f ", BuffX, BuffY, BuffZ, BuffE);   //逻辑位置

        struct StepperPos_t PosX = {0};
        struct StepperPos_t PosY = {0};
        struct StepperPos_t PosZ = {0};
        StepperGetPos(&PosX,X);
        StepperGetPos(&PosY,Y);
        StepperGetPos(&PosZ,Z);

        printf("Count X:%d Y:%d Z:%d\n", PosX.CurPosition, PosY.CurPosition, PosZ.CurPosition);  //步数统计
    }

    return 1;
}

S32 GcodeM114Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
