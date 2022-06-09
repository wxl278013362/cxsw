#include "../Include/CrTime.h"

S32 CrGetSystemTimeSecs()
{
    CrTimeSpec_t Ts;
    clock_gettime(CLOCK_REALTIME, &Ts);

    return Ts.tv_sec;
}

S64 CrGetSystemTimeMilSecs()
{
    CrTimeSpec_t Ts;
    clock_gettime(CLOCK_REALTIME, &Ts);

    S64 Time = Ts.tv_sec * 1000 + Ts.tv_nsec / 1000000;

    return Time;
}

S32 CrGetCurrentSystemTime(CrTimeSpec_t* Time)
{
    if ( !Time )
        return 0;

    clock_gettime(CLOCK_REALTIME, Time);

    return 1;
}

#ifdef TEST_TIME
#include <rtthread.h>
VOID TestSystemTime()
{
    int i = 0;
    CrTimeSpec_t Time;
    while ( i++ < 1000 )
    {
        CrGetCurrentSystemTime(&Time);
        //rt_kprintf("current system time secs = %d, us=%ld\n\r", Time.tv_sec, Time.tv_nsec / 1000);
        rt_kprintf("current system time secs = %d, us=%ld\n\r", Time.tv_sec, Time.tv_nsec );
        rt_kprintf("current system time secs = %d\n\r", CrGetSystemTimeSecs());
    }
}
#endif
