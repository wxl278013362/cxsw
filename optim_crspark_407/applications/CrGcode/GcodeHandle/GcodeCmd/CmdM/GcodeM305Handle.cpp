#include "GcodeM302Handle.h"
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrHeaterManager.h"

#define CMD_M305           "M305"

S32 GcodeM305Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len < 1 || CmdBuff[0] != GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M305) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M305) + sizeof(S8); // 加1是越过分隔符
    struct ArgM305_t *CmdArg = (struct ArgM305_t *)Arg;
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
            case OPT_B:
            {
                CmdArg->HasB = true;
                if ( OptLen > 1 ) CmdArg->B = atof( Opt + 1 );
                break;
            }
            case OPT_C:
            {
                CmdArg->HasC = true;
                if ( OptLen > 1 ) CmdArg->C = atof( Opt + 1 );
                break;
            }
            case OPT_P:
            {
                CmdArg->HasP = true;
                if ( OptLen > 1 ) CmdArg->P = atoi( Opt + 1 );
                break;
            }
            case OPT_R:
            {
                CmdArg->HasR = true;
                if ( OptLen > 1 ) CmdArg->R = atof( Opt + 1 );
                break;
            }
            case OPT_T:
            {
                CmdArg->HasT = true;
                if ( OptLen > 1 ) CmdArg->T = atof( Opt + 1 );
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}


S32 GcodeM305Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM305_t *Arg = (struct ArgM305_t *)ArgPtr;

    BOOL SetParam = (Arg->HasB) || (Arg->HasC) || (Arg->HasT) || (Arg->HasR);
    if(!Arg->HasP)
    {
        printf("M305 no heater index");
        return 1;
    }

    if ( SetParam )
    {
        if ( Arg->P >= (HOTENDS + 1) || (Arg->P < 0  ) )
        {
            printf("!Invalid index. (0 <= P <= %d )\n", HOTENDS);
            return 1;
        }

        //开始设置参数



    }
    else
    {
        if ( Arg->P >= (HOTENDS + 1) )
        {
            printf("!Invalid index. (0 <= P <= %d )\n", HOTENDS);
            return 1;
        }
        else if ( Arg->P < 0 )
        {
            //输出所有喷头的温度

        }
        else if ( Arg->P == (HOTENDS) )
        {
            //输出热床的参数
        }
        else
        {
            //输出某一个喷头的参数
        }
    }

    return 1;
}

S32 GcodeM305Reply(VOID *ReplyResult, S8 *Buff)
{

    return 1;
}




