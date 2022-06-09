#ifndef _CRTHREAD_H
#define _CRTHREAD_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"



#ifdef USING_OS_LINUX

#include <pthread.h>
typedef pthread__t CrPthread_t;
typedef pthread_attr_t CrPthreadAttr_t;

#endif

#ifdef USING_OS_RTTHREAD

#include <pthread.h>
typedef pthread_t CrPthread_t;
typedef pthread_attr_t CrPthreadAttr_t;

#endif

#define THREAD_BYTE_INIT_VALUE (0x23)
#define THREAD_CHECK_VALUE (0x23232323)

#ifdef __cplusplus
extern "C" {
#endif

S32 CrPthreadCreate(CrPthread_t *Thread, CrPthreadAttr_t *Attr, VOID *(*CallBack)(VOID *), VOID *Arg);   //创建线程
S32 CrPthreadDetach(CrPthread_t Thread);   //分离线程
S32 CrPthreadJoin(CrPthread_t Thread, VOID **Retval);  //等待线程结束
CrPthread_t CrPthreadSelf();    //获取当前线程
VOID CrPthreadExit(VOID *Retval);   //线程退出
S32  CrPthreadKill(CrPthread_t Thread, U32 Sig);   //发送信号杀死线程
S32  CrPthreadCancel(CrPthread_t Thread);   //取消线程
S32  CrPthreadEqual(CrPthread_t Thread, CrPthread_t Thread2);   //线程相等
VOID CrPthreadCleanupPush(VOID (*Routine)(VOID *), VOID *Arg);   //将线程的清理资源函数加入到清理处理栈中
VOID CrPthreadCleanupPop(S32 Execute);   //将清理处理栈的顶部元素出栈

S32 CrPthreadAttrInit(CrPthreadAttr_t *Attr);     //初始化属性
S32 CrPthreadAttrDestroy(CrPthreadAttr_t *Attr);  //清除属性
S32 CrPthreadAttrSetStackSize(CrPthreadAttr_t *Attr, S32 StackSize);   //设置堆栈大小
S32 CrPthreadAttrGetStackSize(CrPthreadAttr_t const *Attr, S32 *StackSize);  //获取堆栈大小
S32 CrPthreadAttrSetStackAddr(CrPthreadAttr_t *Attr, VOID *StackAddr);   //设置堆栈地址
S32 CrPthreadAttrGetStackAddr(CrPthreadAttr_t const *Attr, VOID **StackAddr);  //获取堆栈地址
S32 CrPthreadAttrSetStack(CrPthreadAttr_t *Attr, VOID *StackBase, S32 StackSize);   //设置堆栈信息
S32 CrPthreadAttrGetStack(CrPthreadAttr_t const *Attr, VOID **StackBase, S32 *StackSize);  //获取堆栈信息
S32 CrPthreadAttrSetDetachState(CrPthreadAttr_t *Attr, S32 State);
S32 CrPthreadAttrGetDetachState(CrPthreadAttr_t *Attr, S32 *State);

S32 ThreadStackCheck(S8 *StackAddr, S32 StackLen);


#ifdef TEST_THREAD
    VOID TestPthread(); //测试线程接口
#endif


#ifdef __cplusplus
}
#endif

#endif
