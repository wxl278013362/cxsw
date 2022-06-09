
#ifndef _CRLOG_H
#define _CRLOG_H


#include "Common/Include/CrMutex.h"
#include "CrInc/CrType.h"
#include <string.h>
#include <stdio.h>


#define PATH_LENGTH 80


#define  LogOutToUart    (1 << 0)
#define  LogOutToFile    (1 << 1)
#define  LogOutToVcp     (1 << 2)
#define  LogOutToTerm    (1 << 3)


struct CrLogAttr_t
{
    U32 UsingMutex;
    U32 OutPutDevice;
    U8  UartDevName[PATH_LENGTH]; //文件路径(最后一个是字符'/')
    U8  FileName[PATH_LENGTH];    //文件路径(最后一个是字符'/')
    U8  VcpName[PATH_LENGTH];     //文件路径(最后一个是字符'/')
    U8  TermName[PATH_LENGTH];    //文件路径(最后一个是字符'/')
};

#define LOG_ATTR_SET
#define LOG_ATTR_GET

//open接口对应的写入文件的比特位
#define LOG_TO_UART       (0x01 << 0)
#define LOG_TO_FILE       (0x01 << 1)
#define LOG_TO_VCP        (0x01 << 2)
#define LOG_TO_TERM       (0x01 << 3)


#ifdef __cplusplus
extern "C" {
#endif

U32 CrLogOpen(S8 *LogPath, S32 Flags);    //Flag,加入使用LOG_TO_FILE
U32 CrLogIoctl();  //调用初始化
U32 CrLogWrite(S8 *Buff, U32 Size);
U32 CrLogClose();

U32 CrSetLogAttr(struct CrLogAttr_t* Attr);

struct CrLogAttr_t* CrGetLogAttr();



U32 CrLogLock();
U32 CrLogUnLock();

//#define LOG_BUFF_SIZE 200
//static S8 CrLogBuff[LOG_BUFF_SIZE];


//注意snprintf()中拼接字符转的地方不要出现空格,可能会导致出错
//#define LOG(format, arg...)   do \
//        { \
//            CrLogLock();   \
//            snprintf(CrLogBuff, LOG_BUFF_SIZE,"%s<%s>:%d-" format "\r\n",__FILE__, __func__, __LINE__, ##arg);   \
//            CrLogWrite(CrLogBuff,  strlen(CrLogBuff));                                    \
//            CrLogUnLock();\
//        } while(0)




#ifdef __cplusplus
}
#endif


#endif





