#include "SdCardMenuItem.h"
#include "CrView/CrShowView.h"
#include "../Action/MenuDefaultAction.h"
#include "../Show/ShowAction.h"
#include "../Show/LcmShow.h"
#include "PrintMenuItem.h"
#include "Common/Include/CrDirentAndFile.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/CommonQueue/CrMsgQueue.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrSdCard/Driver/CrSdCardDriver.h"
#include "CrInc/CrMsgType.h"
#include "CrInc/CrLcdShowStruct.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

static S32 FileListItemIndex = 0;  //文件列表中内容的索引
struct SdCardPageFileListMsg_t FileList;

static CrMsgQueue_t FileListSend;
static CrMsgQueue_t PrintFileSend;
static CrMsg_t ListRecv;

#define FILELISTMSG_PRI 1 
#define SD_SEND_SUCCESS_WAIT_TIMEOUT  30

S32 SdCardFileListInit( VOID )  //输入当前的menuitem数组的名称地址
{
    //创建队列
    memset(&FileList, 0, sizeof(FileList));

    FileListSend = InValidMsgQueue();
    PrintFileSend = InValidMsgQueue();

    FileListSend = ViewSendQueueOpen((S8 *)SD_LIST_REQUEST_QUEUE, sizeof(struct ViewPageInfoMsg_t));
    PrintFileSend = ViewSendQueueOpen((S8 *)SD_PRINTFILE_SET_QUEUE, sizeof(struct ViewSetPrintFileMsg_t));
    ListRecv = ViewRecvQueueOpen((S8 *)SD_LIST_REPORT_QUEUE, sizeof(struct SdCardPageFileListMsg_t));

    return IsValidMsgQueue(FileListSend) ? 1 : 0;
    
}

VOID *SdCardFileListOKAction(VOID *ArgOut, VOID *ArgIn)  //输入当前的menuitem数组的名称地址
{
      if ( !ArgOut )
        return NULL;

    S32 CurRow = FileListItemIndex % ((GetShowObject())->GetMaxRow());
    S32 *BackParentMenu = (S32 *)ArgOut;
    //返回上一级文件夹
    *BackParentMenu = 0;    //

    if ( FileList.FileList[CurRow].IsDirectory )
    {
        struct ViewPageInfoMsg_t Page;
        if ( 0 == FileListItemIndex )  //第一个文件夹是返回上一级
        {
            // 对上一级
            if ( 0 == strcmp( FileList.Path, SD_CARD_ROOT_DIR) )
            {
                *BackParentMenu = BACK_PARENT;
                return ArgOut;
            }

            sprintf(Page.Path, "%s/..", FileList.Path);

        }
        else
        {
            if ( 0 == strcmp( FileList.Path, SD_CARD_ROOT_DIR) )
            {
                sprintf(Page.Path, "%s%s", FileList.Path, FileList.FileList[CurRow].Name);
            }
            else
            {
                sprintf(Page.Path, "%s/%s", FileList.Path, FileList.FileList[CurRow].Name);
            }
        }

        Page.PageNo = 0;
        Page.PageSize = 5;
        strcpy(Page.Suffix, FILTER);

        S32 Res =  QueueSendMsg(FileListSend, ( S8 * )&Page, sizeof(struct ViewPageInfoMsg_t), FILELISTMSG_PRI);

        if ( Res != 0 )
            return NULL;

        CrM_Sleep(SD_SEND_SUCCESS_WAIT_TIMEOUT);

        struct SdCardPageFileListMsg_t List;
        S32 Prime = 0;
        if ( CrMsgRecv(ListRecv, (S8 * )&List, sizeof(struct SdCardPageFileListMsg_t), (U32*)&Prime) < 0 )
            return ArgOut;

        //FileList = FileListRecv;
        memcpy(&FileList, &List, sizeof(struct SdCardPageFileListMsg_t));
        FileListItemIndex = 0;
    }
    else
    {
#if 0
        //对文件的操作
        if ( strlen(FileList.FileList[CurRow].Name) <= FILE_NAME_LENGTH - 1 )
        {
            struct ViewSetPrintFileMsg_t file;
            strcpy(file.Name, FileList.FileList[CurRow].Name);
            strcpy(file.Path, FileList.Path);

            if ( !IsValidMsgQueue(PrintFileSend) )
                return NULL;

            //rt_kprintf("send print file msg res = %d, path = %s, filename = %s\r\n", Res, file.Path, file.Name);
            //接收消息
            if ( 0 != QueueSendMsg(PrintFileSend, ( S8 * )&file, sizeof(struct ViewSetPrintFileMsg_t), FILELISTMSG_PRI) )
                return NULL;

            CrM_Sleep(50);

            //set Mask
            HideMask |= (MAINMENU_ITEM_PRINTFFROMTF_HID_MASK | MAINMENU_ITEM_PREPARE_HID_MASK);
            HideMask &= ~(MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK | MAINMENU_ITEM_STOP_PRINTF_HID_MASK | MAINMENU_ITEM_TUNE_HID_MASK);
        }
        else /*Modify By Charley : Special handling of long name files */
        {

//            rt_kprintf("Current File Too long Index = %d  %s\n ", FileListItemIndex , FileList.Path , );
        }
#else
        struct ViewSetPrintFileMsg_t file;
//        strcpy(file.Name, FileList.FileList[CurRow].Name);
        file.Index = FileListItemIndex;
        strcpy(file.Path, FileList.Path);
        strcpy(file.Suffix, FILTER);

        if ( !IsValidMsgQueue(PrintFileSend) )
            return NULL;

        if ( 0 != QueueSendMsg(PrintFileSend, ( S8 * )&file, sizeof(struct ViewSetPrintFileMsg_t), FILELISTMSG_PRI) )
            return NULL;

        CrM_Sleep(60);

        //set Mask
        HideMask |= (MAINMENU_ITEM_PRINTFFROMTF_HID_MASK | MAINMENU_ITEM_PREPARE_HID_MASK);
        HideMask &= ~(MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK | MAINMENU_ITEM_STOP_PRINTF_HID_MASK | MAINMENU_ITEM_TUNE_HID_MASK);

#endif
        *BackParentMenu = BACK_ROOT;
    }

    CrBeepAppShortBeeps(); /* 短鸣 */

    return FileList.FileList[CurRow].Name;
}

