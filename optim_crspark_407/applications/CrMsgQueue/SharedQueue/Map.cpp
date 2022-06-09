#include "Map.h"
#include <string.h>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include "HashFun.h"

using namespace std;

struct Map_t
{
    struct MapNode_t *Head;
    S32 Size;
    //读写锁
};

struct QueueBuffInfo_t
{
    S32       Count;//被使用的次数
    S8        *Buff; //数据缓冲区，存放最新得的数据（一个消息的长度的缓冲区）
    S32       BuffLen;
    S8        *QueueName;    //
    CrPthreadMutex_t Mutex;    //缓冲区的锁的指针
};

CrPthreadMutex_t Mutex;
S32 InitFlag = 0;
static CrMap_t MapPtr = NULL;
#define PRIME 31

CrMap_t MapCreate()  //malloc Map的头指针， 不是Node变量
{
    CrMap_t Map = (VOID *)new map<S32, VOID *>;

    return Map;
}

VOID *MapGetBegin(CrMap_t Map)
{
    map<S32, VOID *> *TempMap = (map<S32, VOID *> *)Map;

    return (VOID *)TempMap->begin()->second;
}

//删除整个Map
VOID MapDestory(CrMap_t Map)
{
    if ( !Map )
        return;

    map<S32, VOID  *> *TempMap = (map<S32, VOID *> *)Map;
    map<S32,  VOID *>::iterator Iter;
    //释放所有的节点second
    for ( Iter = TempMap->begin(); Iter != TempMap->end(); Iter++ )
    {
        if ( Iter->second )
        {
            free(Iter->second);
            Iter->second = NULL;
        }
    }

    TempMap->clear();
    if ( TempMap )
    {
        printf("Reles Map\n");
        free(TempMap);
        TempMap = NULL;
    }

    return;
}
//返回：成功--插入Node的地址，失败--NULL（已占用）等，具体查下stl的定义
VOID *MapInsert(CrMap_t Map, struct MapNode_t *Node)
{
    if ( !Map || !Node )
        return NULL;

    VOID *Temp = NULL;

    map<S32,  VOID *> *TempMap = (map<S32, VOID *> *)Map;
    map<S32,  VOID *>::iterator Iter = TempMap->find((S32)Node->Key);
    if ( Iter != TempMap->end() )
    {
        return Temp;
    }
    pair<map<S32, VOID *>::iterator,bool> Ret;
    Ret = TempMap->insert(pair<S32, VOID  *>((S32)Node->Key, Node->Data));
    if ( Ret.second == false )
    {
        return Temp;
    }

    Temp = Ret.first->second;

    return Temp;
}
//返回：找到--Node中Data的地址，找不到--NULL
VOID *MapFind(CrMap_t Map, S32 Key)
{
    VOID *Temp = NULL;

    if ( !Map )
        return Temp;

    map<S32, VOID *> *TempMap = (map<S32, VOID *> *)Map;
    map<S32, VOID  *>::iterator Iter = TempMap->find((U32)Key);

    if ( Iter != TempMap->end() )
    {
        Temp = Iter->second;
    }

    return Temp;
}
//返回：成功--1，失败（找不到）--0
VOID *MapErase(CrMap_t Map, S32 Key)  //擦除指定Key对应的Node
{
    VOID * Ret = 0;

    if ( !Map )
        return Ret;

    map<S32, VOID *> *TempMap = (map<S32, VOID *> *)Map;
    map<S32, VOID *>::iterator Iter = TempMap->find((U32)Key);

    if ( Iter != TempMap->end() )
    {
        if (Iter->second)
        {
            Iter->second = NULL;
        }
        TempMap->erase(Iter);
    }

    return Ret;
}
S32 MapSize(CrMap_t Map)         //返回整张Map节点个数
{
    if ( !Map )
        return 0;

    map<S32, VOID*> *TempMap = (map<S32, VOID *> *)Map;

    return TempMap->size();
}

