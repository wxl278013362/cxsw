#include "GcodeM129Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"

#if HAS_HEATER_1

#define CMD_M129 ("M129")

S32 GcodeM129Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M129, strlen(CMD_M129)) != 0 )
    {
        return 0;
    }

    return 1;
}

S32 GcodeM129Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return 0;
    }

    baricuda_e_to_p_pressure = 0;

    return 1;
}

S32 GcodeM129Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

#endif


