#include "CrPrinter.h"
#include "CrSdCard.h"
#include "GcodeSource.h"
#include <stdio.h>
#include <drv_sdio.h>
#include "CrGpio/CrGpio.h"
#include "CrSdCard/Driver/CrSdCardDriver.h"
#include "CrSdCard/AppInterface/CrSdCardAppInterface.h"
#include "Common/Include/CrDirentAndFile.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrLog/CrLog.h"

#define COUNT(a) (sizeof(a)/sizeof(*a))
#define QUEQUE_SEND_PRI    1
#define QUEQUE_RECV_PRI    1

CrSdCard* SdCardModel = NULL;

//static S8 *CdParent();
//static S8 *CdPath(S8 *Path);

static CrMsg_t SendQueueInit( S8 *QueueName, S32 Size)
{
    if ( !QueueName || Size < 1 )
        return NULL;

    CrMsgAttr_t Attr;
    Attr.mq_flags = O_NONBLOCK;
    Attr.mq_maxmsg = 1;

    Attr.mq_msgsize = Size;
    return SharedQueueOpen(QueueName, O_CREAT | O_RDWR, 0x777, &Attr);
}

static CrMsgQueue_t RecvQueueInit(S8 *QueueName, S32 Size)
{
    if ( !QueueName || Size < 1 )
        return NULL;

    CrMsgAttr_t Attr;
    Attr.mq_flags = O_NONBLOCK;
    Attr.mq_maxmsg = 1;

    Attr.mq_msgsize = Size;
    return QueueOpen(QueueName, O_CREAT | O_RDWR, 0x777, &Attr);
}

static VOID SendQueueUnInit(CrMsg_t SendQueue)
{
    if (  !SendQueue )
        return ;

    CrMsgClose(SendQueue);
    SendQueue = NULL;
}

static VOID RecvQueueUnInit(CrMsgQueue_t RecvQueue)
{
    if ( !RecvQueue )
        return ;

    CrMsgQueueClose(RecvQueue);
    RecvQueue = InValidMsgQueue();
}

CrSdCard::CrSdCard(struct SdQueueName_t &Names, S8 *PinName)
    : CrModel()
{
    LogFd = -1;
    DevStatus = DEV_PLUCKED;
    SdDriverFd = -1;
    memset(FileName, 0, sizeof(FileName));
    memset(FullName, 0, sizeof(FullName));

    if ( PinName )
    {
        if ( (SdDriverFd = SdCardDriverOpen(PinName, 0, 0)) >= 0)
        {
            /*sd mount info*/
            struct SdCartMountParam_t Info =
            {
                SD_CARD_NAME,
                SD_CARD_ROOT_DIR,
                SD_CARD_FILE_SYSTEM_TYPE,
            };

            /*sd mount*/
            if (SdCardDriverIoctl(SdDriverFd, CMD_SD_CARD_MOUNT, (VOID *)&Info) >= 0 )
            {
                DevStatus = GetDevStatus();
                rt_kprintf("********SD Mount success!********\n");
            }
        }
    }

    RecvFileListMsg = InValidMsgQueue();
    RecvStatusMsg = InValidMsgQueue();
    RecvPrintFileMsg = InValidMsgQueue();
    RecvSetPrintFileMsg = InValidMsgQueue();

    SendFileListMsg = NULL;
    SendStatustMsg = NULL;
    SendPrintFileMsg = NULL;

    SendFileListMsg = SendQueueInit( Names.SendListQueueName, sizeof(struct SdCardPageFileListMsg_t));
    RecvFileListMsg = RecvQueueInit(Names.RecvListQueueName, sizeof(struct ViewPageInfoMsg_t));

    SendStatustMsg = SendQueueInit(Names.SendStatusQueueName, sizeof(struct SdStatusMsg_t));
    RecvStatusMsg = RecvQueueInit(Names.RecvStatusQueueName, sizeof(struct ViewReqDevStatusMsg_t));

    SendPrintFileMsg = SendQueueInit(Names.SendFileQueueName, sizeof(struct SdCardPrintFileMsg_t));
    RecvPrintFileMsg = RecvQueueInit(Names.RecvFileQueueName, sizeof(struct ViewSetPrintFileMsg_t));

    RecvSetPrintFileMsg = RecvQueueInit(Names.RecvGetFileQueueName, sizeof(struct ViewReqPrintFileMsg_t));

    SendMsgLineMsg = SendQueueInit( Names.MsgLineMsgQueueName, sizeof(struct MsgLine_t));
}

