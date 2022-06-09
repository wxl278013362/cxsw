#include "../Include/CrCommon.h"
#include <rthw.h>
#include <stdio.h>

VOID SystemReset()
{
    rt_hw_cpu_reset();
}


VOID PrintFloat(FLOAT Value, S8 *Buff)
{
    if ( !Buff )
        return;
    S32 Temp = (S32)Value;
    sprintf(Buff, "%d.%d", Temp, (S32)(Value - Temp)*1000);

    return;
}
