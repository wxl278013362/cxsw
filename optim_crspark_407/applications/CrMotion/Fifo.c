////////////////////////////////////////////////////////////////
// FileName:    fifo.c
// Description: 
// Copyright:    Creality(创想三维)
// By:            
// LastUpdate:    2021.05
// Version:        V1.0
// History:
// Memo:

////////////////////////////////////////////////////////////////
#include <string.h>
#include "Fifo.h"
#include "rthw.h"
//#include "..\main\Global.h"
#define TRUE        (1)
#define FALSE       (0)

#ifdef NULL
#undef NULL
#define NULL        (0)
#endif

#if 0
S32 CreateFifo(struct Fifo_t *Fifo, S32 Size, VOID *Buf)
{
    if(Fifo == NULL) 
        return(0);
    if(Buf == NULL)
    {
        if(Fifo->FifoBuf == NULL) return(0);
    }
    else
    {
        Fifo->FifoBuf = (S8 *)Buf;
    }

    Fifo->FifoSize = Size;
    Fifo->Head = 0;
    Fifo->Tail = 0;
    return(Fifo->FifoSize);
}

void RstFifo(struct Fifo_t *Fifo)
{
    Fifo->Head = 0;
    Fifo->Tail = 0;
}

S32 WriteFifo(struct Fifo_t *Fifo, VOID *Buf, S32 Len) //safety in interrupt proccess
{
    S32 i,next;

    if(Fifo->FifoBuf == NULL || Fifo->FifoSize == 0)
        return(0);
    for(i = 0; i < Len; i++)
    {
        next=(Fifo->Tail + 1) % (Fifo->FifoSize);
        if(next == Fifo->Head) 
            break;
        Fifo->FifoBuf[Fifo->Tail] = *(S8 *)Buf;
        Fifo->Tail = next;
        Buf++;
    }
    return(i);
}

//WriteFifoEx(): 要么全部写入，要么都不写入
//函数：WriteFifoEx将数据写入FIFO，该函数与WriteFifo()的区别在于：
//      本函数先判断FIFO的容量是否足够存入本次写操作所要求的Len个字节，如果足够则写入，如果不够则一个字节也不会写入。
//入参：Fifo指向SFifo结构变量指针（FIFO对象），作为唯一识别FIFO对象的标识
//      Buf 指向数据源的缓冲区指针
//      Len 写入数据的长度
//返回：实际写入FIFO的字节数
S32 WriteFifoEx(struct Fifo_t *Fifo, VOID *Buf, S32 Len) //safety in interrupt proccess
{
    S32 i,next;
    S32 Size;

    if(Fifo->FifoBuf == NULL || Fifo->FifoSize == 0) 
        return(0);

    Size = Fifo->Tail;
    Size += Fifo->FifoSize;
    Size -= Fifo->Head;
    Size %= Fifo->FifoSize;

    Size = Fifo->FifoSize - Size - 1;//需要减去尾指针的空位置
    //rt_kprintf("fifo Tail:%d Head :%d \n",Fifo->Tail,Fifo->Head);
    //rt_kprintf("fifo Size:%d remi size:%d len:%d\n",Fifo->FifoSize,Size,Len);
    if(Size >= Len) //有足够的空间写入len个字节数据
    {
        for(i = 0; i < Len; i++)
        {
            next = (Fifo->Tail + 1) % (Fifo->FifoSize);
            if(next == Fifo->Head) break;

            Fifo->FifoBuf[Fifo->Tail] = *(S8 *)Buf;
            Fifo->Tail = next;
            (S8 *)Buf++;
        }
        return(Len);
    }
    /**/
    else
    {
        //rt_kprintf("fifo:%p full,Tail:%d Head :%d \n",Fifo,Fifo->Tail,Fifo->Head);
        //rt_kprintf("data length:%d\n",GetFifoDataSize(Fifo, Fifo->FifoSize,4));
    }
    return(0);
}

BOOL PutFifo(struct Fifo_t *Fifo, S8 Byte) //safety in interrupt proccess
{
    S32 next;

//    if(Fifo->FifoBuf==NULL || Fifo->FifoSize==0) return(FALSE);

    next = Fifo->Tail + 1;
    if(next >= Fifo->FifoSize)
        next = 0;
    if(next == Fifo->Head)
        return(FALSE);

    Fifo->FifoBuf[Fifo->Tail] = Byte;
    Fifo->Tail = next;

    return(TRUE);
}