CrSdCard::~CrSdCard()
{
    struct SdCardUnmountParam_t Info =
    {
            SD_CARD_ROOT_DIR,
    };
    /*sd unmonut*/
    SdCardDriverIoctl(SdDriverFd, CMD_SD_CARD_UNMOUNT, (VOID *)&Info);

    SendQueueUnInit(SendFileListMsg);
    RecvQueueUnInit(RecvFileListMsg);

    SendQueueUnInit(SendStatustMsg);
    RecvQueueUnInit(RecvStatusMsg);

    SendQueueUnInit(SendPrintFileMsg);
    RecvQueueUnInit(RecvPrintFileMsg);

    RecvQueueUnInit(RecvSetPrintFileMsg);

    SdDriverFd = -1;
}

VOID CrSdCard::Exec()
{
    CheckSdCardStatus();
    FileListMsgOpt();
    PrintFileSetMsgOpt();
    PrintFileReqMsgOpt();
    SdStatusMsgOpt();

    return ;
}

VOID CrSdCard::FileListMsgOpt()
{
    if ( !IsValidMsgQueue(RecvFileListMsg) || !SendFileListMsg )
        return ;

    S8 MsgBuff[sizeof(struct ViewPageInfoMsg_t)] = {0};
    S32 MsgPri = QUEQUE_RECV_PRI;
    if ( RecvMsg(RecvFileListMsg, MsgBuff, sizeof(MsgBuff), &MsgPri) != sizeof(MsgBuff) )
        return ;

    struct SdCardPageFileListMsg_t TempInfo = {0};
    if ( FileListMsgHandle(MsgBuff, sizeof(MsgBuff), (S8 *)&TempInfo) > 0 )
        CrMsgSend(SendFileListMsg, (S8 *)&TempInfo, sizeof(TempInfo), QUEQUE_SEND_PRI);

    return ;
}

VOID CrSdCard::PrintFileSetMsgOpt()
{
    if ( !IsValidMsgQueue(RecvSetPrintFileMsg) )
        return ;

    S8 MsgBuff[sizeof(struct ViewSetPrintFileMsg_t )] = {0};
    S32 MsgPri = QUEQUE_RECV_PRI;
    if ( RecvMsg(RecvSetPrintFileMsg, MsgBuff, sizeof(MsgBuff), &MsgPri) <= 0 )
        return ;

    if( PrintFileSetMsgHandle(MsgBuff, sizeof(MsgBuff)) <= 0 )
        return ;

    struct SdCardPrintFileMsg_t  File;
    strcpy(File.Name, FileName);
    if ( SendPrintFileMsg )
        CrMsgSend(SendPrintFileMsg, (S8 *)&File, sizeof(File), QUEQUE_SEND_PRI);

    return;
}

VOID CrSdCard::PrintFileReqMsgOpt()
{
    if ( !IsValidMsgQueue(RecvPrintFileMsg) || !SendPrintFileMsg )
        return ;

    S8 MsgBuff[sizeof(struct ViewReqPrintFileMsg_t)] = {0};
    S32 MsgPri = QUEQUE_RECV_PRI;
    if ( RecvMsg(RecvPrintFileMsg, MsgBuff, sizeof(MsgBuff), &MsgPri) <= 0 )
        return ;
    if( PrintFileMsgHandle(MsgBuff, sizeof(MsgBuff)) <= 0)
        return ;

    if ( strlen(FileName) <= 0 )
        return;

    struct SdCardPrintFileMsg_t File;
    strcpy(File.Name, FileName);
    CrMsgSend(SendPrintFileMsg, (S8 *)&File, sizeof(File), QUEQUE_SEND_PRI);

    return ;
}

VOID CrSdCard::SdStatusMsgOpt()
{
    if ( !IsValidMsgQueue(RecvStatusMsg) || !SendStatustMsg )
        return ;

    S8 MsgBuff[sizeof(struct ViewReqDevStatusMsg_t )] = {0};
    S32 MsgPri = QUEQUE_RECV_PRI;
    if ( RecvMsg(RecvStatusMsg, MsgBuff, sizeof(MsgBuff), &MsgPri) <= 0 )
        return ;

    struct SdStatusMsg_t Temp = {0};
    if ( SdStatusMsgHandle(MsgBuff, sizeof(MsgBuff), (S8 *)&Temp) > 0)
        CrMsgSend(SendStatustMsg, (S8 *)&Temp, sizeof(Temp), QUEQUE_SEND_PRI);

    return ;
}

S8 *CrSdCard::GetPrintFileName()
{
    return FullName;
}

