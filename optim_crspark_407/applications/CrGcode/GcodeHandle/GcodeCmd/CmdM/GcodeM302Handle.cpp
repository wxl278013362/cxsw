#include "GcodeM302Handle.h"
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrHeaterManager.h"

#define CMD_M302           "M302"

S32 GcodeM302Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0] != GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M302) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M302) + sizeof(S8); // 加1是越过分隔符
    struct ArgM302_t *CmdArg = (struct ArgM302_t *)Arg;

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
            case OPT_P:
            {
                CmdArg->HasP = true;
                if ( OptLen > 1 ) CmdArg->P = atoi( Opt + 1 );
                break;
            }
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


S32 GcodeM302Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM302_t *Arg = (struct ArgM302_t *)ArgPtr;
    if ( Arg->HasS )
    {
        CrHeaterManager::SetExtrudeMinTemper(Arg->S);
        CrHeaterManager::SetColdExtrude(CrHeaterManager::GetExtrudeMinTemper() == 0);
    }

    if ( Arg->HasP )
    {
        CrHeaterManager::SetColdExtrude( (CrHeaterManager::GetExtrudeMinTemper() == 0) || (Arg->P) );
    }
    else if ( !Arg->HasS )
    {
        printf("Cold extrudes are ");
        printf(CrHeaterManager::GetColdExtrude() ? "en" : "dis");
        printf("abled (min temp %d C )\n", CrHeaterManager::GetExtrudeMinTemper());
    }

    return 1;
}

S32 GcodeM302Reply(VOID *ReplyResult, S8 *Buff)
{

    return 1;
}




