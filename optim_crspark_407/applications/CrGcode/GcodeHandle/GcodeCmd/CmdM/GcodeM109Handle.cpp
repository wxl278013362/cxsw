/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-29     cx2470       the first version
 */
#include "GcodeM109Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <rtthread.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrHeaterManager.h"
#include "CrModel/CrGcode.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrInc/CrMsgType.h"
#include "GcodeM155Handle.h"
#include "CrInc/CrConfig.h"
#include "Common/Include/CrSleep.h"
#include "Common/Include/CrTime.h"

#define CMD_M109  "M109"
U8 WaitHeating = 0;

//static CrHeaterManager *M109NozzleHeater = NULL;
static CrMsg_t LineMsgQueue = NULL;
//static CrGcode *GcodeModel = NULL;

VOID GcodeM109Init()
{
    LineMsgQueue = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));
}

S32 GcodeM109Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
     if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
         return 0;
    
     S8 Cmd[CMD_BUFF_LEN] = {0};
     if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
         return 0;

     if ( strcmp(Cmd, CMD_M109) != 0 )
         return 0;

     S32 Offset = strlen(CMD_M109) + sizeof(S8); //加1是越过分隔符
     struct ArgM109_t *CmdArg = (struct ArgM109_t *)Arg;
     CmdArg->IsDefaultT = 1;
     while ( Offset < Len )  //处理结束
     {
         S8 Opt[20] = {0};
         if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
             break;

         S32 OptLen = strlen(Opt);
         if ( OptLen <= 1 )
         {
             rt_kprintf("Gcode M109 has no param opt = %s\n", CmdBuff);
         }

         switch ( Opt[0] )  //判断命令
         {
             case OPT_T:
             {
                 CmdArg->HasT = true;
                 CmdArg->T = atoi(Opt + 1);
                 CmdArg->IsDefaultT = 0;
                 break;
             }
             case OPT_I:
             {
                
                 CmdArg->HasI = true;
                 CmdArg->I = atoi(Opt + 1);
                 break;
             }
             case OPT_S:
             {
                 CmdArg->HasS = true;
                 CmdArg->S = atoi(Opt + 1);
                 break;
             }
             case OPT_R:
             {
                 CmdArg->HasR = true;
                 CmdArg->R = atoi(Opt + 1);
                 break;
             }
             case OPT_B:
             {
                 CmdArg->HasB = true;
                 CmdArg->B = atoi(Opt + 1);
                 break;
             }
             case OPT_F:
             {
                 CmdArg->HasF = true;
                 CmdArg->F = atoi(Opt + 1);
                 break;
             }
             default:
             {
                 rt_kprintf("Gcode M109 No default Opt = %s\n", CmdBuff);
                 break;
             }
         }

         Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
     }
    return 1;
}

