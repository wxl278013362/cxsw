#include "GcodeM117Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrModel/CrPrinter.h"
#include "CrModel/CrSdCard.h"
#include "Common/Include/CrSleep.h"

#define CMD_M117  "M117"
static CrMsg_t LineMsgQ = NULL;

S32 GcodeM117Parse(const S8 *CmdBuff, VOID *Arg)
{
    static BOOL Init = false;
    if ( !Init )
    {
        LineMsgQ = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));
        if ( LineMsgQ )
            Init = true;
    }

    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
     if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
         return 0;

     S8 Cmd[CMD_BUFF_LEN] = {0};
     if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
         return 0;

     if ( strcmp(Cmd, CMD_M117) != 0 )
         return 0;

     S32 Offset = strlen(CMD_M117) + sizeof(S8); //加1是越过分隔符
     struct ArgM117_t *CmdArg = (struct ArgM117_t *)Arg;

     if ( Len > Offset )
     {
         CmdArg->HasStr = true;
         strncpy(CmdArg->Str, CmdBuff + Offset, sizeof(CmdArg->Str));
     }

     return 1;
 }

S32 GcodeM117Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult  )
        return 0;

    struct ArgM117_t *CmdArg = (struct ArgM117_t *)ArgPtr;

    if ( CmdArg->HasStr )
    {
        //输出字符串
        struct MsgLine_t LineMsg = {0};
        LineMsg.MsgType = SPECIAL_MSG;
        strncpy(LineMsg.Info, CmdArg->Str, sizeof(LineMsg.Info) - 1);
        if ( LineMsgQ )
            CrMsgSend(LineMsgQ, (S8 *)&LineMsg, sizeof(LineMsg), 1);

        return 1;
    }

    if ( Printer )
    {
        //
        enum PrintStatus_t Status = Printer->GetPrintStatus();
        if ( Status == PAUSE_PRINT )
        {
            struct MsgLine_t LineMsg = {0};
            LineMsg.MsgType = SPECIAL_MSG;
            strncpy(LineMsg.Info, "Pause Print", sizeof(LineMsg.Info) - 1);
            if ( LineMsgQ )
                CrMsgSend(LineMsgQ, (S8 *)&LineMsg, sizeof(LineMsg), 1);
        }
        else if(Status == STOP_PRINT)
        {
            struct MsgLine_t LineMsg = {0};
            LineMsg.MsgType = SPECIAL_MSG;
            strncpy(LineMsg.Info, WELCOME_STR, sizeof(LineMsg.Info) - 1);
            if ( LineMsgQ )
                CrMsgSend(LineMsgQ, (S8 *)&LineMsg, sizeof(LineMsg), 1);
        }
        else
        {
            //输出打印文件名
            if ( SdCardModel )
            {
                struct MsgLine_t LineMsg = {0};
                LineMsg.MsgType = SPECIAL_MSG;
                strncpy(LineMsg.Info, SdCardModel->GetFileName(), sizeof(LineMsg.Info) - 1);
                if ( LineMsgQ )
                    CrMsgSend(LineMsgQ, (S8 *)&LineMsg, sizeof(LineMsg), 1);
            }
            else
            {
                //输出欢迎字符串
                struct MsgLine_t LineMsg = {0};
                LineMsg.MsgType = SPECIAL_MSG;
                strncpy(LineMsg.Info, WELCOME_STR, sizeof(LineMsg.Info) - 1);
                if ( LineMsgQ )
                    CrMsgSend(LineMsgQ, (S8 *)&LineMsg, sizeof(LineMsg), 1);
            }
        }
    }
    else
    {
         //发送欢迎字符串
        struct MsgLine_t LineMsg = {0};
        LineMsg.MsgType = SPECIAL_MSG;
        strncpy(LineMsg.Info, WELCOME_STR, sizeof(LineMsg.Info) - 1);
        if ( LineMsgQ )
            CrMsgSend(LineMsgQ, (S8 *)&LineMsg, sizeof(LineMsg), 1);
    }

    return 1;
}

S32 GcodeM117Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
