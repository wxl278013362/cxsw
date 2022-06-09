

#include <stdio.h>
#include "MenuStack.h"
#include <stdlib.h>


struct MenuStack* MenuStackCreate(S32 StackCapacity)
{
    if ( StackCapacity <= 0 )
        return NULL;

    struct MenuStack *StackPtr = NULL;
    StackPtr = (struct MenuStack*)malloc(sizeof(struct MenuStack) + sizeof(struct MenuStackNode) * StackCapacity );
    if ( !StackPtr )
        return StackPtr;
    
    //PStack->Nodes = (struct MenuStackNode*)(PStack + sizeof(struct MenuStack));  //分配空间(柔性数组的的地址不用赋值)
    StackPtr->Capacity = StackCapacity;
    StackPtr->Size = 0;
    S32 i = 0;
//    struct MenuStackNode* NodePtr = NULL;
    for ( i = 0; i < StackCapacity ; i++ )
    {
//        PNode = (struct MenuStackNode*)((S8*)PStack->Nodes + Offset);
//        Offset += sizeof(MenuStackNode);
//        PNode->Data = NULL;
        StackPtr->Nodes[i].Data = NULL;
    }

    return StackPtr;
}


S32 MenuStackDestroy(struct MenuStack *StackPtr)
{
    if ( StackPtr )
    {
        free(StackPtr);
    }

    return 1;
}


S32 MenuStackPush(struct MenuStack *StackPtr, struct MenuItem_t *PMenuArrayPtr)
{
    if ( !StackPtr || !PMenuArrayPtr )
        return 0;

    if ( MenuStackFull( StackPtr) )
        return 0;

    StackPtr->Nodes[StackPtr->Size].Data = PMenuArrayPtr;
    StackPtr->Size++;

    return 1;
}


S32 MenuStackMultiPush(struct MenuStack *StackPtr, struct MenuItem_t *MenuArraysPtr[100], S32 Count)
{
    if ( !StackPtr || !MenuArraysPtr || (Count <= 0) )
        return 0;

    int i = 0, PushCount = 0;
    for ( i = 0; i < Count ; ++i )
    {
        if ( MenuStackFull(StackPtr) )
            break;

        if ( MenuStackPush(StackPtr, MenuArraysPtr[i]) )
            PushCount++;
    }

    return PushCount;
}

struct MenuItem_t* MenuStackPop(struct MenuStack *StackPtr)
{
    if ( !StackPtr )
        return NULL;

    if ( MenuStackEmpty(StackPtr) )
        return NULL;

    struct MenuItem_t *Data = StackPtr->Nodes[(StackPtr->Size) - 1].Data;
    StackPtr->Nodes[(StackPtr->Size) - 1].Data = NULL;
    StackPtr->Size--;

    return Data;
}

S32 MenuStackMultiPop(struct MenuStack *StackPtr, struct MenuItem_t *MenuArraysPtr[100], S32 Count)
{
    if ( !StackPtr || !MenuArraysPtr || (Count <= 0) )
        return 0;

    int i = 0, Index = 0;
    for ( i = 0; i < Count ; i++ )
    {
        if ( MenuStackEmpty(StackPtr) )
            break;
            
        struct MenuItem_t *MenuArrayPtr = MenuStackPop(StackPtr);
        if(MenuArrayPtr)
        {
            MenuArraysPtr[Index++] = MenuArrayPtr;
        }
    }

    return Index;
}


S32 MenuStackEmpty(struct MenuStack *StackPtr)
{
    if ( !StackPtr )
        return 1;

    return StackPtr->Size == 0 ? 1 : 0;
}

S32 MenuStackFull(struct MenuStack *StackPtr)
{
    if ( !StackPtr )
        return 1;

    return StackPtr->Size == StackPtr->Capacity ? 1 : 0;
}

S32 MenuStackNodeIndex(struct MenuStack *StackPtr, struct MenuItem_t *MenuArrayPtr)
{
    if ( !StackPtr || !MenuArrayPtr )
        return -1;

    S32 i = 0, Index = -1;
    struct MenuItem_t *MenuPtr = NULL;
    for ( i = 0; i < StackPtr->Size ; i++ )
    {
        MenuPtr = StackPtr->Nodes[i].Data;
        if ( MenuPtr == MenuArrayPtr )
        {
            Index = i;
            break;
        }
    }

    return Index;
}

S32 MenuStackSize(struct MenuStack *StackPtr)
{
    if ( !StackPtr )
        return 0;
    
    return StackPtr->Size;
}











