#ifndef _GCODESOURCE_H
#define _GCODESOURCE_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"
#include "CrGcode/GcodeHandle/GcodeType.h"

#define MSG_ERR_LINE_NO                     "Line Number is not Last Line Number+1, Last Line: "
#define MSG_ERR_CHECKSUM_MISMATCH           "checksum mismatch, Last Line: "
#define MSG_ERR_NO_CHECKSUM                 "No Checksum with line number, Last Line: "

class GcodeSource
{
public:
    GcodeSource();
    virtual ~GcodeSource();

//    virtual S32 Open() = 0;    //开关放到各自的初始化和析构函数中去执行。
//    virtual S32 Close() = 0;
    virtual S32 GetCmd(S8 *CmdBuff, S32 Len) = 0; //读取一条命令（返回-1表示读失败(参数引起)， 0表示GcodeCmd已经读取完毕，大于0表示读取成功）
    virtual BOOL IsEnd() = 0;   //是否结束
    virtual float GetReadPercent() = 0;   //获取进度
    virtual S32 GetFd() = 0;
    virtual S8 *GetFileName() = 0;

    BOOL IsOpened();
protected:
    BOOL Opened;
};

class GcodeFile : public GcodeSource
{
public:
    GcodeFile(S8 *FileName, U32 FileOffSet);
    ~GcodeFile();

    S32 GetCmd(S8 *CmdBuff, S32 Len);     //读取一条命令
    BOOL IsEnd();   //是否结束
    float GetReadPercent();
    S32 GetFd() {return Fd;}
    //S8 *GetFileName() {return Name;}
    S8 *GetFileName();

private:
    S8 Name[FILE_NAME_LENGTH];
    S32 Fd; //文件描述符
};

class GcodeUart : public GcodeSource
{
public:
    GcodeUart(S32 UartRxFd, S32 UartTxFd = -1);
    virtual ~GcodeUart();

    S32 GetCmd(S8 *CmdBuff, S32 Len);     //读取一条命令
    BOOL IsEnd();   //是否结束
    float GetReadPercent();
    S32 GetFd();
    S8 *GetFileName();
    S32 SendData(S8 *CmdBuff, S32 Len);   //发送数据

    static S8 *VerifyCmd(S8 *VerifiedCmd);
    static BOOL GetVerifiedGcodeCmd(S8 *VerifiedCmd, S8 *CmdBuff);

    static S64 LastCmdNumber;
private:
    S32 Fd;    //接收
    S32 TxFd;  //发送端Fd
    S8  Buff[MAX_STRING_LENGTH];   //命令缓冲区
};

#endif
