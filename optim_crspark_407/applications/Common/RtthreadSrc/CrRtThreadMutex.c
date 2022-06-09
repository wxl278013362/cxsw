#include "../Include/CrMutex.h"

U32 CrPthreadMutexInit(CrPthreadMutex_t  *Mutex, const CrPthreadMutexAttr_t *MutexAttr)
{
    return pthread_mutex_init(Mutex, MutexAttr);
}
//RTM_EXPORT(CrPthreadMutexInit);

U32 CrPthreadMutexLock(CrPthreadMutex_t  *Mutex)
{
    return pthread_mutex_lock(Mutex);
}
//RTM_EXPORT(CrPthreadMutexLock);

U32 CrPthreadMutexUnLock(CrPthreadMutex_t *Mutex)
{
    return pthread_mutex_unlock(Mutex);
}
//RTM_EXPORT(CrPthreadMutexUnLock);

U32 CrPthreadMutexDestroy(CrPthreadMutex_t *Mutex)
{
    return pthread_mutex_destroy(Mutex);
}
//RTM_EXPORT(CrPthreadMutexDestroy);


