

#ifndef _MENUSTACK_H

#define _MENUSTACK_H

#include "CrInc/CrType.h"
#include "CrView/CrShowView.h"
#include "CrInc/CrConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

struct MenuStackNode
{
    struct MenuItem_t    *Data;  //记录菜单数组指针
};

struct MenuStack
{
    S32                     Capacity; //容量
    S32                     Size; //此处就是当前的游标cursor位置
//    S32                     CurIndex; //当前的索引
    struct MenuStackNode    Nodes[0];  
//使用柔性数组是不分配空间的也不存在指针变量的空间，但是在创建stack变量时要将柔性数组的空间一起开辟出来,柔性数组的地址就是紧跟在结构体后的空间地址
};

//此栈不开辟空间保存数据，只保存数据的地址，注意不要进栈临时变量的地址
struct MenuStack* MenuStackCreate(S32 StackCapacity);  //创建栈
S32 MenuStackDestroy(struct MenuStack *StackPtr);  //销毁栈 (不对保存的数据指针做释放)
S32 MenuStackPush(struct MenuStack *StackPtr, struct MenuItem_t *MenuArrayPtr); //进栈
S32 MenuStackMultiPush(struct MenuStack *StackPtr, struct MenuItem_t *MenuArraysPtr[100], S32 Count); //出栈多个
struct MenuItem_t* MenuStackPop(struct MenuStack *StackPtr);  //出栈
S32 MenuStackMultiPop(struct MenuStack *StackPtr, struct MenuItem_t *MenuArraysPtr[100], S32 Count); //出栈多个
S32 MenuStackNodeIndex(struct MenuStack *StackPtr, struct MenuItem_t *MenuArrayPtr);  //获取数据在栈中的位置
S32 MenuStackSize(struct MenuStack *StackPtr); 
S32 MenuStackEmpty(struct MenuStack *StackPtr);
S32 MenuStackFull(struct MenuStack *StackPtr);


 #ifdef __cplusplus
 }
 #endif


#endif


