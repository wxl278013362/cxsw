#include "GcodeM380Handle.h"
#include "GcodeM381Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "../../../../CrGpio/CrGpio.h"

#define CMD_M381 ("M381")

static bool DisableAllSolenoidsFlag = false;
static bool ExtruderParked = true;

void Parking_Extruder_Set_Parked(const bool Parked)
{
    ExtruderParked = Parked;
}

static bool HasSolenoid(U8 SolenoidIndex)
{
    //TODO 检查是否有传入Index的挤出机

    return true;    //有，返回true,否则返回false
}

static void DisableSolenoidByIndex(U8 Index)
{
    S32 Fd;
    Fd = GetFdByExtruderIndex(Index);
    struct PinParam_t Pin = { GPIO_PIN_RESET };

    Pin.PinState = GPIO_PIN_SET;
    CrGpioWrite(Fd, (S8 *)&Pin, 0);

#ifdef PARKING_EXTRUDER
    Parking_Extruder_Set_Parked(true);    //If active extruder's solenoid is disabled, carriage is considered parked
#endif
}

static void DisableAllSolenoids()
{
    for ( U8 i = 0; i <= MAX_SOLENOID_COUNT; i++ )
    {
        if ( HasSolenoid(i) )
        {
            DisableSolenoidByIndex(i);
        }
    }
}

S32 GcodeM381Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M381, strlen(CMD_M381)) != 0 )
    {
        return CrErr;
    }

#ifndef MANUAL_SOLENOID_CONTROL
    DisableAllSolenoidsFlag = true;     //Deactivate all solenoids

#else
    struct ArgM381_t *CmdArg = (struct ArgM381_t *)Arg;
    S32 Offset = strlen(CMD_M381) + 1;  //加1是越过分隔符
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
            printf("Gcode M381 has no param opt = %s\n", CmdBuff);  //命令仅为M381, Deactivate active solenoids
            CmdArg->HasS = false;
            CmdArg->S = GetActiveExtruderIndex();

            return GCODE_EXEC_OK;
        }
        else if ( OptLen == 1 )
        {
            if ( Opt[0] == OPT_S )  //命令仅为M380 S, Deactivate active solenoids
            {
                CmdArg->HasS = true;
                CmdArg->S = GetActiveExtruderIndex();

                return GCODE_EXEC_OK;
            }
            else    //非M381 S,不合法
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
                printf("Gcode M381 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

#endif
    return GCODE_EXEC_OK;
}


S32 GcodeM381Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return 0;
    }

    if ( DisableAllSolenoidsFlag )
    {
        DisableAllSolenoids();
        DisableAllSolenoidsFlag = false;

        return 1;
    }

    struct ArgM381_t *Param = (struct ArgM381_t *)ArgPtr;
    if ( Param->S >= MAX_SOLENOID_COUNT )
    {
        return CrErr;
    }
    DisableSolenoidByIndex(Param->S);

    return 1;
}

S32 GcodeM381Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

