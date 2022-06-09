/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     cx2470       the first version
 */

#include "GcodeM125Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <rtthread.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrSleep.h"
#include "CrModel/CrPrinter.h"
#include "CrModel/CrGcode.h"
#include "CrModel/CrHeaterManager.h"

#define CMD_M125  "M125"
BOOL NozParkPaused = false;


static VOID WaitTemperRecover()
{
    if ( !BedHeater )
        return ;

    struct HeaterManagerAttr_t BedAttr = BedHeater->CrHeaterMagagerGetAttr();
    BedAttr.TargetTemper = Printer->GetBedTemp();
    /*Set target Temperature of Bed*/
    BedHeater->CrHeaterMagagerSetAttr(BedAttr);

    if ( GcodeModel )
        GcodeModel->StartWaiting();

    if ( (HOTENDS > 1) && (HotEndArr[0])  )
    {
        struct HeaterManagerAttr_t NozAttr = HotEndArr[0]->CrHeaterMagagerGetAttr();
        NozAttr.TargetTemper = Printer->GetNozTemp();
        /*Set target Temperature of Nozzle*/
        HotEndArr[0]->CrHeaterMagagerSetAttr(NozAttr);

        while ( 1 )
        {
            if ( GcodeModel )
                if( !GcodeModel->IsWaiting() )
                    break;

            if ( HotEndArr[0]->ReachedTargetTemper() )
            {
                S32 i = 10;
                do
                {
                    if ( GcodeModel )
                        if( !GcodeModel->IsWaiting() )
                            break;

                    rt_thread_delay(300);

                }while ( i-- && HotEndArr[0]->ReachedTargetTemper() );

                if (i <= 0 )
                    break;
            }
            rt_thread_delay(500);
        }

    }

    rt_kprintf("M24 stop waiting heatting");

    if ( GcodeModel )
        GcodeModel->StopWaiting();
}

S32 GcodeM125Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
     if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
         return 0;

     S8 Cmd[CMD_BUFF_LEN] = {0};
     if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
         return 0;

     if ( strcmp(Cmd, CMD_M125) != 0 )
         return 0;

     S32 Offset = strlen(CMD_M125) + sizeof(S8); //加1是越过分隔符
     struct ArgM125_t *CmdArg = (struct ArgM125_t *)Arg;

     while ( Offset < Len )  //处理结束
     {
         S8 Opt[20] = {0};
         if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
             break;

         S32 OptLen = strlen(Opt);
         if ( OptLen <= 1 )
         {
             rt_kprintf("Gcode M105 has no param opt = %s\n", CmdBuff);
         }

         switch ( Opt[0] )  //判断命令
         {
             case OPT_P:
             {
                 CmdArg->HasP = true;
                 if ( OptLen > 1 )
                     CmdArg->P = atoi(Opt + 1);
                 break;
             }
             case OPT_X:
             {
                 CmdArg->HasX = true;
                 if ( OptLen > 1 )
                     CmdArg->X = atof(Opt + 1);
                 break;
             }
             case OPT_Y:
             {
                 CmdArg->HasY = true;
                 if ( OptLen > 1 )
                     CmdArg->Y = atof(Opt + 1);
                 break;
             }
             case OPT_Z:
             {
                 CmdArg->HasZ = true;
                 if ( OptLen > 1 )
                     CmdArg->Z = atof(Opt + 1);
                 break;
             }
             case OPT_L:
             {
                 CmdArg->HasL = true;
                 if ( OptLen > 1 )
                     CmdArg->L = atof(Opt + 1);
                 break;
             }
             default:
             {
                 //*没有多个喷头*//
                 rt_kprintf("Gcode M105 Get All Temp = %s\n", CmdBuff);
                 break;
             }
         }

         Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
     }

     return 1;
 }

S32 GcodeM125Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct MotionCoord_t ParkCoor = NOZZLE_PARK_POINT;
    struct ArgM125_t *ParamM125 = (struct ArgM125_t *)ArgPtr;
    struct MotionCoord_t Coor = MotionModel->GetLatestCoord();
//    struct MotionCoord_t RecoveryCoor = Coor;
    if ( ParamM125->HasL )
    {
        Coor.CoorE = Coor.CoorE - ParamM125->L;
    }
    else
    {
        Coor.CoorE = Coor.CoorE - ParkCoor.CoorE;
    }

    if ( ParamM125->HasX )
    {
        Coor.CoorX = ParamM125->X;
    }
    else
    {
        Coor.CoorX = ParkCoor.CoorX;
    }

    if ( ParamM125->HasY )
    {
        Coor.CoorY = ParamM125->Y;
    }
    else
    {
        Coor.CoorY = ParkCoor.CoorY;
    }

    if ( ParamM125->HasZ )
    {
        Coor.CoorZ = Coor.CoorZ + ParamM125->Z;
    }
    else
    {
        Coor.CoorZ = Coor.CoorZ + ParkCoor.CoorZ;
    }

    MotionModel->MotionPause();   //需要修改暂停运动接口
    Printer->PauseHandle();

    //恢复的内容暂时未实现
    if ( ParamM125->HasP )
    {
        if ( ParamM125->P )
        {
            NozParkPaused = 1;
            //等待恢复
            while (NozParkPaused)
            {
                CrM_Sleep(5);
            }

            //执行恢复动作
            Printer->ResumeHandle();
            WaitTemperRecover();
            MotionModel->MotionResume();
        }
    }

    return 1;
}

S32 GcodeM125Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
