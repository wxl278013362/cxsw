#ifndef _MAP_H
#define _MAP_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "Common/Include/CrMutex.h"

typedef VOID* CrMap_t;

#ifdef  __cplusplus
extern "C"{
#endif

struct QueueBuffAttr_t
{
    //这下面的四个参数由总的创建来完成
    S8           *Data; //数据缓冲区，存放最新得的数据（一个消息的长度的缓冲区）
    S32          DataLen;
    S8           *QueueName;    //
    CrPthreadMutex_t    *Mutex;    //缓冲区的锁的指针
};

S32 CrQueueBuffInit(S8 *QueueName, S32 MsgSize, struct QueueBuffAttr_t *Attr);
S32 CrQueueBuffUnInit(S8 *QueueName);
S32 CrQueueBuffDestoryAll();
//FIXME：删除所有申请的空间

/******map的内容************/
struct MapNode_t
{
    U32 Key;
    VOID *Data;
};

//Head 不能为空
CrMap_t MapCreate(); 	//malloc Map的头指针， 不是Node变量

//删除整个Map 必须先删除所有申请的空间才可调用destory
VOID MapDestory(CrMap_t Map);

//返回：成功--插入Node的地址，失败--NULL（已占用）等，具体查下stl的定义
VOID *MapInsert(CrMap_t Map, struct MapNode_t *Node); //插入 必須使用malloc 不能使用临时变量

//返回：找到--Node中Data的地址，找不到--NULL
CrMap_t MapFind(CrMap_t Map, S32 Key);

//返回：成功--返回结点指针，失败（找不到）--NULL 需要手动释放返回指针
CrMap_t MapErase(CrMap_t Map, S32 Key);  //擦除指定Key对应的Node
S32 MapSize(CrMap_t Map);			//返回整张Map节点个数
VOID *MapGetBegin(CrMap_t Map);

#ifdef  __cplusplus
}
#endif

#endif