VOID *SdCardFileListUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if (FileListItemIndex == 0)  //当前是首页
        return NULL;

    (FileListItemIndex)--;
    S32 RowCount = (GetShowObject())->GetMaxRow();
    if ( (FileListItemIndex % RowCount) == (RowCount - 1) )  //换了一页
    {
        //S32 Index = (FileListItemIndex / RowCount) * RowCount;
        //设置上一页界面
        S8 PageNum = ( FileListItemIndex / MAX_FILE_COUNT_IN_PAGE );
        
        //send message 
        ViewPageInfoMsg_t PageInfoMsg;
        PageInfoMsg.PageNo = PageNum;
        PageInfoMsg.PageSize = MAX_FILE_COUNT_IN_PAGE;

        strcpy(PageInfoMsg.Path, FileList.Path);
        strcpy(PageInfoMsg.Suffix, FILTER);
        S32 Ret = QueueSendMsg(FileListSend, ( S8 * )&PageInfoMsg, sizeof(struct ViewPageInfoMsg_t), FILELISTMSG_PRI);

        if ( 0 != Ret )
            return NULL;
        //接收消息
        CrM_Sleep(SD_SEND_SUCCESS_WAIT_TIMEOUT);
        struct SdCardPageFileListMsg_t List;
        S32 Prime = 0;
        if (CrMsgRecv(ListRecv, (S8 * )&List, sizeof(struct SdCardPageFileListMsg_t), (U32*)&Prime) < 0 )
            return ArgOut;

        //FileList = FileListRecv;
        memcpy(&FileList, &List, sizeof(struct SdCardPageFileListMsg_t));
    }

    ShowFileList();

    return NULL;
}

