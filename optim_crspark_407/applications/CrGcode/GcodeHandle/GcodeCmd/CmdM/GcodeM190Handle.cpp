/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-29     cx2470       the first version
 */

#include "GcodeM190Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrMotion/CrStepperRun.h"
#include "CrModel/CrGcode.h"
#include "CrModel/CrHeaterManager.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrInc/CrMsgType.h"
#include "Common/Include/CrSleep.h"
#include "Common/Include/CrTime.h"
#include "GcodeM155Handle.h"

#define CMD_M190        "M190"

static CrHeaterManager *Bed = NULL;

//static CrPrinter *Printer = NULL;
static CrMsg_t LineMsgQueue = NULL;
//static CrGcode *GcodeModel = NULL;

VOID GcodeM190Init()
{
    LineMsgQueue = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));
}

S32 GcodeM190Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
        return 0;

    //
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M190) != 0 )  //传进来的命令不是190命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    S32 Offset = strlen(CMD_M190) + sizeof(S8); //加1是越过分隔符
    struct ArgM190_t *CmdArg = (struct ArgM190_t *)Arg;

    S8 Opt[20] = {0};
    S32 OptLen = 0;
    while ( Offset < Len )
    {
//        S32     I;        //材料预设索引
//        S16     R;      //目标温度（等待冷却或加热）
//        S16     S;        //目标温度（仅在加热时等待）

        memset(Opt,0,sizeof(Opt));
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            //printf(" Cmd get opt failed  cmd = %s\n", CmdBuff);
            break;    //获取参数失败
        }
        OptLen = strlen(Opt);

        if ( OptLen <= 1 )
        {
            printf("Gcode M190 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_I:
            {
                CmdArg->HasI = true;
                if ( OptLen > 1 )
                    CmdArg->I = atoi(Opt + 1);
                break;
            }
            case OPT_R:
            {
                CmdArg->HasR = true;
                if ( OptLen > 1 )
                    CmdArg->R = atoi(Opt + 1);
                else
                    CmdArg->HasR = false;
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                    CmdArg->S = atoi(Opt + 1);
                else
                    CmdArg->HasS = false;
                break;
            }
            default:
                break;

        }

        Offset += strlen(Opt) + sizeof(S8);

    }
    return 1;
}

S32 GcodeM190Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !BedHeater )
        return 0;

    struct ArgM190_t *Param = (struct ArgM190_t *)ArgPtr;
    if ( !Param->HasS && !Param->HasR )     //R或S至少需要其中一个
    {
        return 0;
    }

    BOOL NoWaitForColling = false;
    FLOAT TargetTemper = 0.0f;
    struct HeaterManagerAttr_t Attr = BedHeater->CrHeaterMagagerGetAttr();

    if ( Param->HasS )      //Target temperature (wait only when heating).
    {
        NoWaitForColling = true;
        TargetTemper = Param->S;
        
    }

    if ( Param->HasR )  //Target temperature (wait for cooling or heating).
    {
        NoWaitForColling = false;
        TargetTemper = Param->R;
    }

    Attr.TargetTemper = MIN(TargetTemper, BED_MAX_TARGET);
    BedHeater->CrHeaterMagagerSetAttr(Attr);

    if ( (Attr.CurTemper >= TargetTemper) && (NoWaitForColling == true) )   //当前温度大于目标温度，且不等待冷却
    {
        return 1;
    }

    //加热或冷却到目标温度均需要等待
    if ( GcodeModel )
        GcodeModel->StartWaiting();

    struct MsgLine_t LineMsg = {0};
    LineMsg.MsgType = SPECIAL_MSG;
    strcpy(LineMsg.Info, "Bed Heating");
    if ( LineMsgQueue )
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);

    SetAutoReportTempInterval(1);
    S8 I = 120;
    S64 Time = 0;
    while ( I > 0 )
    {
        if ( GcodeModel )
            if( !GcodeModel->IsWaiting() )
                break;

        if ( BedHeater->ReachedTargetTemper() )
            I--;

        if ( CrGetSystemTimeMilSecs() - Time > 1000 )
        {
            printf("echo:%s\n", STR_BUSY_PROCESSING);
            Time = CrGetSystemTimeMilSecs();
        }

        CrM_Sleep(50);
    }

    if ( GcodeModel )
        GcodeModel->StopWaiting();

    SetAutoReportTempInterval(0);
    //rt_kprintf("M190 stop waiting heatting");

    LineMsg.MsgType = SPECIAL_MSG;
    strcpy(LineMsg.Info, "");
    LineMsg.Finished = 1;
    if ( LineMsgQueue )
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);

    return 1;
}

S32 GcodeM190Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


