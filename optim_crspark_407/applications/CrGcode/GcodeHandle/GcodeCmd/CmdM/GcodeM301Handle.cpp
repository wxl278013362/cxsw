#include "GcodeM301Handle.h"
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrHeaterManager.h"
#include "CrHeater/Pid/CrPid.h"

#define CMD_M301           "M301"
#define MSG_INVALID_EXTRUDER                "Invalid extruder"

S32 GcodeM301Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0] != GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M301) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M301) + sizeof(S8); // 加1是越过分隔符
    struct ArgM301_t *CmdArg = (struct ArgM301_t *)Arg;
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
            case OPT_C:
            {
                CmdArg->HasC = true;
                if ( OptLen > 1 ) CmdArg->C = atof( Opt + 1 );
                break;
            }
            case OPT_D:
            {
                CmdArg->HasD = true;
                if ( OptLen > 1 ) CmdArg->D = atof( Opt + 1 );
                break;
            }
            case OPT_E:
            {
                CmdArg->HasE = true;
                if ( OptLen > 1 ) CmdArg->E = atoi( Opt + 1 );
                break;
            }
            case OPT_I:
            {
                CmdArg->HasI = true;
                if ( OptLen > 1 ) CmdArg->I = atof( Opt + 1 );
                break;
            }
            case OPT_L:
            {
                CmdArg->HasL = true;
                if ( OptLen > 1 ) CmdArg->L = atof( Opt + 1 );
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

    if ( !CmdArg->HasE )
    {
        CmdArg->HasE = 1;
        CmdArg->E = 0;
    }


    return 1;
}


S32 GcodeM301Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM301_t *Arg = (struct ArgM301_t *)ArgPtr;
    if ( !Arg->HasE || (Arg->E >= HOTENDS ) )
    {
        printf(MSG_INVALID_EXTRUDER);
        return 1;
    }

    CrHeaterManager *HotEnd = NULL;
    if ( (HOTENDS) >= 1)
        HotEnd = HotEndArr[Arg->E];    //挤出机的数量

    if ( !HotEnd )
    {
        printf(MSG_INVALID_EXTRUDER);
        return 1;
    }

    CrPid* Pid = HotEnd->GetPid();
    if ( !Pid )
    {
        printf(MSG_INVALID_EXTRUDER);
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

//    //注意此处是PID风扇，不是PWM风扇
//    if ( Arg->HasF && PwmFan)
//    {
//        if ( Arg->F < 0 )
//             Arg->F = 0;
//
//        if ( Arg->F >= 255 )
//            Arg->F = 255;
//
//        struct PwmControllerAttr_t Attr = PwmFan->GetFanAttr();
//        Attr.Duty = Arg->F;
//        PwmFan->SetFanAttr(Attr);
//    }

    //C、F和L暂时不处理
    printf("extrudes num : %d", Arg->E);
    printf("p = %f, i = %f, d = %f", Attr.Kp, Attr.Ki, Attr.Kd);
//    if ( Arg->HasF && PwmFan )
//    {
//        printf("pwm fan duty %d\n", PwmFan->GetFanAttr().Duty);
//    }

    return 1;
}

S32 GcodeM301Reply(VOID *ReplyResult, S8 *Buff)
{
    return 1;
}
