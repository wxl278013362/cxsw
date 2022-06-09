#include "GcodeM672Handle.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrSleep.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CMD_M672 ("M672")
#define M672_PROGBYTE           105                // magic byte to start programming custom sensitivity
#define M672_ERASEBYTE          131                // magic byte to clear custom sensitivity
#define SMART_EFFECTOR_MOD_PIN  -1

#ifndef HIGH
#define HIGH         0x01
#endif

#ifndef LOW
#define LOW          0x00
#endif

static VOID M673OutWrite(S32 Fd, S8 Level)
{
    struct PinInitParam_t Param;
    Param.Mode = GPIO_PIN_MODE_OUTPUT_PP;
    Param.Pull = GPIO_NOPULL;
    Param.Speed =GPIO_PIN_SPEED_FREQ_MEDIUM;
    CrGpioIoctl(Fd, GPIO_SET_PINSTATE, (VOID *)&Param);

    struct PinParam_t Pin = {GPIO_PIN_RESET};
    Pin.PinState = (GPIO_PinState)Level;
    CrGpioWrite(Fd, (S8 *)&Pin, 0);
}

static VOID M672Send(S8 b)
{
    for ( U8 i = b; i < 14; i++ )
    {
        switch ( i )
        {
            case 0:
            case 1:
            case 3:
                M673OutWrite(SMART_EFFECTOR_MOD_PIN, LOW);
                break;

            case 2:
                M673OutWrite(SMART_EFFECTOR_MOD_PIN, HIGH);
                break;

            case 7:
            case 12:
                M673OutWrite(SMART_EFFECTOR_MOD_PIN, !!(b & 0x80));
                break;

            case 8:
            case 13:
                M673OutWrite(SMART_EFFECTOR_MOD_PIN,  !(b & 0x80));
                b <<= 1;
                break;

            default:
                M673OutWrite(SMART_EFFECTOR_MOD_PIN, !!(b & 0x80));
                b <<= 1;
                break;
        }
        CrU_Sleep(1000);
    }
}

S32 GcodeM672Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M672, strlen(CMD_M672)) != 0 )
    {
        return CrErr;
    }

    struct ArgM672_t *CmdArg = (struct ArgM672_t *)Arg;
    S32 Offset = strlen(CMD_M672) + 1; //加1是越过分隔符
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
            printf("Gcode M672 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_R:
                CmdArg->HasR = true;
                if ( OptLen == 1 )
                {
                    CmdArg->R = 0;
                }
                else
                {
                    CmdArg->R = atoi(Opt + 1);
                }
                break;

            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueS = false;
                }
                else
                {
                    CmdArg->HasValueS = true;
                    CmdArg->S = atoi(Opt + 1);
                }
                break;

            default:
                printf("Gcode M672 has no define opt = %s\n", CmdBuff);
                break;
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}


S32 GcodeM672Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    struct ArgM672_t *Param = (struct ArgM672_t *)ArgPtr;
    if ( Param->HasR )
    {
        M672Send(M672_ERASEBYTE);
        M672Send(M672_ERASEBYTE);
    }
    else if ( Param->HasS && Param->HasValueS )
    {
        const S8 M672Sensitivity = Param->S;
        M672Send(M672_PROGBYTE);
        M672Send(M672Sensitivity);
        M672Send(255- M672Sensitivity);
    }
    else
    {
        printf("!'S' or 'R' parameter required.");
        return CrErr;
    }

    M673OutWrite(SMART_EFFECTOR_MOD_PIN, LOW);
}

S32 GcodeM672Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

