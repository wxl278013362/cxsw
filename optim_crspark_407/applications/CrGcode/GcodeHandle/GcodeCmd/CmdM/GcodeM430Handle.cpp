#include "GcodeM430Handle.h"
#include "CrModel/CrMotion.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include "CrInc/CrMsgType.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef HAS_POWER_MONITOR

#define CMD_M430 ("M430")

static VOID SetCurrentDisplay(VOID)
{
    //TODO...
}

static VOID SetVoltageDisplay(VOID)
{
    //TODO...
}

static VOID SetPowerDisplay(VOID)
{
    //TODO...
}

static FLOAT GetAmps(VOID)
{
    return 0.0f;
}

static FLOAT GetVolts(VOID)
{
    return 0.0f;
}

static FLOAT GetPower(VOID)
{
    return 0.0f;
}

S32 GcodeM430Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M430, strlen(CMD_M430)) != 0 )
    {
        return CrErr;
    }

    struct ArgM430_t *CmdArg = (struct ArgM430_t *)Arg;
    S32 Offset = strlen(CMD_M430) + 1; //加1是越过分隔符
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
            printf("Gcode M430 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_I:
                CmdArg->HasI = true;
                if ( OptLen == 1 )
                {
                    CmdArg->I = false;
                }
                else
                {
                    CmdArg->I = atoi(Opt + 1);
                }
                break;

            case OPT_V:
                CmdArg->HasV = true;
                if ( OptLen == 1 )
                {
                    CmdArg->V = false;
                }
                else
                {
                    CmdArg->V = atoi(Opt + 1);
                }
                break;

            case OPT_W:
                CmdArg->HasW = true;
                if ( OptLen == 1 )
                {
                    CmdArg->W = false;
                }
                else
                {
                    CmdArg->W = atoi(Opt + 1);
                }
                break;

            default:
                printf("Gcode M430 has no define opt = %s\n", CmdBuff);
                break;
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;

}

S32 GcodeM430Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    bool DoReport = true;
    struct ArgM430_t *Param = (struct ArgM430_t *)ArgPtr;
#ifdef HAS_WIRED_LCD
    #ifdef POWER_MONITOR_CURRENT
        if ( Param->HasI && Param->I )
        {
            //display current (A) on LCD
            SetCurrentDisplay();
            DoReport = false;
        }
    #endif

    #ifdef POWER_MONITOR_VOLTAGE
        if ( Param->HasV && Param->V )
        {
            //toggle display voltage (V) on LCD
            SetVoltageDisplay();
            DoReport = false;
        }
    #endif

    #ifdef HAS_POWER_MONITOR_WATTS
        if ( Param->HasW && Param->W )
        {
            //display power/watts (W) on LCD
            SetPowerDisplay();
            DoReport = false;
        }
    #endif
#endif

    if ( DoReport )
    {
        //report current LCD display power draw

    #ifdef POWER_MONITOR_CURRENT
        //print electric current
        printf("%4f\n", GetAmps());
    #endif

    #ifdef POWER_MONITOR_VOLTAGE
        //print voltage
        printf("%4f\n", GetVolts());
    #endif

    #ifdef HAS_POWER_MONITOR_WATTS
        //print power/watts
        printf("%4f\n", GetPower());
    #endif
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM430Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}


#endif
