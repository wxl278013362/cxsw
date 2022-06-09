#include "GcodeG4Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrSleep.h"
#include "CrInc/CrConfig.h"
#include "CrMsgQueue/QueueCommonFuns.h"

#define CMD_G4            "G4"
static CrMsg_t LineMsgQueue = NULL;

S32 GcodeG4Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !LineMsgQueue )
        LineMsgQueue = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));

    /*G1 的操作有 E F X Y E */
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_G) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_G4) != 0 )  //传进来的命令不是G1命令
    {
        //printf("current cmd is not G0 cmd , is : %s\n", CmdBuff);
        return 0;
    }
    
    S32 Offset = strlen(CMD_G4) + sizeof(S8); //加1是越过分隔符
    struct ArgG4_t *CmdArg = (struct ArgG4_t *)Arg;

    while ( Offset < Len )  //处理结束
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            //printf(" Cmd get opt failed  cmd = %s\n", CmdBuff);
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode G0 has no param opt = %s\n", CmdBuff);
        }
        
        switch ( Opt[0] )  //判断命令
        {
            case OPT_P:
             {
                 CmdArg->HasP = 1;
                 if ( OptLen > 1 )
                 {
                     CmdArg->TimeP = atoi(Opt + 1);   //毫秒数
                 }
                 break;
             }
             case OPT_S:
             {
                 CmdArg->HasS = 1;
                 if ( OptLen > 1 )
                 {
                     CmdArg->TimeS = (atof(Opt + 1) * 1000);  //毫秒数
                 }
                 break;
             }
            default:
            {
                printf("Gcode G4 has no define opt = %s\n", CmdBuff);
                break;
            }
        }
        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }
    return 1;
}

S32 GcodeG4Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !MotionModel)
        return 0;
    
    struct ArgG4_t *Param = (struct ArgG4_t *)ArgPtr;
    U32 DwellMs = 0;
    if ( Param->HasP )
        DwellMs = Param->TimeP;

    if ( Param->HasS )
        DwellMs = Param->TimeS;

    //等待运动完成
    while (MotionModel->IsMotionMoveFinished())
    {
        CrM_Sleep(5);
    }

    #ifdef NANODLP_Z_SYNC
      printf("%s\n", STR_Z_MOVE_COMP);
    #endif

    if ( LineMsgQueue )
    {
        /* waitting for temperature.*/
        struct MsgLine_t LineMsg = {0};
        LineMsg.MsgType = SPECIAL_MSG;
        strcpy(LineMsg.Info, "Pause cmd queue");
        if ( LineMsgQueue )
            CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
    }

    //dwell(dwell_ms);  //暂停命令队列等待一段时间
    CrM_Sleep(DwellMs);

    return GCODE_EXEC_OK;
}

S32 GcodeG4Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


