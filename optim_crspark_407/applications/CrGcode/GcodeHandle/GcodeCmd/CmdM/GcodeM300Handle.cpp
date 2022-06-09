#include "GcodeM300Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"

#define CMD_M300 ("M300")

S32 GcodeM300Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S8 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M300) != 0 )  //传进来的命令不是G0命令
        return 0;

    S8 Offset = strlen(CMD_M300) + sizeof(S8); //加1是越过分隔符
    struct ArgM300_t *CmdArg = (struct ArgM300_t *)Arg;

    S8 Opt[20] = {0};
    S8 OptLen = 0;

    while ( Offset < Len )
    {
        memset(Opt, 0, sizeof(Opt));
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
            break;    //获取参数失败

        OptLen = strlen(Opt);

        switch ( Opt[0] )
        {
            case OPT_P:

                CmdArg->HasP = true;

                if (OptLen > 1)
                    CmdArg->P = atoi(Opt + 1);
            break;

            case OPT_S:

                CmdArg->HasS = true;

                if (OptLen > 1)
                {
                    CmdArg->S = atoi(Opt + 1);
                }
            break;

        }
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

S32 GcodeM300Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM300_t *Param = (struct ArgM300_t *)ArgPtr;

    CrBeepAppFlexsibleBeeps(Param->P);

    return 1;
}

S32 GcodeM300Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
