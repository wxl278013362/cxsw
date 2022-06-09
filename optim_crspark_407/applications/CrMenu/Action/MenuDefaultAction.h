#ifndef __MENU_DEFAULT_ACTION_H__
#define __MENU_DEFAULT_ACTION_H__

#include <stdio.h>
#include <string.h>
//#include <map>

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrView/CrShowView.h"

//#ifdef __cplusplus
//extern "C" {
//#endif

//菜单的其他接口

VOID *InitMenuDefaultAction();

S32 UnInitMenuDefaultAction();
S32 GetItemIndex(struct MenuItem_t *ItemPtr, struct MenuItem_t *MenuArrayPtr);
S32 GetItemsCount(VOID *      MenuArrayPtr);
struct MenuItem_t * GetRootMenu();
VOID* MoveToParant(VOID **DestMenuArray, VOID *SrcMenuArrayPtr);

//菜单的动作接口
VOID *MoveToRoot();
VOID* MoveToChild(VOID *DestMenuArrayPtr, VOID *SrcMenuArrayPtr);
VOID* MoveToPrevItem(VOID *CurMenuItem, VOID *MenuArrayPtr);
VOID* MoveToNextItem(VOID *CurMenuItemPtr, VOID *MenuArrayPtr);

//#ifdef __cplusplus
//}
//#endif

#endif


