#include "GcodeM226Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrGpio/CrGpio.h"
#include "Common/Include/CrSleep.h"


#define CMD_M226  "M226"

#define HIGH         0x01
#define LOW          0x00

S32 GcodeM226Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M205
    if ( strcmp(Cmd, CMD_M226) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M226) + sizeof(S8);
    struct ArgM226_t *CmdArg = (struct ArgM226_t *)Arg;
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) ) break;

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )
        {
            case OPT_P:
                CmdArg->HasP = true;
                if ( OptLen > 1 ) CmdArg->P = atoi(Opt + 1);
                break;

            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atoi(Opt + 1);
                break;

            default:
                printf("Warn:unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //�˴���1�Ƿָ���
    }

    if ( !CmdArg->HasS )
    {
        CmdArg->HasS = true;
        CmdArg->S = -1;
    }

    return 1;
}

S32 GcodeM226Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel)
        return 0;

    struct ArgM226_t *Arg = (struct ArgM226_t *)ArgPtr;
    if ( !Arg->HasP || (Arg->P < 0))
    {
        return 1;
    }

    S32 Fd = 0;
    S8 PinName[10] = {0};
    //根据
    CrGpioGetPinName(Arg->P, PinName);

    Fd = CrGpioOpen(PinName, 0, 0);
    if ( (Fd & 0xFFFF0000) <= 0)
        return 1;

    //判断引脚是否是受保护引脚，如果是就直接返回


    //引脚不是受保护引脚
    if ( Arg->HasS )
    {
        S32 Status = LOW;

        while ( !MotionModel->IsMotionMoveFinished() )
            CrM_Sleep(5);

        struct PinInitParam_t Param;
        Param.Mode = GPIO_PIN_MODE_INPUT;
        Param.Pull = GPIO_PIN_PULLUP;
        Param.Speed =GPIO_PIN_SPEED_FREQ_MEDIUM;
        CrGpioIoctl(Fd, GPIO_INIT, (VOID *)&Param);

        switch ( Arg->S )
        {
        case 1: Status = HIGH; break;
        case 0: Status = LOW; break;
        default:
            {
                struct PinParam_t Param;
                CrGpioIoctl(Fd, GPIO_GET_PINSTATE, &Param);
                Status = !Param.PinState;
                break;
            }
        }

        struct PinParam_t Param1;
        while (1)
        {
            CrGpioIoctl(Fd, GPIO_GET_PINSTATE, &Param1);
            if ( Param1.PinState == Status )
                break;

            CrM_Sleep(5);
        }
    }

    CrGpioClose(Fd);

    return 1;
}

S32 GcodeM226Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
