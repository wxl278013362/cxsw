#include "../Include/CrThread.h"

S32 CrPthreadCreate(CrPthread_t *Thread, CrPthreadAttr_t *Attr, VOID *(*CallBack)(VOID *), VOID *Arg)
{
    return pthread_create(Thread, Attr, CallBack, Arg);
}

//RTM_EXPORT(CrPthreadCreate);

S32 CrPthreadDetach(CrPthread_t Thread)
{
    return pthread_detach(Thread);
}
//RTM_EXPORT(CrPthreadDetach);

S32 CrPthreadJoin(CrPthread_t Thread, VOID **Retval)
{
    return pthread_join(Thread, Retval);
}
//RTM_EXPORT(CrPthreadJoin);

CrPthread_t CrPthreadSelf()
{
    return pthread_self();
}
//RTM_EXPORT(CrPthreadSelf);

VOID CrPthreadExit(VOID *Retval)
{
    return pthread_exit(Retval);
}
//RTM_EXPORT(CrPthreadExit);

S32  CrPthreadKill(CrPthread_t Thread, U32 Sig)
{
    return pthread_kill(Thread, Sig);
}
//RTM_EXPORT(CrPthreadKill);

S32  CrPthreadCancel(CrPthread_t Thread)
{
    return pthread_cancel(Thread);
}
//RTM_EXPORT(CrPthreadCancel);

S32  CrPthreadEqual(CrPthread_t Thread, CrPthread_t Thread2)
{
    return pthread_equal(Thread, Thread2);
}
//RTM_EXPORT(CrPthreadEqual);

VOID CrPthreadCleanupPush(VOID (*Routine)(VOID *), VOID *Arg)
{
    return pthread_cleanup_push(Routine, Arg);
}
//RTM_EXPORT(CrPthreadCleanupPush);

VOID CrPthreadCleanupPop(S32 Execute)
{
    return pthread_cleanup_pop(Execute);
}
//RTM_EXPORT(CrPthreadCleanupPop);

S32 CrPthreadAttrDestroy(CrPthreadAttr_t *Attr)
{
    return pthread_attr_destroy(Attr);
}
S32 CrPthreadAttrInit(CrPthreadAttr_t *Attr)
{
    return pthread_attr_init(Attr);
}

S32 CrPthreadAttrSetStackSize(CrPthreadAttr_t *Attr, S32 StackSize)
{
    return pthread_attr_setstacksize(Attr, StackSize);
}

S32 CrPthreadAttrGetStackSize(CrPthreadAttr_t const *Attr, S32 *StackSize)
{
    return pthread_attr_getstacksize(Attr, (size_t *)StackSize);
}

S32 CrPthreadAttrSetStackAddr(CrPthreadAttr_t *Attr, VOID *StackAddr)
{
    return pthread_attr_setstackaddr(Attr, StackAddr);
}

S32 CrPthreadAttrSetDetachState(CrPthreadAttr_t *Attr, S32 State)
{
    return pthread_attr_setdetachstate(Attr, State);
}

S32 CrPthreadAttrGetDetachState(CrPthreadAttr_t *Attr, S32 *State)
{
    return pthread_attr_getdetachstate(Attr, State);
}

S32 CrPthreadAttrGetStackAddr(CrPthreadAttr_t const *Attr, VOID **StackAddr)
{
    return pthread_attr_getstackaddr(Attr, StackAddr);
}

S32 CrPthreadAttrSetStack(CrPthreadAttr_t *Attr, VOID *StackBase, S32 StackSize)
{
    return pthread_attr_setstack(Attr, StackBase, StackSize);
}

S32 CrPthreadAttrGetStack(CrPthreadAttr_t const *Attr, VOID **StackBase, S32 *StackSize)
{
    return pthread_attr_getstack(Attr, StackBase, (size_t *)StackSize);
}

S32 ThreadStackCheck(S8 *StackAddr, S32 StackLen)
{
    if ( !StackAddr || (StackLen <= 0) )
        return 0;
    int i = 0, Len = StackLen / 4;
    for ( i = 0; i < Len; i++ )
    {
        S32 *Value = (S32*)(StackAddr + 4 * i);
        if ( *Value != THREAD_CHECK_VALUE )
            break;
    }

    return StackLen - i * 4;
}

#ifdef TEST_THREAD
S32 Total = 0;

static VOID* CallBack(VOID* Arg)
{
    S32 Detach = 0;
    if ( CrPthreadDetach(CrPthreadSelf()) )
    {
        rt_kprintf("sub thread1 detach from main thread failed!\n");
    }
    else
    {
        Detach = 1;
        rt_kprintf("**********sub thread1 detach from main thread success!*************\n");
    }

    if ( Detach )
    {
        S32 Res = 0;
        rt_kprintf("sub thread1 do exit action!\n");
        CrPthreadExit(&Res);
        if ( Res )
        {
            rt_kprintf("sub thread1 exit failed!\n");
        }
        else
        {
            rt_kprintf("sub thread1 exit success!\n");
        }
    }

    return NULL;
}

VOID* Thread_CallBack(VOID* Arg)
{
    S32 i = 0;

    while ( i++ < 20 )
    {
        rt_kprintf("hello sub thread, Total value = %d !\n\r", Total);
        Total += 2;
        rt_thread_mdelay(1000);
    }

    rt_kprintf("sub thread action finished!\n");

    return NULL;
}

VOID TestPthread()
{
    CrPthread_t Thread, Thread1;
    S32 Res = CrPthreadCreate(&Thread, NULL, Thread_CallBack, NULL);
    if ( Res == 0 )
        rt_kprintf("\n\r create thread success!\n\r");

    Res = CrPthreadCreate(&Thread1, NULL, CallBack, NULL);
    if ( Res == 0 )
       rt_kprintf("create thread1 success!\n\r");

    S32 Count = 0;
    while ( Count++ < 10)
    {
        rt_kprintf("Hello RT-Thread, Total value = %d !\n", Total);
        Total += 1;
        rt_thread_mdelay(1000);
    }

    rt_kprintf("main thread action finished, wait sub thread finished!\n\r");

    int KillRes = CrPthreadKill(Thread, 10);   //发送信号给
    if ( KillRes )
    {
        rt_kprintf("main thread kill sub thread failed error = %d\n\r", KillRes);
    }
    else
    {
        rt_kprintf("main thread kill sub thread success!\n\r");
    }

    void* Result = NULL;
#if 0
    if ( CrPthreadJoin(Thread1, &Result) == 0 )   //如果在子线程中做了线程分离，会出现Join失败的情况
    {
        rt_kprintf("sub thread1 finished\n\r");
    }
    else
    {
        rt_kprintf("sub thread1 finish error = %d\n\r", *((int*)Result));
    }
#endif
    Result = NULL;
//    if ( pthread_join(&Thread, &Result) == 0 )   //注意传的是变量自身，不是变量的指针
    if ( CrPthreadJoin(Thread, &Result) == 0 )   //如果在子线程中做了线程分离，会出现Join失败的情况
    {
        rt_kprintf("sub thread finished\n\r");
    }
    else
    {
        rt_kprintf("sub thread finish error = %d\n\r", *((int*)Result));
    }
}
//RTM_EXPORT(TestPthread);
#endif
