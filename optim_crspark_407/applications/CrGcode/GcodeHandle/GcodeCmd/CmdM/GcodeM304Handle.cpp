#include "GcodeM304Handle.h"
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrHeaterManager.h"
#include "CrHeater/Pid/CrPid.h"

#define CMD_M304           "M304"

S32 GcodeM304Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0] != GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M304) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M304) + sizeof(S8); // 加1是越过分隔符
    struct ArgM304_t *CmdArg = (struct ArgM304_t *)Arg;
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
            case OPT_D:
            {
                CmdArg->HasD = true;
                if ( OptLen > 1 ) CmdArg->D = atof( Opt + 1 );
                break;
            }
            case OPT_I:
            {
                CmdArg->HasI = true;
                if ( OptLen > 1 ) CmdArg->I = atof( Opt + 1 );
                break;
            }
            case OPT_P:
            {
                CmdArg->HasP = true;
                if ( OptLen > 1 ) CmdArg->P = atof( Opt + 1 );
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}


S32 GcodeM304Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM304_t *Arg = (struct ArgM304_t *)ArgPtr;
    if ( !BedHeater )
    {
        printf("bed heater object is NULL");
        return 1;
    }

    CrPid* Pid = BedHeater->GetPid();
    if ( !Pid )
    {
        printf("bed heater pid object is NULL");
        return 1;
    }

    struct PidAttr_t Attr = Pid->GetPidAttr();
    if ( Arg->HasP )
        Attr.Kp = Arg->P;

    if ( Arg->HasI )
    {
        Attr.Ki = Arg->I;
    }

    if ( Arg->HasD )
    {
        Attr.Kd = Arg->D;
    }

    Pid->SetPidAttr(Attr);

    printf("bed pid : p = %f, i = %f, d = %f\n", Attr.Kp, Attr.Ki, Attr.Kd);

    return 1;
}

S32 GcodeM304Reply(VOID *ReplyResult, S8 *Buff)
{

    return 1;
}
