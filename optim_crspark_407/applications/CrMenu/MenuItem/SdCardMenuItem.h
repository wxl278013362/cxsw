#ifndef _SDCARDMENUITEM_H
#define _SDCARDMENUITEM_H

#include "CrInc/CrType.h"
#include "../Show/LcmShow.h"
//#include <dfs_posix.h>

//#define SD_CARD_ROOT_DIR  "/"    //SD卡的根目录
#define FILTER "gcode"    //筛选文件后缀
#define MAX_FILE_COUNT_IN_PAGE 5
//#define MAX_PATH_LENGTH 60

#ifdef __cplusplus
extern "C"
{
#endif

S32 SdCardFileListInit();

VOID *SdCardFileListOKAction(VOID *ArgOut, VOID *ArgIn);  //返回值为SD卡操作菜单项的父菜单数组数组的地址，ArgOut返回选中的文件名， ArgIn是SD卡操作的菜单数组地址

VOID *SdCardFileListUpAction(VOID *ArgOut, VOID *ArgIn);  //选择文件列表的上一页的内容

VOID *SdCardFileListDownAction(VOID *ArgOut, VOID *ArgIn); //选择文件列表下一个页的内容

S32 SdCardFileListShowAction(VOID *Arg);  //进入SD卡根目录，筛选内容

VOID ClearCurrentPath();

#if 0
 VOID SetShowPtr(LcmShow *Show);  //设置显示指针
 LcmShow          *ShowPtr;  //显示的内容
#endif

VOID ShowFileList();
// S8               LastPath[MAX_PATH_LENGTH];  //上一个路径
#ifdef __cplusplus
}
#endif
#endif
