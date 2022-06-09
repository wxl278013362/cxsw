/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-08     cx2470       the first version
 */
#include "GcodeM208Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"



#define CMD_M208           "M208"

S32 GcodeM208Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0]!= GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M208) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M208) + sizeof(S8); // 加1是越过分隔符
    struct ArgM208_t *CmdArg = (struct ArgM208_t *)Arg;

    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) ) break;

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )
        {
            case OPT_F:
            {
                CmdArg->HasF = true;
                if ( OptLen > 1 ) CmdArg->F = atof( Opt + 1 );
                break;
            }
            case OPT_W:
            {
                CmdArg->HasW = true;
                if ( OptLen > 1 ) CmdArg->W = atof( Opt + 1 );
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atof( Opt + 1 );
                break;
            }
            case OPT_R:
            {
                CmdArg->HasR = true;
                if ( OptLen > 1 ) CmdArg->R = atof( Opt + 1 );
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}

S32 GcodeM208Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !MotionModel || !ArgPtr || !ReplyResult )
        return 0;

    struct FwRetract_t FwRetract = MotionModel->GetFwRetractSetting();
    struct ArgM208_t *CmdArg = (struct ArgM208_t *)ArgPtr;

    if ( CmdArg->HasF )
        FwRetract.RecoverFeedRate = CmdArg->F;

    if ( CmdArg->HasR )
        FwRetract.SwapRecoverFeedRate = CmdArg->R;

    if ( CmdArg->HasS )
        FwRetract.RecoverLengthExtra = CmdArg->S;

    if ( CmdArg->HasW )
        FwRetract.SwapRecoverLength = CmdArg->W;

    MotionModel->SetFwRetractSetting(FwRetract);

    if ( FwRetract.FwRetractEnable && CmdArg->HasF )
    {
        struct FeedrateAttr_t FeedRate = MotionModel->GetMotionFeedRateAttr();
        FeedRate.ExtrudeRate = FwRetract.RecoverFeedRate;
        MotionModel->SetMotionFeedRateAttr(FeedRate);
    }

    return 1;
}

S32 GcodeM208Reply(VOID *ReplyResult, S8 *Buff)
{


    return 0;
}



