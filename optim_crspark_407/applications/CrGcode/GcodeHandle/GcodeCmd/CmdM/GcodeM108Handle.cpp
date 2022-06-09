#include "GcodeM108Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "CrModel/CrHeaterManager.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"

#define CMD_M108 ("M108")

static CrHeaterManager *Bed = NULL;
static CrHeaterManager *Noz = NULL;


VOID GcodeM108Init(VOID *HeaterBed, VOID *HeaterNoz)
{
    Bed = (CrHeaterManager *)HeaterBed;
    Noz = (CrHeaterManager *)HeaterNoz;
}


S32 GcodeM108Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    if ( strncmp(CmdBuff, CMD_M108, strlen(CMD_M108)) != 0 )  //传进来的命令不是G0命令
        return 0;

    return 1;
}

S32 GcodeM108Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    Bed->SetWaitForHeat(0);
    Noz->SetWaitForHeat(0);

    return 1;
}

S32 GcodeM108Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
