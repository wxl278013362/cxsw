#include "stm32f4xx_hal.h"
#include "CrFanCtrl.h"

static S32 FanCtrl(S32 Fd, BOOL Level)
{
    struct PinParam_t Pin = {0};
    Pin.PinState = Level;
    return CrGpioWrite(Fd, (S8 *)&Pin, 0);
}

S32  CrFanOpen(S8 *Name, S32 Flag, S32 Mode)
{
    if ( !Name )
        return -1;

    S32 Fd =  CrGpioOpen(Name, Flag, Mode);

    struct PinInitParam_t Temp;
    Temp.Mode = GPIO_MODE_OUTPUT_PP;
    Temp.Pull = GPIO_NOPULL;
    Temp.Speed = GPIO_SPEED_FREQ_HIGH;

    CrGpioIoctl(Fd, GPIO_INIT, &Temp);

    return Fd;
}

S32  CrFanClose(S32 Fd)
{
    CrGpioClose(Fd);
    return 0;
}

S32  CrFanIoctl(S32 Fd, U32 Cmd, VOID *Arg)
{
    S32 Ret = -1;

    switch (Cmd) {
        case CMD_FAN_OPEN:
        case CMD_FAN_CLOSE:
        {
            Ret = FanCtrl(Fd, *(BOOL *)Arg);
            break;
        }
        default:
            break;
    }

    return Ret ;
}

#ifdef TEST_FANS
VOID FansOpen()
{
    S32 Fd = CrFanOpen("PA0", 0, 0);
    BOOL FanOpen = GPIO_PIN_SET;
    CrFanIoctl(Fd, CMD_FAN_OPEN, &FanOpen);
    return ;
}

//MSH_CMD_EXPORT(FansOpen, Fans Test)
#endif