S32 CrQueueBuffInit(S8 *QueueName, S32 MsgSize, struct QueueBuffAttr_t *Attr)
{
    if ( !MapPtr )
    {
        CrPthreadMutexInit(&Mutex, NULL);
        CrPthreadMutexLock(&Mutex);

        if ( !MapPtr )
              MapPtr = MapCreate();

        if ( !MapPtr )
        {
            CrPthreadMutexUnLock(&Mutex);
            return 0;
        }
        CrPthreadMutexUnLock(&Mutex);
    }

    if ( !QueueName || !Attr || (MsgSize < 1) )
        return 0;

    CrPthreadMutexLock(&Mutex);
    S32 Hash = BernsteinHash(QueueName, PRIME);

    S32 Find = 0;

    //先进行查询
    VOID *Data = MapFind(MapPtr, Hash);
    if ( Data )
    {
        Find = 1;
    }

    struct QueueBuffInfo_t *Buff = NULL;
    if ( !Find )   //没有找到, 创建Buff
    {
        Buff = (struct QueueBuffInfo_t *)malloc(sizeof(struct QueueBuffInfo_t));
        if ( !Buff )
        {
            CrPthreadMutexUnLock(&Mutex);
            return 0;
        }

        memset(Buff, 0, sizeof(struct QueueBuffInfo_t));
        S32 NameLen = strlen(QueueName);
        Buff->QueueName = (S8 *)malloc((NameLen + 1) *sizeof(S8));
        if ( !(Buff->QueueName) )
        {
          //  CrPthreadMutexDestroy(&(Buff->Mutex));
            free(Buff);
            CrPthreadMutexUnLock(&Mutex);
            return 0;
        }

        Buff->Buff = (S8 *)malloc(MsgSize * sizeof(S8));
        if ( !(Buff->Buff) )
        {
            free(Buff->QueueName);
           //  CrPthreadMutexDestroy(&(Buff->Mutex));
            free(Buff);
            Buff = 0;
            CrPthreadMutexUnLock(&Mutex);
            return 0;
        }
        memset(Buff->Buff, 0, MsgSize);

        CrPthreadMutexInit(&(Buff->Mutex), NULL);
        Buff->BuffLen = MsgSize;
        strcpy(Buff->QueueName, QueueName);

        struct MapNode_t Temp = {0};
        Temp.Key = Hash;
        Temp.Data = (VOID *)Buff;

        if ( !MapInsert(MapPtr, &Temp) )
        {
            free(Buff->QueueName);
            Buff->QueueName = NULL;
            free(Buff->Buff);
            Buff->Buff = NULL;
            CrPthreadMutexDestroy(&(Buff->Mutex));
            free(Buff);
            Buff = 0;
            CrPthreadMutexUnLock(&Mutex);
        }
    }
    else
    {
        Buff = (struct QueueBuffInfo_t *)Data;
    }

    if ( Buff )
    {
        CrPthreadMutexLock(&(Buff->Mutex));
        Buff->Count++;
        Attr->Data = Buff->Buff;
        Attr->DataLen = Buff->BuffLen;
        Attr->QueueName = Buff->QueueName;
        Attr->Mutex = &(Buff->Mutex);
        CrPthreadMutexUnLock(&(Buff->Mutex));
    }

    CrPthreadMutexUnLock(&Mutex);

    return 1;
}

S32 CrQueueBuffUnInit(S8 *QueueName)
{
    if ( !QueueName )
        return 0;

    S32 HashValue = BernsteinHash(QueueName, PRIME);
    CrPthreadMutexLock(&Mutex);
   // struct QueueBuffInfo_t *NewHead = (struct QueueBuffInfo_t *)((S8 *)HashList.HashTable + sizeof(struct QueueBuffInfo_t*) * (HashValue % TableSize) );
    struct QueueBuffInfo_t *NewHead = (struct QueueBuffInfo_t *)MapFind(MapPtr, HashValue);

    if ( !NewHead )
    {
        CrPthreadMutexUnLock(&Mutex);
        return 0;
    }

    BOOL DelBuff = 0;
    if ( strcmp(NewHead->QueueName, QueueName) == 0 )
    {
        CrPthreadMutexLock(&(NewHead->Mutex));
        NewHead->Count--;
        if ( NewHead->Count <= 0 )
        {
            DelBuff = 1;
        }
        CrPthreadMutexUnLock(&(NewHead->Mutex));
    }

    //删除
    if ( DelBuff )
    {
        printf("DelBuff = %d", DelBuff);
        CrPthreadMutexLock(&(NewHead->Mutex));
        free(NewHead->QueueName);
        NewHead->QueueName = NULL;
        free(NewHead->Buff);
        NewHead->Buff = NULL;
        CrPthreadMutexUnLock(&(NewHead->Mutex));
        CrPthreadMutexDestroy(&(NewHead->Mutex));
        free(NewHead);
        NewHead = NULL;
        MapErase(MapPtr, HashValue);
    }

    CrPthreadMutexUnLock(&Mutex);

    return 1;
}

