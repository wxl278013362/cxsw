#ifndef _CRTIME_H
#define _CRTIME_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef USING_OS_LINUX

#include <time.h>    //Posix的消息队列
typedef struct timespec CrTimeSpec_t;

#endif

#ifdef USING_OS_RTTHREAD
#include <time.h>
#include <clock_time.h>
typedef struct timespec CrTimeSpec_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

S32 CrGetCurrentSystemTime(CrTimeSpec_t* Time);  //获取当前系统时间
S32 CrGetSystemTimeSecs();   //获取系统的当前时间的秒数，从1970-01-01 0:0:0开始
S64 CrGetSystemTimeMilSecs();  ////获取系统的当前时间的毫秒数，从1970-01-01 0:0:0开始

#ifdef TEST_TIME
VOID   TestSystemTime();
#endif

#ifdef __cplusplus
}
#endif

#endif