S32 CrSdCard::SetPrintFile(S8 *FullFileName, U32 FileOffset)
{
    if ( !FullFileName )
        return 0;

    if ( Print && Print->GetPrintStatus() != STOP_PRINT )
         return 0;

     GcodeSource *Gcode = new GcodeFile(FullFileName, FileOffset);

     if ( !Gcode || (Gcode->IsOpened() == 0) )
     {
         delete Gcode;
         return 0;
     }

     if ( Print->StartPrint(Gcode) <= 0 )
     {
         delete Gcode;
         return 0;
     }

     S8* Index = strstr(FullFileName, "/");
     S8* Index2 = strstr(FullFileName, "\\");

     memset(FullName, 0, sizeof(FullName));
     memset(FileName, 0, sizeof(FileName));
     strncpy(FullName, FullFileName, sizeof(FullName) -1 );

     if ( ( !Index ) && ( !Index2 ) )   //没有找到
     {
         strncpy(FileName, FullFileName, sizeof(FileName) -1 );
     }
     else if (Index)
     {
         Index2 = Index;
         while ( Index2 )
         {
             Index2 = strstr(Index2 + 1, "/");
             if ( Index2 )
             {
                 Index = Index2;
             }
         }

         strncpy(FileName, Index + 1, sizeof(FileName) -1 );
     }
     else
     {
         Index = Index2;
         while ( Index )
         {
             Index = strstr(Index + 1, "/");
             if ( Index )
             {
                 Index2 = Index;
             }
         }

         strncpy(FileName, Index2 + 1, sizeof(FileName) -1 );
     }

     return 1;
}

S8 *CrSdCard::GetFileName()
{
    return FileName;
}

S32 CrSdCard::SetPrint(CrPrinter *PrintPtr)
{
    if ( PrintPtr )
        Print = PrintPtr;

    return 0;
}

S32 CrSdCard::GetFileList(S8 *Path, S32 StartIndex, S8 *Suffix, struct FileInfo_t *FileList, S32 ListCount)
{
    if ( !Path || !FileList )
        return -1;

    CrChDir(Path);

   return CrSdCardGetFileNameToBeShow(StartIndex, FileList, ListCount, Suffix);
}

enum DevStatus_t CrSdCard::GetDevStatus()
{
    struct PinParam_t Buff = {GPIO_PIN_RESET};
    SdCardDriverIoctl(SdDriverFd, CMD_SD_CARD_GET_STATUE, (VOID *)&Buff);



    return (Buff.PinState == GPIO_PIN_RESET) ? DEV_INSERTED : DEV_PLUCKED;
}

S32 CrSdCard::FileListMsgHandle(S8 *Buff, S32 Len, S8 *OutBuff)
{
    if ( !Buff || (Len < (S32)sizeof(ViewPageInfoMsg_t)) || !OutBuff )
        return 0;

    struct ViewPageInfoMsg_t *Temp = (struct ViewPageInfoMsg_t *)Buff;
    if ( Temp->PageSize > PAGE_FILE_COUNT )
        Temp->PageSize = PAGE_FILE_COUNT;

    struct SdCardPageFileListMsg_t *TempInfo = (struct SdCardPageFileListMsg_t *)OutBuff;
    S32 FileNum = GetFileList(Temp->Path, Temp->PageNo, Temp->Suffix, TempInfo->FileList, Temp->PageSize);
    TempInfo->MaxCount = FileNum;
    TempInfo->PageNo = Temp->PageNo;
    CrGetCwd(TempInfo->Path, sizeof(TempInfo->Path) - 1);
    TempInfo->Count = Temp->PageSize;

    return 1;
}

S32 CrSdCard::SdStatusMsgHandle(S8 *Buff, S32 Len, S8 *Out)
{
    if ( !Buff || !Out || Len < (S32)sizeof(struct ViewReqDevStatusMsg_t ) )
        return 0;

    struct SdStatusMsg_t *Status = (struct SdStatusMsg_t *)Out;
    Status->Inserted = GetDevStatus();

    return 1;
}

S32 CrSdCard::PrintFileMsgHandle(S8 *Buff, S32 Len)
{
    if ( !Buff || (Len < (S32)sizeof(struct ViewReqPrintFileMsg_t)) )
        return 0;

    struct ViewReqPrintFileMsg_t *File = (struct ViewReqPrintFileMsg_t *)Buff;
    if ( File->ReqFile == 0 )
        return 0;

    return 1;
}

