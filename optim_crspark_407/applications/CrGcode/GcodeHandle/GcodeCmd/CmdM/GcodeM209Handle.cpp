/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-08     cx2470       the first version
 */
#include "GcodeM209Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"

#define CMD_M209           "M209"

S32 GcodeM209Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0]!= GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M209) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M209) + sizeof(S8); // 加1是越过分隔符
    struct ArgM209_t *CmdArg = (struct ArgM209_t *)Arg;

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
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atoi( Opt + 1 );
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}

S32 GcodeM209Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !MotionModel || !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM209_t *CmdArg = (struct ArgM209_t *)ArgPtr;
    struct FwRetract_t FwRetract = MotionModel->GetFwRetractSetting();

    if ( CmdArg->HasS )
    {
        FwRetract.FwRetractEnable = CmdArg->S;
        MotionModel->SetFwRetractSetting(FwRetract);
    }

    return 1;
}
S32 GcodeM209Reply(VOID *ReplyResult, S8 *Buff)
{


    return 0;
}



