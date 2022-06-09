#include "GcodeG32Handle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "../../GcodeType.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_G32 "G32"

S32 GcodeG32Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    printf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_G32, strlen(CMD_G32)) != 0 )
    {
        return CrErr;
    }

    if ( strlen(CmdBuff) > (strlen(CMD_G32) + sizeof(S8)) )
    {
        printf("Error: G32 needs no parameter \n");
        return CrErr;
    }

    return GCODE_EXEC_OK;
}

S32 GcodeG32Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    if ( !SetDeployed(false) ) //收起探头
    {
        printf("Stow probe failed!!!\n");

        return CrErr;
    }

    return GCODE_EXEC_OK;
}

S32 GcodeG32Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

