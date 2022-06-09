#include "GcodeT_Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"

#define CMD_T            "T"

S32 GcodeCmdT_Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;


    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_T) )   //Gcode命令的长度大于1,且是G
        return 0;
    

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;
    struct ArgT_t *Tool = (struct ArgT_t *)Arg;
    Tool->T_Num = atoi((Cmd + 1));
    
    return 1;
}

S32 GcodeCmdT_Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
        return 0;

    struct ArgT_t *Cmd = (struct ArgT_t *)ArgPtr;
    printf("current Tool num = %d\n", Cmd->T_Num);

    return 1;
}

S32 GcodeCmdT_Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


