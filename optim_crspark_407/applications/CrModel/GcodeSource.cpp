#include "GcodeSource.h"
#include <string.h>
#include "Common/Include/CrDirentAndFile.h"
#include "CrGcode/GcodeCmdGet/GcodeCmdGet.h"
#include "CrSerial/CrSerial.h"
#include <rtthread.h>

GcodeSource::GcodeSource()
    : Opened(0)
{

}

GcodeSource::~GcodeSource()
{
    Opened = 0;
}

BOOL GcodeSource::IsOpened()
{
    return Opened;
}


GcodeFile::GcodeFile(S8 *FileName, U32 FileOffSet)
    : GcodeSource(),
      Fd(-1)
{
    memset(Name, 0, sizeof(Name));
    if ( FileName )
    {
        Fd = GcodeFileOpen(FileName);
        if ( Fd >= 0 )
        {
            strncpy(Name, FileName, sizeof(Name) - 1);
            Opened = 1;
            lseek(Fd, FileOffSet, 0);  //����ƫ��
            rt_kprintf("Open File: %s Success, Fd = %d\n", FileName, Fd);
        }
        else
        {
            rt_kprintf("Open File:%s Failed, Fd = %d\n",FileName, Fd);
        }
    }
}

GcodeFile::~GcodeFile()
{
    if ( Fd >= 0 )
        GcodeFileClose(Fd);

    Fd = -1;
    Opened = 0;
    memset(Name, 0, sizeof(Name));
}

S32 GcodeFile::GetCmd(S8 *CmdBuff, S32 Len)
{
    if ( !CmdBuff || Len <= 1 || !IsOpened() || (Fd < 0) )
        return -1;

    S32 Res = GcodeFileGetCmd(Fd, CmdBuff);
    return Res;
}

BOOL GcodeFile::IsEnd()
{
    if ( !IsOpened() || (Fd < 0) )
        return 1;

    return FileIsEnd(Fd);
}

float GcodeFile::GetReadPercent()
{
    return GetPercent(Fd);
}

S8 *GcodeFile::GetFileName()
{
    return Name;
}


S64 GcodeUart::LastCmdNumber = 0;

GcodeUart::GcodeUart(S32 UartFd, S32 UartTxFd)
{
    Fd = UartFd;
    TxFd = UartTxFd;
    if ( Fd < 0 )
    {
        printf("Serial open Rx Failed.\n");
    }

    if ( TxFd < 0 )
    {
        printf("Serial open Tx Failed.\n");
    }
    memset(Buff, 0, sizeof(Buff));
}

GcodeUart::~GcodeUart()
{
    if ( Fd >= 0 )
    {
        CrSerialClose(Fd);
    }

    if ( TxFd >= 0 )
    {
        CrSerialClose(Fd);
    }
    TxFd = -1;
    Fd = -1;
}

S32 GcodeUart::GetCmd(S8 *CmdBuff, S32 Len)
{
    S8 SerialDate[MAX_STRING_LENGTH] = {'0'};     //传输的中间数组
    S8 Char = '0';
    S32 Count = 0;

    //1、当前串口缓冲区是否可读，不可读则退出
    //2、从串口读取单个字节
    //3、如果数据 < 0，输出报错
    //4、判断数据是否是结束符（\n或者\r）,如果是结束符则说明一条Gcode命令传输完成
    //5、是结束符则对中间数组的内容进行处理：如果有特定的Gcode命令（M108、M410、M112、N开头的命令）进行特定的处理
    //6、不是则放在中间数组内
    //7、最后返回Gcode命令和长度

    while(CrSerialIoctl(Fd, SERIAL_AVAILABLE, NULL))
    {
        if ( CrSerialRead(Fd, &Char, 1) )
        {
            if ( Char < 0 )
            {
                printf("Serial available but read -1.\n");
                continue;
            }
        }

        if ( Char == '\n' || Char == '\r' )
        {
            S8 Len = strlen(Buff);
            if ( Len )
            {
                memcpy(CmdBuff, Buff, Len);
                memset(Buff, 0, sizeof(Buff));
            }

//            for( int i = 0; i < Count; i++ )
//            {
//                CmdBuff[i] = SerialDate[i];
//            }
            memcpy(CmdBuff + Len, SerialDate, Count);
            //printf("LOGD:++++++++++Cmdbuff %s\n", CmdBuff);

            return Count + Len;
        }
        else
        {
            SerialDate[Count++] = Char;
        }
    }

    if ( Count )  //读取了部分
    {
        memcpy(Buff + strlen(Buff), SerialDate, Count); //缓存数据
        Buff[strlen(Buff) + 1] = '\0';
        //printf("LOGD:------------------------------------------Cmd %s\n", Buff);
    }


    return 0;
}

S32 GcodeUart::SendData(S8 *CmdBuff, S32 Len)
{
    if ( TxFd < 0 )
        return 0;

    return (S32)CrSerialWrite(TxFd, CmdBuff, Len);
}

BOOL GcodeUart::IsEnd()
{
    return 1;
}

float GcodeUart::GetReadPercent()
{

    return 1.0;
}

S32 GcodeUart::GetFd()
{
    return 1;
}

S8* GcodeUart::GetFileName()
{
    return NULL;
}

S8 *GcodeUart::VerifyCmd(S8 *VerifiedCmd)
{
    if ( !VerifiedCmd )
        return NULL;

    char* command = VerifiedCmd;

    while (*command == ' ') command++;                // Skip leading spaces
    char *npos = (*command == 'N') ? command : NULL;  // Require the N parameter to start the line

    if (npos)
    {
          bool M110 = strstr(command, "M110") != NULL;

          if (M110) {
            char* n2pos = strchr(command + 4, 'N');
            if (n2pos) npos = n2pos;
          }

          S64 GcodeNum = atoi(npos + 1);

          if (GcodeNum != GcodeUart::LastCmdNumber + 1 && !M110)
          {
              printf("%s%d\n", MSG_ERR_LINE_NO, GcodeUart::LastCmdNumber);
              return NULL;
          }

          char *apos = strrchr(command, '*');
          if (apos)
          {
            uint8_t checksum = 0, count = uint8_t(apos - command);
            while (count) checksum ^= command[--count];
            if (strtol(apos + 1, nullptr, 10) != checksum)
            {
                printf("%s%d\n", MSG_ERR_CHECKSUM_MISMATCH, GcodeUart::LastCmdNumber);
                return NULL;
            }

          }
          else
          {
              printf("%s%d\n", MSG_ERR_NO_CHECKSUM, GcodeUart::LastCmdNumber);
              return NULL;
          }

          GcodeUart::LastCmdNumber = GcodeNum;
    }

    return VerifiedCmd;
}

BOOL GcodeUart::GetVerifiedGcodeCmd(S8 *VerifiedCmd, S8 *CmdBuff)
{
    if ( !VerifiedCmd || !CmdBuff )
        return false;

    char* command = VerifiedCmd;

    while (*command == ' ') command++;                // Skip leading spaces
    char *npos = (*command == 'N') ? command : NULL;  // Require the N parameter to start the line

    if ( npos )   //含有行校验符
    {
        char *apos = strrchr(command, '*');
        if ( !apos )
            return false;

        *apos = '\0';
        apos = strchr(command, ' ');
        if ( !apos )
            return false;

        strcpy(CmdBuff, apos + 1);
    }
    else
    {
        strcpy(CmdBuff, VerifiedCmd);
    }

    return true;
}



#if 0
S32 GcodeUart::SourceOpen()
{


}

S32 GcodeUart::SourceClose()
{

}
#endif


