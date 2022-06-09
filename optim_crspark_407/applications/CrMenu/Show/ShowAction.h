#ifndef _SHOWACTION_H
#define _SHOWACTION_H

#include <stdio.h>
#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrView/CrShowView.h"
#include "LcmShow.h"

#define NOZZLE_MIN_EXTRUDED_TEMPER (180)

#ifdef __cplusplus
extern "C" {
#endif

extern struct ShowPrintInfo_t ShowInfo;

S32 ShowInit(LcmShow *ShowObjectPtr, BOOL SwitchPageMode = 0);    //设置翻页方式, 设置翻页的方式：1：一次切换一整个页，0：一次切换一行
LcmShow* GetShowObject(); //获取显示类对象指针
S32 ShowItems(struct MenuItem_t *Array, S32 Count, S32 CurItemIndex); //显示菜单数组中的所有子菜单
S32 ShowRoot(VOID *DataPtr);      //显示根菜单(即开机后的界面), DataPtr是显示的参数的内容
S32 ShowBootScreen(VOID *DataPtr);    //显示系统界面

//菜单的默认显示函数
S32 ShowItemName(VOID *MenuInfoPtr);        //显示菜单子项的名称(默认接口)

VOID ShowActTimerCallback(VOID *Param);

#ifdef __cplusplus
}
#endif

#endif