S32 ReadFifo(struct Fifo_t *Fifo, VOID *Buf, S32 Len)
{
    S32 i;
    if(Fifo->FifoBuf == NULL || Fifo->FifoSize == 0) 
        return(0);

    for(i = 0; i < Len; i++)
    {
        if(Fifo->Head == Fifo->Tail) 
            break;

        *(S8 *)Buf = Fifo->FifoBuf[Fifo->Head];
        Fifo->Head++;
        Fifo->Head %= Fifo->FifoSize;
        Buf++;
    }
    return(i);
}

//函数：ReadFifoEx()将数据从FIFO中读出，并删除FIFO内已经读出的字节
//      本函数先判断FIFO是否有Len个字节，如果有则读出Len个字节，如果不够Len个字节则不会读出FIFO内数据。
//入参：Fifo指向Fifo结构变量指针（FIFO对象），作为唯一识别FIFO对象的标识
//      Buf 指向存放目的数据缓冲区的指针
//      Len Buf所指向的缓冲区的长度
//返回：实际读出的字节数
volatile U8 Reading = 0;     //调试用的，可以删除掉

S32 ReadFifoEx(struct Fifo_t *Fifo,volatile VOID *Buf, S32 Len)
{
    S32 i;
    S32 Size;

    if(Fifo->FifoBuf == NULL || Fifo->FifoSize == 0) 
        return(0);
    
    Size = Fifo->Tail;
    Size += Fifo->FifoSize;
    Size -= Fifo->Head;
    Size %= Fifo->FifoSize;
    //Size -= 1;
    if(Size >= Len)
    {
        Reading = 2;
        for(i = 0; i < Len; i++)
        {
            if(Fifo->Head == Fifo->Tail)
            {
                Reading = 3;
                break;
            }
            *(S8 *)Buf = Fifo->FifoBuf[Fifo->Head];
            Fifo->Head++;
            Fifo->Head %= Fifo->FifoSize;
            Buf++;
        }
        
        return(Len);
    }
    else
    {
        Reading = 4;
    }
    return(0);
}

BOOL GetFifo(struct Fifo_t *Fifo, S8 *Ret)
{
//    if(Fifo->FifoBuf==NULL || Fifo->FifoSize==0) return(FALSE);

    if(Fifo->Head == Fifo->Tail) 
        return(FALSE);

    *Ret = Fifo->FifoBuf[Fifo->Head];
    Fifo->Head ++;
    if(Fifo->Head >= Fifo->FifoSize) Fifo->Head = 0;

    return(TRUE);
}

void DestroyFifo(struct Fifo_t *Fifo)
{
//    if(pf->bSelfCreateBuf)  free(pf->pFifoBuf);
//    memset(pf,0,sizeof(SFifo));
}

/******************************************************************************
* 函数名称: GetFifoDataSize
* 函数功能: 获取FIFO中的数据长度
* 输入参数: struct Fifo_t *Fifo          FIFO指针
*           S32 WishFifoSize         期望的FIFO长度
* 输出参数: 无
* 返 回 值: -1 FIFO异常, 其他 FIFO的数据长度
******************************************************************************/
S32 GetFifoDataSize(struct Fifo_t *Fifo, S32 WishFifoSize, S32 SingleSize)
{
    S32 i32Size = 0;

    if (NULL == Fifo)
    {
        return -1;
    }

    // FIFO缓冲的指针为空
    if (Fifo->FifoBuf == NULL)
    {
        return -1;
    }

    // FIFO的大小与期望的大小不符
    if (WishFifoSize != Fifo->FifoSize)
    {
        return -1;
    }

    // 头或尾越限
    if ((Fifo->Tail >= WishFifoSize) || (Fifo->Head >= WishFifoSize))
    {
        return -1;
    }

    if (0 == SingleSize)
    {
        return -1;
    }

    // 头尾部对其，认为FIFO已经异常
    if ((0 != (Fifo->Head % SingleSize)) || (0 != (Fifo->Tail % SingleSize)))
    {
        return -1;
    }

    if (Fifo->FifoSize == 0)
    {
        return 0;
    }
    
    i32Size = Fifo->Tail;
    i32Size += Fifo->FifoSize;
    i32Size -= Fifo->Head;
    i32Size %= Fifo->FifoSize;
    return i32Size / SingleSize;
}

