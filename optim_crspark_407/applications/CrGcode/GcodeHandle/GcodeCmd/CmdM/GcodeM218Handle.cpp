#include "GcodeM218Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"

#define CMD_M218 "M218"
#define STR_INVALID_EXTRUDER                "Invalid extruder"
#define STR_HOTEND_OFFSET                   "Hotend offsets:"

S32 GcodeM218Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M218
    if ( strcmp(Cmd, CMD_M218) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M218) + sizeof(S8);
    struct ArgM218_t *CmdArg = (struct ArgM218_t *)Arg;

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
            case OPT_T:
                CmdArg->HasT = true;
                if ( OptLen > 1 )
                    CmdArg->T = atoi(Opt + 1);
                break;
            case OPT_X:
                CmdArg->HasX = true;
                if ( OptLen > 1 )
                    CmdArg->X = atof(Opt + 1);
                break;
            case OPT_Y:
                CmdArg->HasY = true;
                if ( OptLen > 1 )
                    CmdArg->Y = atof(Opt + 1);
                break;
            case OPT_Z:
                CmdArg->HasZ = true;
                if ( OptLen > 1 )
                    CmdArg->Z = atof(Opt + 1);
                break;
            default:
                printf("Warn:unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    if ( !(CmdArg->HasT) )
    {
        //获取活动挤出机的索引
        S8 ActiveExtruder = 0;
        CmdArg->HasT = true;
        CmdArg->T = ActiveExtruder;
    }

    return 1;
 }


S32 GcodeM218Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM218_t *Param = (struct ArgM218_t *)ArgPtr;
    if ( Param->T >= HOTENDS )
    {
        printf("M218 %s %d\n", STR_INVALID_EXTRUDER, Param->T);
        return 1;
    }

    if ( Param->HasX )
    {
        HotendOffset[Param->T].X = Param->X;
    }

    if ( Param->HasY )
    {
        HotendOffset[Param->T].Y = Param->Y;
    }

    if ( Param->HasZ )
    {
        HotendOffset[Param->T].Z = Param->Z;
    }

    if ( !( Param->HasX || Param->HasY || Param->HasZ) )
    {
        printf("%s %f,%f,%f\n", STR_HOTEND_OFFSET,HotendOffset[Param->T].X
                                , HotendOffset[Param->T].Y, HotendOffset[Param->T].Z);
    }

#ifdef DELTA
    //获取活动挤出机的索引
    S8 ActiveExtruder = 0;
    if ( Param->T == ActiveExtruder )
    {
        //do_blocking_move_to_xy(current_position, planner.settings.max_feedrate_mm_s[X_AXIS]);
        //快速运动到当前位置。

    }
#endif

    return 1;
}

S32 GcodeM218Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

