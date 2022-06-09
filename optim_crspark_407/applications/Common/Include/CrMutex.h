#ifndef  _CRMUTEX_H
#define _CRMUTEX_H


/********

    跨平台

    USING_OS_LINUX
    USING_OS_WINDOWS
    USING_OS_MOC



***************/

#define CR_MULTI_THREAD_SUPPORT (0x01 << 24)   //需要使用锁的标志

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef USING_OS_LINUX

#include <pthread.h>
typedef pthread_mutex_t CrPthreadMutex_t;
typedef pthread_mutexattr_t CrPthreadMutexAttr_t;

#endif

#ifdef USING_OS_RTTHREAD

#include <pthread.h>
typedef pthread_mutex_t CrPthreadMutex_t;
typedef pthread_mutexattr_t CrPthreadMutexAttr_t;

#endif

#ifdef __cplusplus
extern "C" {
#endif


U32 CrPthreadMutexInit(CrPthreadMutex_t  *Mutex, const CrPthreadMutexAttr_t *MutexAttr);

U32 CrPthreadMutexLock(CrPthreadMutex_t  *Mutex);
U32 CrPthreadMutexUnLock(CrPthreadMutex_t *Mutex);

U32 CrPthreadMutexDestroy(CrPthreadMutex_t *Mutex);


#ifdef __cplusplus
}
#endif



#endif
