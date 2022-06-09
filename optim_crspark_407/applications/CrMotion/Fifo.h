////////////////////////////////////////////////////////////////
// FileName:    fifo.h
// Description: 
// Copyright:    Creality(创想三维)
// By:            
// LastUpdate:    2021.05
// Version:        V1.0
// History:
// Memo:
////////////////////////////////////////////////////////////////

#ifndef __FIFO_H__
#define __FIFO_H__

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C"{
#endif


typedef struct
{
    U8 *FifoBuf;
    U8 *BufHead;
    U8 *BufTail;
    U32 ReadIndex;
    U32 WriteIndex;
    U32 Fifolen;
    U32 FifoSingleSize;
    U32 FifoLeftSlot;
} CommonFiFo_t;

S32 CommonCreateFifo(CommonFiFo_t *CommonFifo,VOID *FifoBuf,U32 SingleSize, U32 FifoLen);

S32 CommonReadFifo(CommonFiFo_t *CommonFifo,VOID *ReadBuf,U32 ReadLen);             //线程安全

S32 CommonWriteFifo(CommonFiFo_t *CommonFifo,VOID *WriteBuf,U32 WriteLen);          //线程安全

S32 CommonIsFullFifo(CommonFiFo_t *CommonFifo);

S32 CommonIsEmptyFifo(CommonFiFo_t *CommonFifo);

S32 CommonRstFifo(CommonFiFo_t *CommonFifo);

S32 CommonDestoryFifo(CommonFiFo_t *CommonFifo);

#if 0
// FIFO的管理结构，用户不需对其内部结构成员变量进行操作。
//      只需定义Fifo变量，交给FIFO创建、写入、读出等操作函数即可
struct Fifo_t
{
    S32 FifoSize;
    volatile S32 Head;      //不一定是线程安全的，volatile不具有原子性
    volatile S32 Tail;
    S8 *FifoBuf;
//    BOOL bSelfCreateBuf;        //use for free memeory
};

//注意：得到验证和使用的接口是三个CreateFifo、WriteFifoEx和ReadFifoEx，其他接口未得到验证


//函数：CreateFifo()创建一个FIFO对象，在使用FIFO的其他任何函数前，必须先创建FIFO，此函数不进行空间分配，实际是完成Fifo_t变量的初始化。
//入参：Fifo指向Fifo结构变量指针（使用时一般需要先定义一个全局的Fifo变量），作为唯一识别FIFO对象的标识，其它FIFO对象的操作函数都使用它作为对象标识
//      Size FIFO的缓冲区的大小，即Buf的大小
//      Buf 指向FIFO缓冲区指针，（一般需要用户定义一个全局变量的缓冲数组，用于存放FIFO数据）
//返回：创建成功则返回Size，否则返回0
S32 CreateFifo(struct Fifo_t *Fifo, S32 Size, VOID *Buf);

//函数：RstFifo()复位FIFO对象（将fifo的head和tail清零）
//入参：无
//返回：无
void RstFifo(struct Fifo_t *Fifo);

//函数：WriteFifo()将数据写入FIFO
//入参：Fifo指向Fifo结构变量指针（FIFO对象），作为唯一识别FIFO对象的标识
//      Buf 指向数据源的缓冲区指针
//      Len 写入数据的长度
//返回：实际写入FIFO的字节数
S32 WriteFifo(struct Fifo_t *Fifo, VOID *Buf, S32 Len);

//函数：WriteFifoEx将数据写入FIFO，该函数与WriteFifo()的区别在于：
//      本函数先判断FIFO的容量是否足够存入本次写操作所要求的Len个字节，如果足够则写入，如果不够则一个字节也不会写入。
//入参：Fifo指向Fifo结构变量指针（FIFO对象），作为唯一识别FIFO对象的标识
//      Buf 指向数据源的缓冲区指针
//      Len 写入数据的长度
//返回：实际写入FIFO的字节数
S32 WriteFifoEx(struct Fifo_t *Fifo, VOID *Buf, S32 Len);    //WriteFifoEx(): 要么全部写入，要么都不写入

//函数：PutFifo()向FIFO写入一个字节
//入参：Fifo指向Fifo结构变量指针（FIFO对象），作为唯一识别FIFO对象的标识
//      Byte要写入的字节
//返回：写入成功返回TRUE，否则返回FALSE
BOOL PutFifo(struct Fifo_t *Fifo, S8 Byte);

//函数：ReadFifo()将数据从FIFO中读出，并删除FIFO内已经读出的字节
//入参：Fifo指向Fifo结构变量指针（FIFO对象），作为唯一识别FIFO对象的标识
//      Buf 指向存放目的数据缓冲区的指针
//      Len pBuf所指向的缓冲区的长度
//返回：实际读出的字节数
S32 ReadFifo(struct Fifo_t *Fifo, VOID *Buf, S32 Len);

//函数：ReadFifoEx()将数据从FIFO中读出，并删除FIFO内已经读出的字节
//      本函数先判断FIFO是否有Len个字节，如果有则读出Len个字节，如果不够Len个字节则不会读出FIFO内数据。
//入参：Fifo指向Fifo结构变量指针（FIFO对象），作为唯一识别FIFO对象的标识
//      Buf 指向存放目的数据缓冲区的指针
//      Len Buf所指向的缓冲区的长度
//返回：实际读出的字节数
S32 ReadFifoEx(struct Fifo_t *Fifo,volatile VOID *Buf, S32 Len);

//函数：GetFifo()从FIFO中读出一个字节
//入参：Fifo指向Fifo结构变量指针（FIFO对象），作为唯一识别FIFO对象的标识
//      Ret指向存放读出数据的指针
//返回：写入成功返回TRUE，否则返回FALSE
BOOL GetFifo(struct Fifo_t *Fifo, S8 *Ret);

//函数：DestroyFifo()摧毁FIFO对象（）
//入参：Fifo指向Fifo结构变量指针（FIFO对象），作为唯一识别FIFO对象的标识
//返回：无
void DestroyFifo(struct Fifo_t *Fifo);

S32 GetFifoDataSize(struct Fifo_t *Fifo, S32 WishFifoSize, S32 SingleSize);

//函数：CopyReadFifo()将数据从FIFO中读出，并且不删除原来数据，即读后FIFO内容不变
//入参：Fifo指向Fifo结构变量指针（FIFO对象），作为唯一识别FIFO对象的标识
//      CopyBuf 指向存放目的数据缓冲区的指针
//      Len CopyBuf所指向的缓冲区的长度
//返回：实际读出的字节数
S32 CopyReadFifo(struct Fifo_t *Fifo, VOID *CopyBuf, S32 Len);

//函数：EraseFifo()删除FIFO队列头部开始的Len个字节。
//入参：Fifo指向Fifo结构变量指针（FIFO对象），作为唯一识别FIFO对象的标识
//      Len 要删除的数据长度
//返回：实际删除的字节数
S32 EraseFifo(struct Fifo_t *Fifo, S32 Len);

S32 SafeReadFifo(struct Fifo_t * Fifo, VOID * FifoBuf, S32 FifoSize, S32 SingleSize, VOID *OutBuf);

S32 SafeWriteFifo(struct Fifo_t * Fifo, VOID * FifoBuf, S32 FifoSize, S32 SingleSize, VOID *InBuf);
#endif

#ifdef __cplusplus
}
#endif

#endif




