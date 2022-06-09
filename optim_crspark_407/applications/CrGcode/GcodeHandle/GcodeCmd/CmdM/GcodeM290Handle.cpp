#include "GcodeM290Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "CrModel/CrHeaterManager.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"

#define CMD_M290 ("M290")

S32 GcodeM290Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    if ( strncmp(CmdBuff, CMD_M290, strlen(CMD_M290)) != 0 )  //传进来的命令不是G0命令
        return 0;

    return 1;
}

S32 GcodeM290Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    /*Babystep*/
    /*StepperBabyStepMove(enum Axis_t axis, FLOAT Dis)*/
    return 1;
}

S32 GcodeM290Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