VOID CrSdCard::CheckSdCardStatus()
{
    enum DevStatus_t Status = GetDevStatus();
    if ( Status == DevStatus )
        return ;

    //不相等，要进行文件系统的加载和卸载操作
    if (DevStatus == DEV_INSERTED)
    {
        struct SdCardUnmountParam_t Param = {SD_CARD_ROOT_DIR};
        SdCardDriverIoctl(SdDriverFd, CMD_SD_CARD_UNMOUNT, (VOID *)&Param);
        DevStatus = DEV_PLUCKED;

        struct MsgLine_t Msg = {0};
        //LOG("SD Card Inserted");
        CrLogClose();
        strcpy(Msg.Info, "SD Card Removed");
        rt_kprintf("msg is : %s\n", Msg.Info);
        if ( SendMsgLineMsg )
            CrMsgSend(SendMsgLineMsg, (S8 *)&Msg, sizeof(Msg), 1);
    }
    else
    {
        struct SdCartMountParam_t Info =
        {
            SD_CARD_NAME,
            SD_CARD_ROOT_DIR,
            SD_CARD_FILE_SYSTEM_TYPE,
        };
        if ( !SdCardDriverIoctl(SdDriverFd, CMD_SD_CARD_MOUNT, (VOID *)&Info) )
        {
            DevStatus = DEV_INSERTED;
            struct MsgLine_t Msg = {0};
            //LOG("SD Card Inserted");
            strcpy(Msg.Info, "SD Card Inserted");
            rt_kprintf("msg is : %s\n", Msg.Info);
            if ( SendMsgLineMsg )
                CrMsgSend(SendMsgLineMsg, (S8 *)&Msg, sizeof(Msg), 1);
        }
    }

    return ;
}

VOID CrSdCard::Mount()
{
    struct SdCartMountParam_t Info =
    {
        SD_CARD_NAME,
        SD_CARD_ROOT_DIR,
        SD_CARD_FILE_SYSTEM_TYPE,
    };

    if ( !SdCardDriverIoctl(SdDriverFd, CMD_SD_CARD_MOUNT, (VOID *)&Info) )
    {
        DevStatus = DEV_INSERTED;
        struct MsgLine_t Msg = {0};
        strcpy(Msg.Info, "SD Card Inserted");
        rt_kprintf("msg is : %s\n", Msg.Info);
        if ( SendMsgLineMsg )
            CrMsgSend(SendMsgLineMsg, (S8 *)&Msg, sizeof(Msg), 1);
    }
}

VOID CrSdCard::Release()
{
    struct SdCardUnmountParam_t Param = {SD_CARD_ROOT_DIR};
    SdCardDriverIoctl(SdDriverFd, CMD_SD_CARD_UNMOUNT, (VOID *)&Param);
    DevStatus = DEV_PLUCKED;

    struct MsgLine_t Msg = {0};
    strcpy(Msg.Info, "SD Card Removed");
    rt_kprintf("msg is : %s\n", Msg.Info);
    if ( SendMsgLineMsg )
        CrMsgSend(SendMsgLineMsg, (S8 *)&Msg, sizeof(Msg), 1);
}

S32 CrSdCard::PrintFileSetMsgHandle(S8 *Buff, S32 Len)
{
    if ( !Buff || (Len < (S32)sizeof(struct ViewSetPrintFileMsg_t)) )
        return 0;

    if ( Print && Print->GetPrintStatus() != STOP_PRINT )
        return 0;

    /*Modify by Charley*/
    S8 Name [FILE_NAME_LENGTH + MAX_PATH_LENGTH] = {0};
    struct ViewSetPrintFileMsg_t *File = (struct ViewSetPrintFileMsg_t *)Buff;

    S8 *PrintfFileName = CrSdCardGetFileForIndex(File->Index, File->Path, File->Suffix);

    if ( strcmp(File->Path, SD_CARD_ROOT_DIR) == 0 )
    {
        snprintf(Name, sizeof(Name) - 1, "/%s", PrintfFileName);
    }
    else
    {
        snprintf(Name, sizeof(Name) - 1, "%s/%s", File->Path, PrintfFileName);  //将文件名保存
    }

    rt_kprintf("print file name = %s, Led : %d\n", Name, strlen(Name));

    return SetPrintFile(Name, 0);
}

S32 CrSdCard::OpenLogFile(S8 *FileName)
{
    if ( !FileName )
       return 0;

    if ( LogFd >= 0 )
        CrClose(LogFd);

    printf("Now doing file: %s\n", FileName);

    LogFd = CrOpen(FileName, O_CREAT | O_APPEND | O_WRONLY | O_TRUNC, 0x666);
    S8* File = FileName;
    S8* Index = strchr(FileName, '/');
    while ( Index )
    {
        File = Index + 1;
        Index = strchr(File, '/');
    }

    if ( LogFd < 0 )
    {
        if ( File )
            printf("open failed, File: %s.\n", File);
        CrClose(LogFd);
        return 0;
    }

    //flag.saving = true;
    //selectFileByName(fname);

    if ( File )
        printf("Writing to file: %s\n", File);
//    ui.set_status(fname);


    return 1;
}

#if 0
S8 *CdPath(S8 *Path)
{
    if ( !Path )
        return NULL;

    S8 CurPath[DFS_PATH_MAX] = {0};

    return (CrChDir(Path) == 0) ? CrGetCwd(CurPath, sizeof(CurPath) - 1) : NULL;
}

S8 *CdParent()
{
    S8 *CurPath = CdPath((S8 *)"./..");
    return CurPath;
}
#endif

