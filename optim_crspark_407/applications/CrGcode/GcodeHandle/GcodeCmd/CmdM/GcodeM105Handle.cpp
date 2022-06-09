/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     cx2470       the first version
 */

#include "GcodeM105Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rtthread.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrHeaterManager.h"
#include "Common/Include/CrCommon.h"

#define CMD_M105  "M105"

S32 GcodeM105Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
     if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
         return 0;

     S8 Cmd[CMD_BUFF_LEN] = {0};
     if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
         return 0;

     if ( strcmp(Cmd, CMD_M105) != 0 )
         return 0;

     S32 Offset = strlen(CMD_M105) + sizeof(S8); //加1是越过分隔符
     struct ArgM105_t *CmdArg = (struct ArgM105_t *)Arg;

     while ( Offset < Len )  //处理结束
     {
         S8 Opt[20] = {0};
         if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
             break;

         S32 OptLen = strlen(Opt);
         if ( OptLen <= 1 )
         {
             //rt_kprintf("Gcode M105 has no param opt = %s\n", CmdBuff);
         }

         switch ( Opt[0] )  //判断命令
         {
             case OPT_T:
             {
                 CmdArg->HasT = true;
                 if ( OptLen > 1 )
                     CmdArg->T = atoi(Opt + 1);
                 break;
             }

             default:
             {
                 //*没有多个喷头*//
                 //rt_kprintf("Gcode M105 Get All Temp = %s\n", CmdBuff);
                 break;
             }
         }

         Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
     }

     return 1;
 }

S32 GcodeM105Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM105_t *ParamM105 = (struct ArgM105_t *)ArgPtr;
//    if ( !ParamM105->HasT )
//        return 1;
    if ( !ParamM105->HasT )
    {
        ParamM105->T = 0;   //应该设置成活动挤出机索引
    }

    if ( (HOTENDS <= 0) || (ParamM105->T < 0) || (ParamM105->T > (HOTENDS)) )  //注意T==HOTENDS时表示bed heater
        return 1;

    BOOL HasHeater = false;
    if ( BedHeater )
    {
        HasHeater = true;
    }

    //S8 i = 0;   //warning: array subscript has type 'char'
    S32 i = 0;
    for ( i = 0; i < (HOTENDS) ; i++ )
    {
        if ( HotEndArr[i] )
            HasHeater = true;
    }

    printf("ok ");
    if ( !HasHeater )
    {
        //printf("T:0\n");
        printf("T:0 ");
    }
    else
    {
        struct HeaterManagerAttr_t Attr, AttrBed;
        if ( (ParamM105->T >= 0) && (ParamM105->T < (HOTENDS) )  )
        {
            if ( HotEndArr[ParamM105->T] )
            {
                struct ViewHeaterMsg_t handle;
                handle.Action = GET_HEATER_MANAGER_ATTR;
                HotEndArr[ParamM105->T]->HandleTemperMsg((S8 *)&handle, sizeof(handle));
                Attr = HotEndArr[ParamM105->T]->CrHeaterMagagerGetAttr();
//                Attr.CurTemper = 100.0f;
//                Attr.TargetTemper = 200.12f;
                S8 Cur[20] = {0}, Target[20] = {0};
                PrintFloat(Attr.CurTemper, Cur);
                PrintFloat(Attr.TargetTemper, Target);
                printf("T:%s /%s ", Cur, Target);
                //printf("T:%f /%f ", HotEndArr[ParamM105->T]->CrHeaterManagerGetTemper(), HotEndArr[ParamM105->T]->CrHeaterMagagerGetAttr().TargetTemper);
            }
        }

        if ( BedHeater )
        {
            struct ViewHeaterMsg_t handle;
            handle.Action = GET_HEATER_MANAGER_ATTR;
            BedHeater->HandleTemperMsg((S8 *)&handle, sizeof(handle));
            AttrBed = BedHeater->CrHeaterMagagerGetAttr();
            S8 Cur[20] = {0}, Target[20] = {0};
            PrintFloat(AttrBed.CurTemper, Cur);
            PrintFloat(AttrBed.TargetTemper, Target);
            printf("B:%s /%s ", Cur, Target);
            //printf("B:%f /%f ", BedHeater->CrHeaterManagerGetTemper(), BedHeater->CrHeaterMagagerGetAttr().TargetTemper);
        }

        if ( (ParamM105->T >= 0) && (ParamM105->T < (HOTENDS) )  )
        {
            if ( HotEndArr[ParamM105->T] )
            {
                printf("@:%d ", Attr.HeaterAttr.Duty);
                //printf("@:%d ", HotEndArr[ParamM105->T]->CrHeaterMagagerGetAttr().HeaterAttr.Duty);
            }
        }

        if ( BedHeater )
        {
            printf("@:%d ", AttrBed.HeaterAttr.Duty);
            //printf("B@:%d ", BedHeater->CrHeaterMagagerGetAttr().HeaterAttr.Duty);
        }
    }
    printf("\n");

    return 1;
}

S32 GcodeM105Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    strcpy(Buff, "ok is not needed");

    return Len;
}
