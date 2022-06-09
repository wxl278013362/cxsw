#include "GcodeM16Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"

#include "CrInc/CrConfig.h"

#define CMD_M16 ("M16")

S32 GcodeM16Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S8 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M16) != 0 )  //传进来的命令不是G0命令
        return 0;

    S8 Offset = strlen(CMD_M16) + sizeof(S8); //加1是越过分隔符
    struct ArgM16_t *CmdArg = (struct ArgM16_t *)Arg;

    memcmp(CmdArg->Str, CmdBuff + Offset, strlen(CmdBuff) - Offset );

    return 1;
}

S32 GcodeM16Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM16_t *Param = (struct ArgM16_t *)ArgPtr;


    if (0 != strcmp(Param->Str, MACHINE_NAME))
        rt_kprintf("ERROR:\n");

    return 1;
}

S32 GcodeM16Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
