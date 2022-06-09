#include "GcodeM145Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrModel/CrParam.h"

#define CMD_M145        "M145"
static CrMsg_t TargetTemperQueue = NULL;
static CrMsg_t FanSpeedQueue = NULL;

VOID GcodeM145Init()
{
    TargetTemperQueue = ViewRecvQueueOpen((S8 *)PARAM_TEMPER_REPORT_QUEUE, sizeof(struct ParamMaterialTemperSet_t));  //材料的预热温度
    FanSpeedQueue = ViewRecvQueueOpen((S8 *)PARAM_FANS_REPORT_QUEUE, sizeof(struct ViewFanSpeedParamMsg_t));  //材料的风扇速度
}

S32 GcodeM145Parse(const S8 *CmdBuff, VOID *Arg)
{
    printf("M145\n");
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M145) != 0 )  //传进来的命令不是G0命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    S32 Offset = strlen(CMD_M145) + sizeof(S8); //加1是越过分隔符
    struct ArgM145_t *CmdArg = (struct ArgM145_t *)Arg;

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
            printf("Gcode G0 has no param opt = %s\n", CmdBuff);
        }
        switch (Opt[0])
        {
            case OPT_B:
                CmdArg->HasB = 1;
                if (OptLen > 1)
                {
                    CmdArg->B = atoi(Opt + 1);
                }
            break;
            case OPT_S:
                CmdArg->HasS = 1;
                if (OptLen > 1)
                {
                    CmdArg->S = atoi(Opt + 1);
                }
            break;
            case OPT_H:
                CmdArg->HasH = 1;

                if (OptLen > 1)
                {
                    CmdArg->H = atoi(Opt + 1);
                }
            break;
            case OPT_F:
                CmdArg->HasF = 1;
                if (OptLen > 1)
                {
                    CmdArg->F = atof(Opt + 1);
                }
            break;
        }
        
        Offset += strlen(Opt) + sizeof(S8);
        
    }
    return 1;
}

S32 GcodeM145Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !ParamModel )
        return 0;

    struct ArgM145_t *Param = (struct ArgM145_t *)ArgPtr;
    if ( !(Param->HasS) )
        return 1;

    struct ParamMaterialTemperSet_t  Temper =  ParamModel->TargetTemperParmGet();
    struct ViewFanSpeedParamMsg_t  Fan = ParamModel->FanSpeedParmGet();

    S16 SendTemper = 0, SendFan = 0;
    S16 NozzleTemper = -1, BedTemper = -1;
    FLOAT FanSpeed = -1.0f;
    if ( Param->HasB )
    {
        SendTemper = 1;
        BedTemper = Param->B;
    }

    if ( Param->HasH )
    {
        SendTemper = 1;
        NozzleTemper = Param->H;
    }

    if ( Param->HasF )
    {
        FanSpeed = Param->F;
        SendFan = 1;
    }

    if ( Param->S )   //非零位PLA， 零位ABS
    {
        if ( NozzleTemper >= 0 )
            Temper.Pla.Nozzle = NozzleTemper;

        if ( BedTemper >= 0 )
            Temper.Pla.Bed = BedTemper;

        if ( FanSpeed >= 0 )
            Fan.Fan.Pla.Speed = FanSpeed;
    }
    else
    {
        if ( NozzleTemper >= 0 )
            Temper.Abs.Nozzle = NozzleTemper;

        if ( BedTemper >= 0 )
            Temper.Abs.Bed = BedTemper;

        if ( FanSpeed >= 0 )
            Fan.Fan.Abs.Speed = FanSpeed;
    }

    if ( SendTemper )
        CrMsgSend(TargetTemperQueue, (S8 *)&Temper, sizeof(Temper), 1);

    if ( SendFan )
        CrMsgSend(FanSpeedQueue, (S8 *)&Fan, sizeof(Fan), 1);

    return 1;
}

S32 GcodeM145Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


