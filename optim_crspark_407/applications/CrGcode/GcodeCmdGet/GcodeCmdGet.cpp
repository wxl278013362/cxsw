/****************************************************************************
#include <GcodeCmdGet.h>
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     SdGcode.cpp
  Author :        ChenLi
  Version:        1.0
  Description:    解析Gcode文件
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          ChenLi         2020-11-6     V1.0.0.1
****************************************************************************/
#include "GcodeCmdGet.h"
#include "dfs_posix.h"
#include <string.h>
#include "Common/Include/CrDirentAndFile.h"

#define USER_DBG(Fmt, Args...)\
    do\
    {\
        fprintf(stdout, " USER_DBG(%s:%d): " Fmt "\n", __FUNCTION__, __LINE__, ##Args);\
    }while(0);

#define USER_ERR(Fmt, Args...)\
    do\
    {\
        fprintf(stderr, "\033[1;31m  USER_ERR(%s:%u): \033[0m" Fmt "\n", __FUNCTION__, __LINE__, ##Args);\
    }while(0);

struct FileInfo_t
{
    S8    FileName[MAX_FILE_NAME];
    S32   Fd;
    S64   ParseByteCount;               //已經處理的字節個數
};

static struct FileInfo_t FileInfo[MAX_FILE_OPEN] = {0};

S32 GetFileSize(S32 Fd)
{
    S32 FileLen = -1;

    if ( Fd >= 0 )
    {
        FileLen = lseek(Fd, 0, SEEK_END);
    }

    return FileLen;
}

S32 GcodeFileOpen(S8 *FileFullName)
{
    if ( !FileFullName )
        return -1;

    S32 FreeFlag = -1, Find = 0;
    S32 Fd = open(( const char * )FileFullName, O_RDONLY, 0x777);
    if ( Fd < 0 )
    {
//        close(Fd);
        return -1;
    }
    //rt_kprintf("open file %s fd %d\n", FileFullName, Fd);
    for ( int i = 0; i < MAX_FILE_OPEN; i++ )
    {
        if ( FreeFlag < 0)
        {
            if ( strlen((const char *)FileInfo[i].FileName) <= 0 )
            {
                //rt_kprintf("%s: i = %d\n", __FUNCTION__, i);
                FreeFlag = i;
            }
        }

        if ( strcmp((const char *)FileInfo[i].FileName, (const char *)FileFullName ) == 0 )
        {
            //rt_kprintf("%s: i = %d, file %s already open\n", __FUNCTION__, i, FileFullName);
            if ( FileInfo[i].Fd >= 0)   //找到文件并已经打开
            {
                //rt_kprintf("%s: i = %d, Found file %s already open\n", __FUNCTION__, i, FileFullName);
                Find = 1;
                break;
            }

            FreeFlag = i;   //文件未打开
            break;
        }
    }

    if ( FreeFlag < 0 || Find )
    {
        close(Fd);
        return -1;
    }

    FileInfo[FreeFlag].Fd = Fd;
    strncpy(FileInfo[FreeFlag].FileName, FileFullName, sizeof(FileInfo[FreeFlag].FileName) - 1 );

    return FreeFlag;
}

VOID GcodeFileClose(S32 Fd)
{
    if ( Fd < 0 )
        return ;

     close(FileInfo[Fd].Fd);
     FileInfo[Fd].Fd = -1;
     FileInfo[Fd].ParseByteCount = 0;
     rt_kprintf("~~~~~~~~~~ Close File %s \n", FileInfo[Fd].FileName);
     memset(FileInfo[Fd].FileName, 0, sizeof(MAX_FILE_NAME));

    return ;
}

S32 GetRealCmd(S8 *Buf, int Len, S8 *ResultBuf, struct FileInfo_t *File)
{
    S32 Res = 1;
    if ( !Buf || !ResultBuf || !File)
        return Res;

    S32 Count = 0, i = 0;      //Gcode命令的字符个数
    static S8  Flag = true;    //当前的字符串是否是不是Gcode命令
    S32 CmdComplete = 0;    //命令是否取完整（0：表示未取完整，1：表示完整）
    S8 *Ptr = Buf;

    for( i = 0 ; i < Len ; i++ )
    {
        File->ParseByteCount++;
        if ( Ptr[i] == ';' )   //判断开始处理注释部分的字符
        {
            Flag = false;
            continue;
        }

        if ( Flag == false )    //处理注释部分的字符
        {
            if ( Ptr[i] == '\n' ) //判断处理注释部分的字符结束
            {
                Flag = true;      //无效字符已读取完毕
                if ( strlen(ResultBuf) >= 2 )   //去除空行，正确命令长度最少为2，故取2
                    CmdComplete = 1;
                break;
            }
            continue;
        }
        else
        {
             ResultBuf[Count] = Ptr[i];
             if(Ptr[i] == '\n')
             {
                ResultBuf[Count - 1] = '\0';
                if ( strlen(ResultBuf) >= 2 ) //去除空行，正确命令长度最少为2，故取2
                    CmdComplete = 1;
                break;
             }
             Count++;
        }
    }

    Res = (Count << 8) + CmdComplete;

    return Res;
}

S32 SdGcodeParseFunc(S32 Fd, S8 *Buf)
{
    if ( !Buf || Fd < 0 )
        return -2;   //参数传错了

    S32 ReadByte = 0;
    S32 Count = 0;
    S8 Temp[READ_BYTE_COUNT_ONCE + 1] = {0};
    struct FileInfo_t *Info = &FileInfo[Fd];

    if ( !Info )
        return -2;

    S32 Offset = 0;
    do
    {
        lseek(Info->Fd, (long)Info->ParseByteCount, 0);  //每次从上次解析完的位置开始读取
        memset(Temp, 0, sizeof(Temp));

        ReadByte = read(Info->Fd, Temp, READ_BYTE_COUNT_ONCE) ;
        if ( ReadByte <= 0 )
        {
            close(FileInfo[Count].Fd);
            printf("ReadByt = %d\n", ReadByte);
            return ReadByte;   //ReadByte等于0表示读完了，等于-1表示出错了。
        }

        S32 Res = GetRealCmd(Temp, ReadByte, Buf, &FileInfo[Count]);
        Buf[strlen(Buf)] = '\0';
        if ( Res )
        {
            Offset += Res >> 8;
            if ( ((Res & 0x01) == 1) && Offset )
            {
                break;
            }
        }

    }while( 1 );

    return Offset;
}

S32 GcodeFileGetCmd(S32 Fd, S8 *Buf)
{
    if ( !Buf || Fd < 0 )
        return 0;

    return SdGcodeParseFunc(Fd, Buf);
}

FLOAT GetPercent(S32 Fd)
{
    if ( Fd < 0 )
        return 0;

    S64 Len = CrLseek(FileInfo[Fd].Fd, 0, SEEK_CUR);
    S64 Total = CrLseek(FileInfo[Fd].Fd, 0, SEEK_END);

    return (Len * 1.0) / (Total * 1.0);
}

BOOL FileIsEnd(S32 Fd)
{
    S64 Len = CrLseek(FileInfo[Fd].Fd, 0, SEEK_CUR);
    S64 Total = CrLseek(FileInfo[Fd].Fd, 0, SEEK_END);
    if ( Len == Total )
        return 1;

    return 0;
}

S32 GetFileFd(S32 Flag)
{
    return FileInfo[Flag].Fd;
}

VOID Test()
{
    S32 Fd = GcodeFileOpen(( S8 * )"/ccc.gcode");
    if ( Fd < 0 )
    {
        USER_DBG("Open file error");
        return ;
    }

    S8 Buf[100] = {0};
    for ( ; ; )
    {
        memset(Buf, 0, 100);
        S32 Ret = GcodeFileGetCmd(Fd, Buf);
        if ( Ret == 0 )
        {
            USER_ERR("Read ERR!\n");
            break;
        }
        else if ( Ret == -1 )
        {
            USER_ERR("File End!\n");
            break;
        }
        else
            USER_ERR("~~~~~~~~~~~~~~~~~~~~~~~~~Recv Cmd %s: %s","/ccc.gcode", Buf);
    }
    GcodeFileClose(Fd);
}

VOID TestMany()
{
    S32 Fd = GcodeFileOpen(( S8 * )"/fk20x20x20.gcode");
    if ( Fd < 0 )
    {
        USER_DBG("Open file error2");
        return ;
    }
    printf("Fd = %d\n", Fd);
    S8 Buf[100] = {0};
    for ( int i = 0; i < 20; i++)
    {
        memset(Buf, 0, 100);
        S32 Ret = GcodeFileGetCmd(Fd, Buf);
        if ( Ret == 0 )
        {
            USER_ERR("Read ERR!\n");
            return;
        }
        else if ( Ret == -1 )
        {
            USER_ERR("File End!\n");
            return ;
        }
        else
        USER_ERR("~~~~~~~~~~~~~~~~~~~~~~~~~Recv Cmd %s: %s, %x, %x,%x","/qqq.gcode", Buf, Buf[0], '\n', ' ');
    }
    GcodeFileClose(Fd);
}

MSH_CMD_EXPORT(TestMany, Test many file sample);
MSH_CMD_EXPORT(Test, Test one file sample);

