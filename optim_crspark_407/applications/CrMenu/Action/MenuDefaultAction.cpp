#include <stdio.h>
#include <string.h>
#include "MenuStack.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

static struct MenuStack *MenuStackPtr = NULL;

#define MENUARRAY_STACKCAPACITY 40     //菜单数组压栈的栈空间大小


/**************************************接口定义(不变部分)********************************************/

VOID *InitMenuDefaultAction()
{

    struct MenuStack *StackPtr = MenuStackCreate(MENUARRAY_STACKCAPACITY);
    if ( !StackPtr )
        return NULL;

    MenuStackPtr = StackPtr;

    return (VOID *)RootMenu;
}

S32 UnInitMenuDefaultAction()
{
    if ( MenuStackPtr )
    {
        MenuStackDestroy(MenuStackPtr);
        MenuStackPtr = NULL;
    }

    return 1;
}

struct MenuItem_t * GetRootMenu()
{
    return RootMenu;
}

VOID* MoveToRoot()
{
    if ( !RootMenu || !MenuStackPtr) //判断栈空间是否分配
        return NULL;

    S32 Size = MenuStackSize(MenuStackPtr);
    struct MenuItem_t *MenuArray[Size];
    MenuStackMultiPop(MenuStackPtr, MenuArray, Size);   //清空了

    return RootMenu;
}


S32 GetItemsCount(VOID *      MenuArrayPtr)
{
    S32 Size = 0;
    if ( !MenuArrayPtr )
    {
        return Size;
    }
    struct MenuItem_t *Item = (struct MenuItem_t *)MenuArrayPtr;
    S32 Offset = 0;
    while ( Item )
    {
        Item = (struct MenuItem_t *)((S8 *)MenuArrayPtr + Offset);
        if ( Item->ItemNameId == MENU_ITEM_FINISH )
        {
            break;
        }

        Offset += sizeof(struct MenuItem_t);
        Size++;
    }

    return Size;
}

VOID* MoveToChild(VOID *DestMenuArrayPtr, VOID *SrcMenuArrayPtr)
{
      //  printf("----------MoveToNextMenu SrcMenuArray = %p, DestMenuArray = %p\n", SrcMenuArrayPtr, DestMenuArrayPtr);
        if ( !SrcMenuArrayPtr || !DestMenuArrayPtr || !MenuStackPtr)
            return NULL;

        S32 Size = GetItemsCount(DestMenuArrayPtr);
        if ( Size <= 0 )
            return NULL;

        struct MenuItem_t *ItemPtr = NULL;
        struct MenuItem_t *CurItemPtr = NULL;
        S32 i = 0, Offset = 0;
        for ( i = 0; i < Size ; i++ )
        {
            ItemPtr = (struct MenuItem_t *)((S8 *)DestMenuArrayPtr + Offset);

            if ( ItemPtr->Next == (struct MenuItem_t *)SrcMenuArrayPtr )
            {
                CurItemPtr = ItemPtr;
                break;
            }

            Offset += sizeof(struct MenuItem_t);
        }

        //将菜单数组压栈
        S32 SrcIndex = -1, DestIndex = -1, PushFlag = 0;
        SrcIndex = MenuStackNodeIndex(MenuStackPtr, (struct MenuItem_t *)SrcMenuArrayPtr);
        if ( SrcIndex < 0 ) //在栈中不存在
        {
            
            if ( !MenuStackPush(MenuStackPtr, (struct MenuItem_t *)SrcMenuArrayPtr) ) //无法进栈
                return NULL;

            PushFlag = 1;
            SrcIndex = MenuStackNodeIndex(MenuStackPtr, (struct MenuItem_t *)SrcMenuArrayPtr);
        }

        DestIndex = MenuStackNodeIndex(MenuStackPtr, (struct MenuItem_t *)DestMenuArrayPtr);
        if ( DestIndex < 0 )
        {
            if ( !MenuStackPush(MenuStackPtr, (struct MenuItem_t *)DestMenuArrayPtr) ) //无法进栈
            {
                if ( PushFlag )  //进行了进栈操作才会进行出栈
                    MenuStackPop(MenuStackPtr);

                return NULL;
            }

            DestIndex = MenuStackNodeIndex(MenuStackPtr, (struct MenuItem_t *)DestMenuArrayPtr);
        }

        if ( DestIndex < SrcIndex )  //说明目的菜单是源菜单的上级节点
        {
            //要进行出栈
            S32 StackSize = MenuStackSize(MenuStackPtr);
            if ( StackSize > (DestIndex + 1) )  //不包含目的菜单数组
            {
                S32 PopSize = StackSize - DestIndex - 1; //不能将目的菜单数组也出栈了
                struct MenuItem_t *MenuArray[PopSize];
                MenuStackMultiPop(MenuStackPtr, MenuArray, PopSize);
            }
        }

        if ( !CurItemPtr ) //可能不是回到上一级菜单
            CurItemPtr = (struct MenuItem_t *)DestMenuArrayPtr;

        return CurItemPtr;
}

