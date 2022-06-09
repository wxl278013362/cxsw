#ifndef _CRSDCARDMODEL_H
#define _CRSDCARDMODEL_H

#include "CrModel.h"
#include "CrPrinter.h"
#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrMsgQueue/CommonQueue/CrMsgQueue.h"
#include "CrInc/CrMsgType.h"


struct SdQueueName_t
{
    S8 SendListQueueName[20];
    S8 RecvListQueueName[20];
    S8 SendStatusQueueName[20];
    S8 RecvStatusQueueName[20];
    S8 SendFileQueueName[20];
    S8 RecvFileQueueName[20];
    S8 RecvGetFileQueueName[20];
    S8 MsgLineMsgQueueName[20];
};


//SD卡的初始化和窗台的
class CrSdCard : public CrModel
{
public:
    CrSdCard(struct SdQueueName_t &Names, S8 *PinName);
    ~CrSdCard();

    VOID Exec();                           //在exec中进行数据的处理
    S8 *GetPrintFileName();                //获取打印文件名（全名）
    S8 *GetFileName();                     //获取打印文件名（不含路径）
    S32 SetPrint(CrPrinter *PrintPtr);       //在文件正在打印的过程中不能二次设置打印文件，所以需要获取打印的状态
    S32 SetPrintFile(S8 *FullFileName, U32 FileOffset);    //设置打印文件名，并开始打印
    VOID Mount();                          //M21
    S32 OpenLogFile(S8 *FileName);   //打开日志文件
    VOID Release();                        //M22
    enum DevStatus_t GetDevStatus();

private:
    S32 GetFileList(S8 *Path, S32 StartIndex, S8 *Suffix, struct FileInfo_t *FileList, S32 ListCount);

    S32 FileListMsgHandle(S8 *Buff, S32 Len, S8 *OutBuff);
    S32 SdStatusMsgHandle(S8 *Buff, S32 Len, S8 *Out);
    S32 PrintFileSetMsgHandle(S8 *Buff, S32 Len);
    S32 PrintFileMsgHandle(S8 *Buff, S32 Len);
    VOID CheckSdCardStatus();    //检查SD卡的状态

    VOID FileListMsgOpt();
    VOID PrintFileSetMsgOpt();
    VOID PrintFileReqMsgOpt();
    VOID SdStatusMsgOpt();

    CrMsg_t SendFileListMsg; //发送文件列表（消息队列使用共享队列）
    CrMsg_t SendStatustMsg;  //发送状态
    CrMsg_t SendPrintFileMsg; //发送打印文件名
    CrMsg_t SendMsgLineMsg;   //信息行信息

    CrMsgQueue_t RecvFileListMsg; //接收文件列表查询
    CrMsgQueue_t RecvStatusMsg;   //接收状态查询
    CrMsgQueue_t RecvPrintFileMsg; //接收确认打印文件名请求
    CrMsgQueue_t RecvSetPrintFileMsg; //接收请求打印文件名请求


    CrPrinter *Print;

    S8 FileName[FILE_NAME_LENGTH];   //打印文件名
    S8 FullName[MAX_PATH_LENGTH];   //打印文件路径
    S32 SdDriverFd;
    S32 LogFd;

    enum DevStatus_t DevStatus;   //卡的状态
};

extern CrSdCard* SdCardModel;

#endif