S32 CopyReadFifo(struct Fifo_t *Fifo, VOID *CopyBuf, S32 Len)
{
    S32 i,Head,Tail;

    if(Fifo->FifoBuf == NULL || Fifo->FifoSize == 0) 
        return(0);

    Head = Fifo->Head;
    Tail = Fifo->Tail;

    for(i = 0; i < Len; i ++)
    {
        if(Head == Tail) 
            break;

        *(S8 *)CopyBuf = Fifo->FifoBuf[Head];
        Head++;
        Head %= Fifo->FifoSize;
        CopyBuf ++;
    }
    return(i);

}
S32 EraseFifo(struct Fifo_t *Fifo, S32 Len)
{
    S32 i;

    if(Fifo->FifoBuf == NULL || Fifo->FifoSize == 0) 
        return(0);

    for(i = 0; i < Len; i ++)
    {
        if(Fifo->Head == Fifo->Tail) 
            break;
        Fifo->Head++;
        Fifo->Head %= Fifo->FifoSize;
    }
    return(i);
}

/******************************************************************************
* 函数名称: SafeReadFifo
* 函数功能: 安全的方式读FIFO中的一个数据
* 输入参数:
*           struct Fifo_t *Fifo FIFO指针
*           VOID *Buf FIFO缓冲的指针
*           S32  FifoSize FIFO的大小
*           S32  SingleSize 单个数据的大小
* 输出参数: VOID *OutBuf 读FIFO的输出缓冲
* 返 回 值: < 0 : 读取异常; 0 : FIFO中无数据; > 0 读到的数据长度
******************************************************************************/
S32 SafeReadFifo(struct Fifo_t *Fifo, VOID *Buf, S32 FifoSize, S32 SingleSize, VOID *OutBuf)
{
    S32 FifoDataSize = 0;
    S32 ReadSize = 0;
    
    // 根据FIFO size判断FIFO是否正常
    FifoDataSize = GetFifoDataSize(Fifo, FifoSize, SingleSize);

    if (FifoDataSize < 0)
    {
//        printf("[SafeReadFifo] Fifo is abnormal. try to create it again.\n");

        if (FifoSize != CreateFifo(Fifo, FifoSize, Buf))
        {
//            printf( "[SafeReadFifo] Create FIFO failed.\n");
        }
        else
        {
//            printf( "[SafeReadFifo] Create FIFO success.\n");
        }

        // 本次还是返回失败
        return -1;
    }
    else if (FifoDataSize == 0)
    {
        return 0;
    }

    // 下面开始读取FIFO内容
    ReadSize = ReadFifoEx(Fifo, OutBuf, SingleSize);

    return (S32)ReadSize;
}

/******************************************************************************
* 函数名称: SafeWriteFifo
* 函数功能: 安全的方式写FIFO中的一个数据
* 输入参数:
*           Fifo * Fifo FIFO指针
*           VOID * FifoBuf FIFO缓冲的指针
*           S32  FifoSize FIFO的大小
*           S32  SingleSize 单个数据的大小
*           VOID *InBuf 写FIFO的输入缓冲
* 输出参数: 无
* 返 回 值: < 0 : FIFO异常; 0 : 写入失败; > 0 写入的数据长度
******************************************************************************/
S32 SafeWriteFifo(struct Fifo_t *Fifo, VOID *FifoBuf, S32 FifoSize, S32 SingleSize, VOID *InBuf)
{
    S32 FifoDataSize = 0;
    S32 WrittenSize = 0;
    S8 TempOutBuff[32] = {0};      // FIFO单个数据长度的最大值

    // 根据FIFO size判断FIFO是否正常
    FifoDataSize = GetFifoDataSize(Fifo, FifoSize, SingleSize);

    if (FifoDataSize < 0)
    {
//        printf( "[SafeWriteFifo] Fifo is abnormal. try to create it again.");

        if (FifoSize != CreateFifo(Fifo, FifoSize, FifoBuf))
        {
//            printf("[SafeWriteFifo] Create FIFO failed.");
            
            return -1;
        }
        else
        {
            // 尝试创建FIFO成功，则继续
//            printf( "[SafeWriteFifo] Create FIFO success.");
        }
    }

    // 下面开始写入FIFO内容
    WrittenSize = WriteFifoEx(Fifo, InBuf, SingleSize);

    // 认为是满了，读掉一个最老的数据
    if (0 == WrittenSize)
    {
        // 读掉1个
        if (SingleSize != ReadFifoEx(Fifo, TempOutBuff, SingleSize))
        {
//            printf( "[SafeWriteFifo] ApiReadFifoEx one data failed.");

            return 0;
        }
        else
        {
//            printf("[SafeWriteFifo] ApiReadFifoEx one data success, try to write again.");

            return WriteFifoEx(Fifo, InBuf, SingleSize);
        }
    }

    return (S32)WrittenSize;
}

