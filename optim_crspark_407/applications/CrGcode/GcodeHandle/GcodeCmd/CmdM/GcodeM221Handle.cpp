#include "GcodeM221Handle.h"
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"

#define CMD_M221 ("M221")

S32 GcodeM221Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S8 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是m
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M221) != 0 )  //传进来的命令不是G0命令
        return 0;

    S8 Offset = strlen(CMD_M221) + sizeof(S8); //加1是越过分隔符
    struct ArgM221_t *CmdArg = (struct ArgM221_t *)Arg;

    S8 Opt[20] = {0};
    S8 OptLen = 0;

    while ( Offset < Len )
    {
        memset(Opt, 0, sizeof(Opt));
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
            break;    //获取参数失败

        OptLen = strlen(Opt);

        if ( OptLen <= 1 )
        {
//            printf("Gcode M221 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_T:

                CmdArg->HasT = true;

                if (OptLen > 1)
                    CmdArg->T = atoi(Opt + 1);
            break;

            case OPT_S:

                CmdArg->HasS = true;

                if (OptLen > 1)
                    CmdArg->S = atof(Opt + 1);
            break;

        }
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

S32 GcodeM221Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM221_t *Param = (struct ArgM221_t *)ArgPtr;
    //rt_kprintf("%s:%d\n", __FUNCTION__, __LINE__);

    struct FeedrateAttr_t Flow = MotionModel->GetMotionFeedRateAttr();

    S8 T = 0;
    if ( Param->HasT )
    {
        /*FIXME: Switch target extruder*/
        T = Param->T;
        if ( T < 0 || T >= EXTRUDERS )      //传入的挤出机索引非法
        {
            return 0;
        }
    }

    if ( Param->HasS )
    {
        Flow.FlowRatio = Param->S;
        MotionModel->SetMotionFeedRateAttr(Flow);
    }
    else
    {
        printf("echo Flow: %d%\n", Flow.FlowRatio);
    }

    return 1;
}

S32 GcodeM221Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