VOID* MoveToParant(VOID **DestMenuArray, VOID *SrcMenuArrayPtr)
{
    if ( !MenuStackPtr || !SrcMenuArrayPtr || MenuStackEmpty(MenuStackPtr) || !DestMenuArray)
        return NULL;

    S32 Index = -1;

    Index = MenuStackNodeIndex(MenuStackPtr, (struct MenuItem_t*)SrcMenuArrayPtr);

    if ( Index < 0 )
        return NULL;

    if ( Index == 0 )
        return SrcMenuArrayPtr;

    //进行出栈
    struct MenuItem_t *MenuPtr = MenuStackPtr->Nodes[ --Index ].Data;  //获取目的菜单数组
    struct MenuItem_t *TmpPtr = (struct MenuItem_t *)MoveToChild((VOID*)MenuPtr, SrcMenuArrayPtr);

    if ( TmpPtr )
    {
        *DestMenuArray = (VOID*)MenuPtr;
    }

    return TmpPtr;
}

VOID* MoveToPrevItem(VOID *CurMenuItem, VOID *MenuArrayPtr)
{
    if ( !CurMenuItem || !MenuArrayPtr )
        return NULL;

    S32 Size = GetItemsCount(MenuArrayPtr);
    //printf("MoveToPrevMenuItem current menu array Size = %d\n", Size);
    if ( Size <= 0 )
        return NULL;

    S32 Index = -1, Offset = 0, i = 0;
    struct MenuItem_t *ItemPtr = NULL;
    for ( i = 0; i < Size ; i++ )
    {
        ItemPtr = (struct MenuItem_t *)((S8 *)MenuArrayPtr + Offset);
        if ( ItemPtr == CurMenuItem )
        {
            Index = i;
            break;
        }

        Offset += sizeof(MenuItem_t);
    }

    if ( Index < 0 )
        return NULL;

    if ( Index == 0 ) //最开始的一个
        return CurMenuItem;

    Index -= 1; //向前移动一个menuitem
    //printf("MoveToPrevMenuItem current menu item index = %d\n", Index);
    ItemPtr = (struct MenuItem_t *)((S8 *)MenuArrayPtr + Index * sizeof(struct MenuItem_t));

    return ItemPtr;
}

VOID* MoveToNextItem(VOID *CurMenuItemPtr, VOID *MenuArrayPtr)
{
    if ( !CurMenuItemPtr || !MenuArrayPtr )
        return NULL;

    S32 Size = GetItemsCount(MenuArrayPtr);
    //printf("MoveToNextMenuItem current menu array Size = %d\n", Size);
    if ( Size <= 0 )
        return NULL;
    
    S32 Index = -1, Offset = 0, i = 0;
    struct MenuItem_t *ItemPtr = NULL;
    for ( i = 0; i < Size ; i++ )
    {
        ItemPtr = (struct MenuItem_t *)((S8 *)MenuArrayPtr + Offset);

        if ( ItemPtr == CurMenuItemPtr )
        {
            Index = i;
            break;
        }
        Offset += sizeof(struct MenuItem_t);
    }

    if ( Index < 0 )
        return NULL;

    if ( Index == (Size - 1) ) //最后一个
        return CurMenuItemPtr;

    Index += 1; //向前移动一个menuitem
    //printf("MoveToNextMenuItem current menu item index = %d\n", Index);
    ItemPtr = (struct MenuItem_t *)((S8 *)MenuArrayPtr + Index * sizeof(struct MenuItem_t));

    return ItemPtr;
}

S32 GetItemIndex(struct MenuItem_t *ItemPtr, struct MenuItem_t *MenuArrayPtr)
{
    if ( !ItemPtr  || !MenuArrayPtr)
        return -1;

    S32 Size = GetItemsCount(MenuArrayPtr);
    if ( Size <= 0 )
        return -1;
    
    struct MenuItem_t *ItemTmpPtr = NULL;
    S32 Index = -1, i = 0, Offset = 0;

    for ( i = 0; i < Size ; i++ )   //
    {
        ItemTmpPtr = (struct MenuItem_t *)((S8 *)MenuArrayPtr + Offset);
        if ( ItemTmpPtr== ItemPtr )
        {
            Index = i;
            break;
        }

        Offset += sizeof(struct MenuItem_t);
    }

    return Index;

}