S32 CrQueueBuffDestoryAll( )
{
    if ( !MapPtr )
        return 0;

    S32 Size = MapSize(MapPtr);
    S32 Ret = 0;
    CrPthreadMutexLock(&Mutex);

    for ( S32 i = 0; i < Size ; i++ )
    {
        printf("Map Size = %d\n", MapSize(MapPtr));
        struct QueueBuffInfo_t *Head = (struct QueueBuffInfo_t *)MapGetBegin(MapPtr);
        if ( !Head )
        {
            printf("Head NULL\n");
            break;
        }
        CrPthreadMutexLock(&((Head->Mutex)));
        if ( Head->Buff )
        {
            printf("relese BUFF = %p\n", Head->Buff);
            free(Head->Buff);
            Head->Buff = NULL;

        }
        if ( Head->QueueName )
        {
            Ret = BernsteinHash(Head->QueueName, PRIME);
            printf("relese QueueName = %p, %s\n", Head->QueueName, Head->QueueName);
            free(Head->QueueName);
            Head->QueueName = NULL;
        }

        CrPthreadMutexUnLock(&(Head->Mutex));
        CrPthreadMutexDestroy(&(Head->Mutex));
        if ( Head )
        {
            printf("relese QueueBuffInfo_t = %p\n", Head);
            free(Head);
            Head = NULL;
        }

        MapErase(MapPtr, Ret);
    }

    MapDestory(MapPtr);
    CrPthreadMutexUnLock(&Mutex);
    MapPtr = NULL;

    return 1;
}

VOID MapTest(VOID)
{
    CrMap_t Head = MapCreate();
    if ( !Head )
    {
        printf("Map Create Fail!\n");
        return ;
    }
    U32 Key = 1;
   // struct MapNode_t Value = {Key, (VOID *)"Creality"};

    S8 ValueAr[] = "Creaty" ;

    struct MapNode_t *Value = (struct MapNode_t *)malloc(sizeof(struct MapNode_t ));
    Value->Key = Key;
    Value->Data = (VOID *)malloc(sizeof(ValueAr));

    memcpy(Value->Data, ValueAr, sizeof(ValueAr));

    VOID * Temp = MapInsert(Head, Value);
    if ( !Temp )
    {
        printf("Insert Fail!\n");
        return ;
    }

    S8 ValueArr[] = "Creaty2" ;
    Key = 2;
    struct MapNode_t *Value2 = (struct MapNode_t *)malloc(sizeof(struct MapNode_t ));
    Value2->Key = Key;
    Value2->Data = (VOID *)malloc(sizeof(ValueArr));
    memcpy(Value2->Data, ValueArr, sizeof(ValueArr));

    Temp = MapInsert(Head, Value2);
    if ( !Temp )
    {
        printf("Insert Fail!\n");
        return ;
    }

    S8 * String = (S8 *)MapFind(Head, Key);
    if ( String )
    {
        printf("Find Key : %d Value is ---%s--- \n", Key, String);
    }
    S32 Size = MapSize(Head);
    printf("Map Size is %d\n", Size);

    VOID * Ret = MapErase(Head, Key);  //擦除指定Key对应的Node
    if ( Ret )
    {
        printf("Erase Key :%d Ok!\n", Key);
    }

    MapDestory(Head);

}

MSH_CMD_EXPORT(MapTest, 111);
