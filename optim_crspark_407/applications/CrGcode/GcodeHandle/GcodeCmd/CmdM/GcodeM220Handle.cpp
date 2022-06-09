#include "GcodeM220Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"

#define CMD_M220 ("M220")

S32 GcodeM220Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S8 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M220) != 0 )  //传进来的命令不是G0命令
        return 0;

    S8 Offset = strlen(CMD_M220) + sizeof(S8); //加1是越过分隔符
    struct ArgM220_t *CmdArg = (struct ArgM220_t *)Arg;

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
//            printf("Gcode M220 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_B:

                CmdArg->HasB = true;

                if (OptLen > 1)
                    CmdArg->B = atof(Opt + 1);
            break;

            case OPT_R:

                CmdArg->HasR = true;

                if (OptLen > 1)
                {
                    CmdArg->R = atof(Opt + 1);
                }
            break;

            case OPT_S:

                CmdArg->HasS = true;

                if (OptLen > 1)
                    CmdArg->S = atof(Opt + 1);
                else
                    CmdArg->HasS = false;
            break;

        }
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

S32 GcodeM220Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM220_t *Param = (struct ArgM220_t *)ArgPtr;

    /*back up the current FeedRate Percentage.*/
    static U32 BackupFeedRatePercentage = 100;

    struct FeedrateAttr_t FeedRate = MotionModel->GetMotionFeedRateAttr();
    //rt_kprintf("%s:%d\n", __FUNCTION__, __LINE__);

    if ( Param->HasB )
    {
        BackupFeedRatePercentage = FeedRate.FeedrateRatio;
    }
    if ( Param->HasR )
    {
        FeedRate.FeedrateRatio = BackupFeedRatePercentage;
    }
    if ( Param->HasS )
    {
        FeedRate.FeedrateRatio = ((U32)Param->S == 0.0) ? 100 : (U32)Param->S;
    }

    MotionModel->SetMotionFeedRateAttr(FeedRate);
    //rt_kprintf("%s:%d Param->S = %d\n", __FUNCTION__, __LINE__, (S32)FeedRate.FeedrateRatio);

    if ( !Param->HasB && !Param->HasR && !Param->HasS )
    {
        /*report current FeedRate Percentage*/
        printf("FR: %d%\n", FeedRate.FeedrateRatio);
    }
    //rt_kprintf("%s:%d\n", __FUNCTION__, __LINE__);

    return 1;
}

S32 GcodeM220Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
