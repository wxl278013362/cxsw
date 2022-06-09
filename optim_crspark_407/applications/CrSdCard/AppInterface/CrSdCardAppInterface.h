#ifndef _CRSDCARDAPPINTERFACE_H
#define _CRSDCARDAPPINTERFACE_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
*   Page是页号
*   MaxCount 是一页中最大的元素个数
*/
S32 CrSdCardGetFileNameToBeShow(S32 Page, struct FileInfo_t  *FileList, S32 MaxCount, S8 *Filter);    //返回经过筛选后的当前文件夹下的文件总个数
S8 *CrSdCardGetFileForIndex(S32 Index, S8 *Path, S8 *Filter);
#ifdef __cplusplus
}
#endif

#endif
