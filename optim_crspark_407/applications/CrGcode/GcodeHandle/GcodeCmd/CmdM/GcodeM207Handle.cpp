/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-08     cx2470       the first version
 */
#include "GcodeM207Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrMotion.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"

#define CMD_M207           "M207"

S32 GcodeM207Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0]!= GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M207) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M207) + sizeof(S8); // 加1是越过分隔符
    struct ArgM207_t *CmdArg = (struct ArgM207_t *)Arg;

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
            case OPT_Z:
            {
                CmdArg->HasZ = true;
                if ( OptLen > 1 ) CmdArg->Z = atof( Opt + 1 );
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}

S32 GcodeM207Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !MotionModel || !ArgPtr || !ReplyResult )
        return 0;

    struct FwRetract_t FwRetract = MotionModel->GetFwRetractSetting();
    struct ArgM207_t *CmdArg = (struct ArgM207_t *)ArgPtr;

    if ( CmdArg->HasF )
        FwRetract.FeedRate = CmdArg->F;

    if ( CmdArg->HasS )
        FwRetract.RetractLength = CmdArg->S;

    if ( CmdArg->HasW )
        FwRetract.SwapRetractLength = CmdArg->W;

    if ( CmdArg->HasZ )
        FwRetract.RetractZ_Raise = CmdArg->Z;

    MotionModel->SetFwRetractSetting(FwRetract);

    if ( FwRetract.FwRetractEnable && CmdArg->HasF )
    {
        struct FeedrateAttr_t FeedRate = MotionModel->GetMotionFeedRateAttr();
        FeedRate.RetractRate = FwRetract.FeedRate;
        MotionModel->SetMotionFeedRateAttr(FeedRate);
    }

    return 1;
}

S32 GcodeM207Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