VOID *SdCardFileListDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( FileListItemIndex >= FileList.MaxCount - 1)   //达到最大行就不再移动
        return NULL;

    (FileListItemIndex)++;
    if ( FileListItemIndex % ((GetShowObject())->GetMaxRow()) == 0 )
    {
        //设置上一页界面
        S8 PageNum = (FileListItemIndex / MAX_FILE_COUNT_IN_PAGE);
        //send message 
        //S32 CurRow = FileListItemIndex % ((GetShowObject())->GetMaxRow());
        ViewPageInfoMsg_t PageInfoMsg;
        PageInfoMsg.PageNo = PageNum;
        PageInfoMsg.PageSize = MAX_FILE_COUNT_IN_PAGE;
        strcpy(PageInfoMsg.Path, FileList.Path);
        strcpy(PageInfoMsg.Suffix, FILTER);
        S32 Ret = QueueSendMsg(FileListSend, ( S8 * )&PageInfoMsg, sizeof(struct ViewPageInfoMsg_t), FILELISTMSG_PRI);

        if ( 0 != Ret )
            return NULL;

        CrM_Sleep(SD_SEND_SUCCESS_WAIT_TIMEOUT);

        struct SdCardPageFileListMsg_t List;
        S32 Prime = 0;
        if (CrMsgRecv(ListRecv, (S8 * )&List, sizeof(struct SdCardPageFileListMsg_t), (U32*)&Prime) < 0 )
            return ArgOut;

        //FileList = FileListRecv;
        memcpy(&FileList, &List, sizeof(struct SdCardPageFileListMsg_t));
    }

    ShowFileList();

    return NULL;
}

S32 SdCardFileListShowAction(VOID *Arg)
{
//    S8  Path[60] = {0};
    if ( strlen(FileList.Path) == 0 )  //当前的路径是否为空
    {
        //进入到根目录，并将内容显示出来
        //send message 
        struct ViewPageInfoMsg_t PageInfoMsg;
        PageInfoMsg.PageNo = 0;
        PageInfoMsg.PageSize = MAX_FILE_COUNT_IN_PAGE;

        strcpy(PageInfoMsg.Path, SD_CARD_ROOT_DIR);
        strcpy(PageInfoMsg.Suffix, FILTER);
        S32 Ret = QueueSendMsg(FileListSend, ( S8 * )&PageInfoMsg, sizeof(struct ViewPageInfoMsg_t), MOVEAXISMSG_PRI);

        if ( 0 != Ret )
            return 0;
        //接收消息
        CrM_Sleep(SD_SEND_SUCCESS_WAIT_TIMEOUT);
        struct SdCardPageFileListMsg_t List;
        S32 Prime = 0;
        if ( CrMsgRecv(ListRecv, (S8 * )&List, sizeof(struct SdCardPageFileListMsg_t), (U32*)&Prime) < 0 )
            return 0;

        memcpy(&FileList, &List, sizeof(struct SdCardPageFileListMsg_t));
    }

    ShowFileList();
    
    return 1;
}


VOID ShowFileList()
{
    S32 Param = 1;
    LcmShow *ShowPtr = GetShowObject();
    ShowPtr->Ioctl(LCD_CLEAN, (VOID *)&Param);

    S32 i, CurRow = FileListItemIndex % (ShowPtr->GetMaxRow());
    for ( i = 0; i < PAGE_FILE_COUNT; i++ )
    {
        if ( i >= ShowPtr->GetMaxRow() )  //超出旧不显示了
            break;

        S32 HighLight = 0;
        if ( i == CurRow )  //高亮
            HighLight = 1;

        //显示文件内容
        struct MenuShowInfo_t Info;
        Info.Highligh = HighLight;
        Info.Index = i;
        Info.MenuType = 3;
        if ( strcmp(FileList.FileList[i].Name, "..") )
            Info.MenuType = 1;
        Info.ShowData = (VOID *)(FileList.FileList[i].Name);
        ShowPtr->ShowString((S8 *)(Info.ShowData), Info.Index, 0, Info.Highligh);
//        ShowPtr->WriteString((S8 *)Info.ShowData, Info.Index * 12, 0, Info.Highligh);

    }

    ShowPtr->Ioctl(LCD_FLUSH, (VOID *)&Param);

    return ;
}

VOID ClearCurrentPath()
{
   // printf("Clean Current Path\n");
    FileListItemIndex = 0;
    memset(&FileList, 0, sizeof(FileList));
}