#endif

#define ENTRY_CRITICAL()   U32 Level=rt_hw_interrupt_disable()

#define EXIT_CRITICAL()     rt_hw_interrupt_enable(Level)

VOID TestPrintFifoInfo(CommonFiFo_t *CommonFifo)
{
    rt_kprintf("CommonFifo->ReadIndex = %d\r\n",CommonFifo->ReadIndex);

    rt_kprintf("CommonFifo->WriteIndex = %d\r\n",CommonFifo->WriteIndex);

    rt_kprintf("CommonFifo->FifoLeftSlot = %d\r\n",CommonFifo->FifoLeftSlot);

    return;
}

S32 CommonCreateFifo(CommonFiFo_t *CommonFifo,VOID *FifoBuf,U32 SingleSize, U32 FifoLen)
{
    if((CommonFifo == NULL) && (FifoBuf == NULL))
    {
        return -1;
    }

    CommonFifo->FifoBuf = (U8*)FifoBuf;                                      //初始化起始地址，不要修改此值。如果扩展成动态分配存储空间，该指针可用来释放空间

//    rt_kprintf("CommonFifo->FifoBuf = %p\r\n",CommonFifo->FifoBuf);

    CommonFifo->BufHead = (U8*)FifoBuf;                                      //初始化头部指针，允许指针或下标寻址

//    rt_kprintf("CommonFifo->BufHead = %p\r\n",CommonFifo->BufHead);

    CommonFifo->BufTail = (U8*)((U8*)FifoBuf + SingleSize * FifoLen); //初始化尾部指针，防止越界

//    rt_kprintf("CommonFifo->BufTail = %p\r\n",CommonFifo->BufTail);

    CommonFifo->ReadIndex = 0;                                          //读索引

//    rt_kprintf("CommonFifo->ReadIndex = %d\r\n",CommonFifo->ReadIndex);

    CommonFifo->WriteIndex = 0;                                         //写索引

//    rt_kprintf("CommonFifo->WriteIndex = %d\r\n",CommonFifo->WriteIndex);

    CommonFifo->Fifolen = FifoLen;                                      //缓冲区长度，意思是可存储多少个单体

//    rt_kprintf("CommonFifo->Fifolen = %d\r\n",CommonFifo->Fifolen);

    CommonFifo->FifoSingleSize = SingleSize;                            //缓冲区一个单体大小，单位：字节

//    rt_kprintf("CommonFifo->FifoSingleSize = %d\r\n",CommonFifo->FifoSingleSize);

    CommonFifo->FifoLeftSlot = FifoLen;                                 //缓冲区可允许最大插入单体个数

//    rt_kprintf("CommonFifo->FifoLeftSlot = %d\r\n",CommonFifo->FifoLeftSlot);

    memset(CommonFifo->FifoBuf,0,SingleSize * FifoLen);                 //对缓冲区清零


    return 0;
}

S32 CommonReadFifo(CommonFiFo_t *CommonFifo,VOID *ReadBuf,U32 ReadLen)             //线程安全
{
    U32 Index = 0;

    if(CommonFifo == NULL && ReadBuf == NULL)
    {
        return -1;
    }

    if((CommonFifo->Fifolen - CommonFifo->FifoLeftSlot) < ReadLen)                 //当前FIFO中的数据不足需求
    {
        return -2;
    }

    ENTRY_CRITICAL();

    //Fifo线性剩余空间足够读取的数量
    if(CommonFifo->ReadIndex + ReadLen <= CommonFifo->Fifolen)
    {
        memcpy(ReadBuf,(CommonFifo->BufHead + CommonFifo->ReadIndex * CommonFifo->FifoSingleSize), (ReadLen * (CommonFifo->FifoSingleSize)));

        CommonFifo->ReadIndex += ReadLen;

        if(CommonFifo->ReadIndex == CommonFifo->Fifolen)
        {
            CommonFifo->ReadIndex = 0;
        }
    }
    else                                                                                              //Fifo线性剩余空间不足读取的数量
    {
        U32 LeftLen = CommonFifo->ReadIndex + ReadLen - CommonFifo->Fifolen;   //回滚后读的数量

        U32 ForwardLen = ReadLen - LeftLen;                                   //继续往前读的数量

        memcpy(ReadBuf,(CommonFifo->BufHead + CommonFifo->ReadIndex * CommonFifo->FifoSingleSize),(ForwardLen * (CommonFifo->FifoSingleSize)));

        memcpy((U8*)ReadBuf + ForwardLen * (CommonFifo->FifoSingleSize), CommonFifo->BufHead,(LeftLen * (CommonFifo->FifoSingleSize)));

        CommonFifo->ReadIndex = LeftLen;
    }

    CommonFifo->FifoLeftSlot += ReadLen;                                        //更新剩余空间大小

    EXIT_CRITICAL();

//    TestPrintFifoInfo(CommonFifo);

    return ReadLen; //返回读取的数据长度
}

