#include "GcodeM191Handle.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrHeaterManager.h"
#include "CrMotion/CrStepperRun.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrMsgQueue/sharedQueue/CrSharedMsgQueue.h"
#include "CrInc/CrMsgType.h"
#include "CrModel/CrGcode.h"
#include "Common/Include/CrSleep.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"

#define CMD_M191        "M191"

static CrMsg_t LineMsgQueue        = NULL;

/********额外定义的变量和函数***********/
extern VOID AutoJobCheckTimer(const BOOL CanStart,const BOOL CanStop);
/********额外定义的变量和函数***********/


VOID GcodeM191Init()
{
    LineMsgQueue = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));
}

S32 GcodeM191Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;


    if ( strcmp(Cmd, CMD_M191) != 0 )// 如果不是M191命令
        return 0;

    S32 Offset = strlen(CMD_M191) + sizeof(S8);
    struct ArgM191_t *CmdArg = (struct ArgM191_t *)Arg;

    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) ) break;

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M191 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                {
                    CmdArg->S = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->R = 0;
                    CmdArg->HasS = false;
                }
                break;
            }
            case OPT_R:
            {
                CmdArg->HasR = true;
                if ( OptLen > 1 )
                {
                    CmdArg->R = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->R = 0;
                    CmdArg->HasR = false;
                }
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}
S32 GcodeM191Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM191_t *Param = (struct ArgM191_t *)ArgPtr;
    struct HeaterManagerAttr_t Attr;
    const BOOL NoWaitForCooling = Param->HasS;

    //设置目标温度
    if ( NoWaitForCooling || Param->HasR )
    {
        if ( NoWaitForCooling )
        {
            Attr = ChamberHeater->CrHeaterMagagerGetAttr();
            Attr.TargetTemper = MIN(Param->S, CHAMBER_MAXTEMP);
            ChamberHeater->CrHeaterMagagerSetAttr(Attr);
        }
        else if ( Param->HasR )
        {
            Attr = ChamberHeater->CrHeaterMagagerGetAttr();
            Attr.TargetTemper = MIN(Param->R, CHAMBER_MAXTEMP);
            ChamberHeater->CrHeaterMagagerSetAttr(Attr);
        }

        //开始打印计时器计时
        AutoJobCheckTimer(true,false);
    }
    else
    {
        return 0;
    }

    //判断是否需要等待
    Attr = ChamberHeater->CrHeaterMagagerGetAttr();
    const BOOL IsHeating = Attr.TargetTemper > Attr.CurTemper;
    if ( IsHeating || !NoWaitForCooling )
    {
        //屏幕提示等待加热
        if ( GcodeModel )
            GcodeModel->StartWaiting();

        struct MsgLine_t LineMsg = {0};
        LineMsg.MsgType = SPECIAL_MSG;
        if ( IsHeating )
        {
            strcpy(LineMsg.Info,"Chamber Heating...");//显示机箱加热中
        }
        else
        {
            strcpy(LineMsg.Info,"Chamber Cooling...");//显示机箱冷却中
        }

        if ( LineMsgQueue )
            CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);

        //等待
        while ( !ChamberHeater->ReachedTargetTemper() )
        {
            if ( GcodeModel )
            {
                if( !GcodeModel->IsWaiting() )
                    break;
            }
            CrM_Sleep(50);
        }

        if( GcodeModel )
        {
            GcodeModel->StopWaiting();
        }

        //操作完成后输出提示
        printf("M191 stop waiting heatting.\n");

        LineMsg.MsgType = SPECIAL_MSG;
        strcpy(LineMsg.Info,"");
        LineMsg.Finished = 1;
        if ( LineMsgQueue )
        {
            CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
        }

    }

    return 1;
}
S32 GcodeM191Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
