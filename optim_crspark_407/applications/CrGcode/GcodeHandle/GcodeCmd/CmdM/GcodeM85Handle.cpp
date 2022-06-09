#include "GcodeM85Handle.h"
#include "../CmdG/GcodeG6Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include "Common/Include/CrTime.h"

/**
* Use this command to set a maximum period of time for the machine to be inactive (with no moves).
* If the machine is idle for longer than the set period, the firmware will shut everything down and halt the machine.
* To disable set zero (default)
*/
#define CMD_M85 ("M85")

static U32 MaxInactiveTime = 15 * 60 * 1000UL;    //default: 15minutes

U32 GetMaxInactiveTime(void)
{
    return MaxInactiveTime;
}

S32 GcodeM85Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M85, strlen(CMD_M85)) != 0 )
    {
        return 0;
    }

    struct ArgM85_t *CmdArg = (struct ArgM85_t *)Arg;
    S32 Offset = strlen(CMD_M85) + 1; //加1是越过分隔符
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M85 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_S:
            {
                CmdArg->HasS = true;
                CmdArg->S = atoi(Opt + 1);
                break;
            }
            default:
            {
                printf("Gcode M85 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM85Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return 0;
    }

    struct ArgM85_t *Param = (struct ArgM85_t *)ArgPtr;
    if ( !Param->HasS )
    {
        return CrErr;
    }

    ResetStepperTimeOut();
    MaxInactiveTime = (U32)Param->S * 1000UL;

    return GCODE_EXEC_OK;
}

S32 GcodeM85Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

