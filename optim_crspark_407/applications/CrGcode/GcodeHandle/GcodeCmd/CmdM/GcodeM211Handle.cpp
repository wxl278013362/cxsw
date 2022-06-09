#include "GcodeM211Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"

#define CMD_M211 "M211"

S32 GcodeM211Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M211
    if ( strcmp(Cmd, CMD_M211) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M211) + sizeof(S8);
    struct ArgM211_t *CmdArg = (struct ArgM211_t *)Arg;

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
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                    CmdArg->S = atoi(Opt + 1);
                break;
            default:
                printf("Warn:unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
 }


S32 GcodeM211Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM211_t *Param = (struct ArgM211_t *)ArgPtr;
    BOOL SoftEndstopEnable = MotionModel->SoftEndStopEnabled();
    struct MotionCoord_t SoftMin = {X_MIN_POS,Y_MIN_POS,Z_MIN_POS,0};
    struct MotionCoord_t SoftMax = {X_MAX_POS, Y_MAX_POS,Z_MAX_POS, 0};
    if ( Param->HasS )
    {
        SoftEndstopEnable = Param->S;
        //设置软件限位的使能或失能
        MotionModel->SetSoftEndStop(SoftEndstopEnable);
    }

    printf("%s%d\n", STR_SOFT_ENDSTOPS,SoftEndstopEnable);

    printf("%s(%f, %f, %f) ", STR_SOFT_MIN, SoftMin.CoorX, SoftMin.CoorY, SoftMin.CoorZ);
    printf("%s(%f, %f, %f)\n", STR_SOFT_MAX, SoftMax.CoorX, SoftMax.CoorY, SoftMax.CoorZ);

    return 1;
}

S32 GcodeM211Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

