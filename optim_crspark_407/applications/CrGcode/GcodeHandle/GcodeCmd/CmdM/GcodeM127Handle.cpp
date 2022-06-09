#include "GcodeM127Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"

#if HAS_HEATER_0

#define CMD_M127 ("M127")

S32 GcodeM127Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M127, strlen(CMD_M127)) != 0 )
    {
        return 0;
    }

    return 1;
}

S32 GcodeM127Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return 0;
    }

    BaricudavalvePressure = 0;

    return 1;
}

S32 GcodeM127Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

#endif

