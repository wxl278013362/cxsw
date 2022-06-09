#include "GcodeM603Handle.h"
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "Common/Include/CrEepromApi.h"

#define CMD_M603           "M603"

S32 GcodeM603Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0] != GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M603) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M603) + sizeof(S8); //加1是越过分隔符
    struct ArgM603_t *CmdArg = (struct ArgM603_t *)Arg;

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
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_T:
            {
                CmdArg->HasT = true;
                if ( OptLen > 1 ) CmdArg->T = atoi( Opt + 1 );
                break;
            }
            case OPT_L:
            {
                CmdArg->HasL = true;
                if ( OptLen > 1 ) CmdArg->L = atof( Opt + 1 );

                break;
            }
            case OPT_U:
            {
                CmdArg->HasU = true;
                if ( OptLen > 1 ) CmdArg->U = atof( Opt + 1 );

                break;
            }
            default:   /*关闭所有电机*/
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}


S32 GcodeM603Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM603_t *CmdArg = (struct ArgM603_t *)ArgPtr;
    S8 ExtruderIndex = 0;   //当前活动的挤出机
    S8 LastExtruderIndex = 0;  //在切换前保存当前活动的挤出机索引
    if ( CmdArg->HasT )
    {
        if ( (CmdArg->T >= 0) && (CmdArg->T < (AxisNum - E)) )
        {
            ExtruderIndex = CmdArg->T;
        }
        else
        {
            printf("%s %s %d", CMD_M603,  "Invalid extruder", CmdArg->T);
        }
    }

    if ( ExtruderIndex < 0 )
        return 0;

    struct FilaChangeSettings_t *FcSetting = NULL;   //获取目的挤出机的FilaChangeSettings
    if ( FcSetting )
    {
        if ( CmdArg->HasL )
        {
            FcSetting->LoadLength = CmdArg->L;   //加载长丝
#ifdef PREVENT_LENGTHY_EXTRUDE
            if ( FcSetting->LoadLength > EXTRUDE_MAXLENGTH )
                FcSetting->LoadLength = EXTRUDE_MAXLENGTH;
#endif
        }
        if ( CmdArg->HasU )
        {
            FcSetting->UnloadLength = CmdArg->U;    //卸下长丝
#ifdef PREVENT_LENGTHY_EXTRUDE
            if ( FcSetting->UnloadLength > EXTRUDE_MAXLENGTH )
                FcSetting->UnloadLength = EXTRUDE_MAXLENGTH;
#endif
        }
    }

    return 1;
}

S32 GcodeM603Reply(VOID *ReplyResult, S8 *Buff)
{

    return 1;
}