S32 CommonWriteFifo(CommonFiFo_t *CommonFifo,VOID *WriteBuf,U32 WriteLen)       //线程安全
{
    U32 Index = 0;

    if(CommonFifo == NULL && WriteBuf == NULL)
    {
        return -1;
    }

    if(CommonFifo->FifoLeftSlot < WriteLen)                                     //当前FIFO中的空间不足需求
    {
        return -2;
    }

    ENTRY_CRITICAL();

    if(CommonFifo->WriteIndex + WriteLen <= CommonFifo->Fifolen)  //Fifo线性剩余空间足够写入的数量
    {
        memcpy((CommonFifo->BufHead + CommonFifo->WriteIndex * CommonFifo->FifoSingleSize), WriteBuf,(WriteLen * (CommonFifo->FifoSingleSize)));

        CommonFifo->WriteIndex += WriteLen;

        if(CommonFifo->WriteIndex == CommonFifo->Fifolen)
        {
            CommonFifo->WriteIndex = 0;
        }
    }
    else
    {
        U32 LeftLen = CommonFifo->WriteIndex + WriteLen - CommonFifo->Fifolen; //回滚后要写入的数据

        U32 ForwardLen = WriteLen - LeftLen;                                  //继续往前写入的数量

        memcpy(CommonFifo->BufHead + CommonFifo->WriteIndex * CommonFifo->FifoSingleSize, WriteBuf,(ForwardLen * (CommonFifo->FifoSingleSize)));

        memcpy((CommonFifo->BufHead), (U8*)WriteBuf + ForwardLen * CommonFifo->FifoSingleSize,LeftLen * (CommonFifo->FifoSingleSize));

        CommonFifo->WriteIndex = LeftLen;
    }

    CommonFifo->FifoLeftSlot -= WriteLen;

    EXIT_CRITICAL();

//    TestPrintFifoInfo(CommonFifo);

    return WriteLen;
}

S32 CommonIsFullFifo(CommonFiFo_t *CommonFifo)
{
    if(CommonFifo == NULL)
    {
        return -1;
    }

    if(CommonFifo->FifoLeftSlot == 0)
    {
        return 1;
    }

    return 0;
}

S32 CommonIsEmptyFifo(CommonFiFo_t *CommonFifo)
{
    if(CommonFifo == NULL)
    {
        return -1;
    }

    if(CommonFifo->FifoLeftSlot == CommonFifo->Fifolen)
    {
        return 1;
    }

    return 0;
}

S32 CommonRstFifo(CommonFiFo_t *CommonFifo)
{
    if(CommonFifo == NULL)
    {
        return -1;
    }

    CommonFifo->ReadIndex = 0;

    CommonFifo->WriteIndex = 0;

    CommonFifo->FifoLeftSlot = CommonFifo->Fifolen;

    memset(CommonFifo->FifoBuf, 0, CommonFifo->FifoSingleSize * CommonFifo->Fifolen);

    return 0;
}

S32 CommonDestoryFifo(CommonFiFo_t *CommonFifo)
{
    if(CommonFifo == NULL)
    {
        return -1;
    }

    memset(CommonFifo->FifoBuf, 0, CommonFifo->FifoSingleSize * CommonFifo->Fifolen);

    CommonFifo->FifoBuf = NULL;

    CommonFifo->BufHead = NULL;

    CommonFifo->BufTail = NULL;

    CommonFifo->ReadIndex = 0;

    CommonFifo->WriteIndex = 0;

    CommonFifo->Fifolen = 0;

    CommonFifo->FifoSingleSize = 0;

    CommonFifo->FifoLeftSlot =0;

    return 0;
}