S32 GcodeM109Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        rt_kprintf("Param is null\n");
        return 0;
    }
    struct ArgM109_t *Param = (struct ArgM109_t *)ArgPtr;
    if ( ( HOTENDS < 1 ) || !HotEndArr[0] )
    {
        printf("M109 Nozzle is null!\n");
        return 1;
    }
    if (Param->IsDefaultT)
    {
        if (Param->HasB)
        {
            rt_kprintf("M109 NOT SUPPORT opt:B \n");
        }
        if (Param->HasF)
        {
            rt_kprintf("M109 NOT SUPPORT opt:F \n");
        }
        if ( Param->HasR )
        {
            /* set heater temperature */
            struct HeaterManagerAttr_t Attr = {0};
            Attr = HotEndArr[0]->CrHeaterMagagerGetAttr();
            if ( Attr.TargetTemper - Param->R > 0.000001 || Attr.TargetTemper - Param->R < -0.000001)
            {
                Attr.TargetTemper = Param->R;
                HotEndArr[0]->CrHeaterMagagerSetAttr(Attr);
            }
        }
        if (Param->HasS)
        {
            S32 index = Param->S;
            if (Param->HasI)
            {
                index = Param->I;
            }
            /* set heater temperature.*/
            struct HeaterManagerAttr_t Attr = {0};
            Attr = HotEndArr[0]->CrHeaterMagagerGetAttr();
            if ( Attr.TargetTemper - index > 0.000001 || Attr.TargetTemper - index < -0.000001)
            {
                Attr.TargetTemper = index;
                HotEndArr[0]->CrHeaterMagagerSetAttr(Attr);
            }
        }
    }
    else /* All active heater.*/
    {
        if (!Param->HasT)
        {
            printf("has no hotend index.\n");
            return 0;
        }
        if (Param->HasS)
        {
            S32 celsius = Param->S;
            if (Param->HasI)
            {
                celsius = Param->I;
            }
            /* set heater temperature.*/
            struct HeaterManagerAttr_t Attr = {0};
            Attr = HotEndArr[0]->CrHeaterMagagerGetAttr();
            if ( Attr.TargetTemper - celsius > 0.000001 || Attr.TargetTemper - celsius < -0.000001)
            {
                Attr.TargetTemper = celsius;
                HotEndArr[0]->CrHeaterMagagerSetAttr(Attr);
            }
        }
        if (Param->HasB)
        {
            /* set heater max temperature.*/
            rt_kprintf("M109 NOT SUPPORT opt:B \n");
        }
        if ( Param->HasR )
        {
            /*FIXME:
             * 1 需要等待加熱标志
             * */
            struct HeaterManagerAttr_t Attr = {0};
            Attr = HotEndArr[0]->CrHeaterMagagerGetAttr();
            if ( Attr.TargetTemper - Param->R > 0.000001 || Attr.TargetTemper - Param->R < -0.000001)
            {
                Attr.TargetTemper = Param->R;
                HotEndArr[0]->CrHeaterMagagerSetAttr(Attr);
            }
        }
        if (Param->HasF)
        {
            /* set auto-temperature flag.*/ 
            rt_kprintf("M109 NOT SUPPRT opt:F \n");
        }
    }

    if(!(Param->HasB || Param->HasF || Param->HasI || Param->HasS || Param->HasT))
    {
        /* disable auto-temperature.*/
        struct HeaterManagerAttr_t Attr = {0};
        Attr = HotEndArr[0]->CrHeaterMagagerGetAttr();
        Attr.TargetTemper = 0.0f;
        HotEndArr[0]->CrHeaterMagagerSetAttr(Attr);
    }
    WaitHeating = 1;
    if ( GcodeModel )
        GcodeModel->StartWaiting();

    /* waitting for temperature.*/
    struct MsgLine_t LineMsg = {0};
    LineMsg.MsgType = SPECIAL_MSG;
    strcpy(LineMsg.Info, "Nozzle Heating");
    if ( LineMsgQueue )
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);


    SetAutoReportTempInterval(1);
    if ( WaitHeating )
    {
        S64 Time = 0;
        if ( !HotEndArr[0]->ReachedTargetTemper() )
        {
            S8 I = 120;
            do
            {
                if( GcodeModel )
                    if ( !(GcodeModel->IsWaiting()) )
                        break;

                rt_thread_delay(50);

                if ( HotEndArr[0]->ReachedTargetTemper() )
                    I--;

                if ( CrGetSystemTimeMilSecs() - Time > 1000 )
                {
                    printf("echo:%s\n", STR_BUSY_PROCESSING);
                    Time = CrGetSystemTimeMilSecs();
                }

            }while ( I > 0  );
        }

        //打印输出温度
        WaitHeating = 0;
    }

    SetAutoReportTempInterval(0);
    if ( GcodeModel )
        GcodeModel->StopWaiting();

    //rt_kprintf("M109 stop waiting heatting\n");
    LineMsg.MsgType = SPECIAL_MSG;
    strcpy(LineMsg.Info, "");
    LineMsg.Finished = 1;
    if ( LineMsgQueue )
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);

    return 1;
}
S32 GcodeM109Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

void SetTemperature()
{
	if ( HotEndArr[0] )
	{
	    struct HeaterManagerAttr_t Attr = {0};
	    Attr = HotEndArr[0]->CrHeaterMagagerGetAttr();
	    FLOAT celsius = 210.0f;
	    if ( Attr.TargetTemper - celsius > 0.000001 || Attr.TargetTemper - celsius < -0.000001)
	    {
	        Attr.TargetTemper = celsius;
	        HotEndArr[0]->CrHeaterMagagerSetAttr(Attr);
	    }
	}
}
MSH_CMD_EXPORT(SetTemperature,SetTemperature);

