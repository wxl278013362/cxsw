#include "GcodeM380Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"

#define CMD_M380 ("M380")

static U8 Active_Extruder = 0;

U8 GetActiveExtruderIndex(void)
{
    return Active_Extruder;
}

S32 GetFdByExtruderIndex(U8 ActiveExtruderIndex)
{
    S32 Fd;
    //TODO根据挤出机编号获取pin脚的fd

    return Fd;
}

S32 GcodeM380Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M380, strlen(CMD_M380)) != 0 )
    {
        return 0;
    }

    struct ArgM380_t *CmdArg = (struct ArgM380_t *)Arg;
#ifndef MANUAL_SOLENOID_CONTROL
    CmdArg->HasS = false;
    CmdArg->S = Active_Extruder;

#else
    S32 Offset = strlen(CMD_M380) + 1; //加1是越过分隔符
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen == 0 )
        {
            printf("Gcode M380 has no param opt = %s\n", CmdBuff);  //命令仅为M380
            CmdArg->HasS = false;
            CmdArg->S = Active_Extruder;

            return GCODE_EXEC_OK;
        }
        else if ( OptLen == 1 )
        {
            if ( Opt[0] == OPT_S )  //命令仅为M380 S
            {
                CmdArg->HasS = true;
                CmdArg->S = Active_Extruder;

                return GCODE_EXEC_OK;
            }
            else    //非M380 S,不合法
            {
                return CrErr;
            }
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
                printf("Gcode M380 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

#endif
    return GCODE_EXEC_OK;
}

S32 GcodeM380Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    struct ArgM380_t *Param = (struct ArgM380_t *)ArgPtr;
    if ( Param->S >= MAX_SOLENOID_COUNT )
    {
        return CrErr;
    }

    S32 Fd = GetFdByExtruderIndex(Param->S);
    struct PinParam_t Pin = { GPIO_PIN_RESET };

    Pin.PinState = GPIO_PIN_RESET;
    if ( CrGpioWrite(Fd, (S8 *)&Pin, 0) )
    {
        printf("%s command exec failed!!!\n", CMD_M380);

        return CrErr;
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM380Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}
