#include "GcodeM503Handle.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Common/Include/CrEepromApi.h"
#include "../../GcodeHandle.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"

#define CMD_M503 ("M503")

S32 GcodeM503Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    //rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M503, strlen(CMD_M503)) != 0)
        return 0;

    S32 Offset = strlen(CMD_M503) + sizeof(S8); //加1是越过分隔符
    struct ArgM503_t *CmdArg = (struct ArgM503_t *)Arg;

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
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atoi( Opt + 1 );

                break;
            }
            default:
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    if ( CmdArg->HasS )
    {
        CmdArg->HasS = true;
        CmdArg->S = true;
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM503Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM503_t *Arg = (struct ArgM503_t *)ArgPtr;
    if ( Arg->HasS )
        SettingReport(!(Arg->S));

    return GCODE_EXEC_OK;
}

S32 GcodeM503Reply(VOID *ReplyResult, S8 *Buff)
{

    return 0;
}
