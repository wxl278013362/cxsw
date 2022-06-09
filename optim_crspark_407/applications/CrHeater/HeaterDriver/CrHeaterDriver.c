#include "stm32f4xx_hal.h"
#include "CrHeaterDriver.h"
#include "CrGpio/CrGpio.h"
#include "rtthread.h"    //MSH_CMD_EXPORT

S32 CrHeaterOpen(S8 *Name, S32 Flag, S32 Mode)
{
    S32 Fd = CrGpioOpen(Name, Flag, Mode);
    struct PinInitParam_t pin_init = {0};

    pin_init.Mode = GPIO_MODE_OUTPUT_PP;
    pin_init.Pull = GPIO_NOPULL;
    pin_init.Speed = GPIO_SPEED_FREQ_HIGH;


    CrGpioIoctl(Fd, GPIO_INIT, &pin_init);

    return Fd;
}

S32 CrHeaterClose(S32 Fd)
{
    CrGpioClose(Fd);
    return 0;
}

S32 CrHeaterIoctl(S32 Fd, U32 Cmd, VOID* Arg)
{
    S32 Ret = -1;
    if ( !Arg )
        return Ret;

    switch ( Cmd )
    {
        case HEATER_HEAT_POWERON:
        case HEATER_HEAT_POWEROFF:
        {
            struct PinParam_t Level = {0};
            Level.PinState = *(S32 *)Arg;
            Ret = CrGpioIoctl(Fd, GPIO_SET_PINSTATE, &Level);
            break;
        }
    }

    return Ret;
}

VOID HeaterOpen(VOID)
{
    S32 Fd = CrHeaterOpen(BED_HEATTING_PIN_NAME, 0, 0);
    S32 level = 1;
    CrHeaterIoctl(Fd, HEATER_HEAT_POWERON, &level);
}

VOID HeaterClose(VOID)
{
    S32 Fd = CrHeaterOpen(BED_HEATTING_PIN_NAME, 0, 0);
    S32 level = 0;
    CrHeaterIoctl(Fd, HEATER_HEAT_POWERON, &level);
}
MSH_CMD_EXPORT(HeaterOpen,  Heater Test)
MSH_CMD_EXPORT(HeaterClose,  Heater Test)

