
#include "GcodeM997Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "Common/Include/CrCommon.h"

#define CMD_M997  "M997"

S32 GcodeM997Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M911
    if ( strcmp(Cmd, CMD_M997) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M997) + sizeof(S8); //加1是越过分隔符
    struct ArgM997_t *CmdArg = (struct ArgM997_t *)Arg;

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
                if ( OptLen > 1 ) CmdArg->S = atof( Opt + 1 );
                break;
            }
            default:
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    return 1;
 }

S32 GcodeM997Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM997_t *CmdArg = (struct ArgM997_t *)ArgPtr;
    if ( CmdArg->HasS )
    {
        SystemReset();
    }


    return 1;
}

S32 GcodeM997Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

