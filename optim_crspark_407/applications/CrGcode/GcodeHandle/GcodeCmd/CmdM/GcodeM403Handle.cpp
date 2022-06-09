#include "GcodeM403Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "CrModel/CrHeaterManager.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"

#define CMD_M403 ("M403")

S32 GcodeM403Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S8 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是m
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M403) != 0 )  //传进来的命令不是G0命令
        return 0;

    S8 Offset = strlen(CMD_M403) + sizeof(S8); //加1是越过分隔符
    struct ArgM403_t *CmdArg = (struct ArgM403_t *)Arg;

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
            case OPT_F:
                CmdArg->HasF = true;

                if (OptLen > 1)
                    CmdArg->F = atoi(Opt + 1);
            break;

            case OPT_E:
                CmdArg->HasE = true;

                if (OptLen > 1)
                    CmdArg->E = atoi(Opt + 1);
            break;

        }
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

S32 GcodeM403Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM403_t *CmdArg = (struct ArgM403_t *)ArgPtr;

    S8 Index = (S8)(( CmdArg->HasE == true ) ? CmdArg->E : -1 ),
       Type  = (S8)(( CmdArg->HasF == true ) ? CmdArg->F : -1 );

    if ( ((Index) >= (0) && (Index) <= (4)) && ((Type) >= (0) && (Type) <= (2)) )
    {
        //FIXME:设置材料插槽的灯丝类型
    }
    else
        rt_kprintf("M403 - bad arguments.");

    return 1;
}

S32 GcodeM403Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
