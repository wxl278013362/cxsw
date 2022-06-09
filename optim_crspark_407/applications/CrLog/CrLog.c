#include "CrLog.h"

#ifdef USING_OS_LINUX

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

extern int errno;

typedef struct stat Stat_t;
#else

#include <rtthread.h>
#include <dfs_posix.h>
#include <rtlibc.h>
#include <rtdef.h>

typedef struct stat Stat_t;
#endif

#define VALUEFLAG (0x01)
#define MAX_SIZE 200
#define PERMISSION (S_IRWXG | S_IRWXO | S_IRWXU)

static S32 UartFd = -1;
static S32 FileFd = -1;
static S32 VcpFd  = -1;
static S32 TermFd = -1;

static CrPthreadMutex_t MutexLog;
static BOOL BoolUsingMutexFlag = 0; //是否使用锁的标志

#define MAXFILECOUNT 4  //一个类型文件的最大个数
#define MAXFILELENGTH (10*1024*1024)
//#define MAXFILELENGTH (256*1024)  //测试使用


//LogName(日志文件的名称)
static const U8 UartName[] = "UartLog";
static const U8 FileName[] = "FileLog";
static const U8 VcpName[] = "VcpLog";
static const U8 TermName[] = "TermLog";

static struct CrLogAttr_t   LogAttr;   //保存属性的接口


//检查文件是否更换
static S16 IsChangedFile(U32 Fd, S32 FileMaxSize);

//拼接文件名
/*
*   文件的全名是由文件路径+文件名+文件下标+文件类型组成
*   FileName 输出缓冲区，
*   path 日志文件路径
*   LogName 这个是固定的名称，在文件中有定义
*   最后加上时间的年月日时分秒
*/
static S16 CatFileName(U8* FileName, const U8* Path, const U8* LogName); 


//统计文件夹下的文件个数
static S16 CountLogFile(const S8* Path, const S8* Filter);

// 获取修改时间最早的那个文件ming
static S16 GetMinModificationTimeFileName(S8* Name, const S8* Path, const S8* Filter);

// 删除文件夹下超过文件个数限制的文件
static VOID DeleteFileOverTheLimit(const S8* Path, const S8* Filter);

// 获取存在文件的最大的下标
static S32 MaxIndexOfLogFile();


static U32 CrUartWrite(U32 Fd, const S8 *Buff, const U32 Size);
static U32 CrFileWrite(U32 Fd, const S8 *Buff, const U32 Size);
static U32 CrVcpWrite(U32 Fd, const S8 *Buff, const U32 Size);
static U32 CrTermWrite(U32 Fd, const S8 *Buff, const U32 Size);

static U32 CrUartOpen(const S8* Name, U32  access, U32 mode);
static U32 CrFileOpen(const S8* Name, U32  access, U32 mode);
static U32 CrVcpOpen(const S8* Name, U32  access, U32 mode);
static U32 CrTermOpen(const S8* Name, U32  access, U32 mode);

static U32 CrUartClose(U32 Fd);
static U32 CrFileClose(U32 Fd);
static U32 CrVcpClose(U32 Fd);
static U32 CrTermClose(U32 Fd);


U32 CrSetLogAttr(struct CrLogAttr_t* Attr)
{
    if ( !Attr )
        return 0;

    memcpy(&LogAttr, Attr, sizeof(struct CrLogAttr_t));

    return 1;
}

struct CrLogAttr_t* CrGetLogAttr()
{
    return &LogAttr;
}

static U32 CrLogInit(struct CrLogAttr_t * Attr) //是否使用锁，锁只有不同的线程之间才使用
{
    //to do ：打开各个Fd;
    if ( !Attr )
        return -1;

    CrPthreadMutexInit(&MutexLog, NULL);
    BoolUsingMutexFlag = Attr->UsingMutex;

    U32 U32Flag = 0;
    U32Flag = Attr->OutPutDevice & VALUEFLAG;
    S8 Name[MAX_SIZE] = {0};
    U8   TempUartPath[PATH_LENGTH] = {0};
    U8   TempFilePath[PATH_LENGTH] = {0};
    U8   TempVcpPath[PATH_LENGTH] = {0};
    U8   TempTermPath[PATH_LENGTH] = {0};
    S32  SUartFd = 0;
    S32  SFileFd = 0;
    S32  SVcpFd = 0;
    S32  STermFd = 0;
    if ( U32Flag > 0 )
    {
        strcpy((S8 *)TempUartPath, (S8 *)(Attr->UartDevName));
        //chmod(GUartPath, PERMISSION); //给文件夹授权
        DeleteFileOverTheLimit((S8 *)TempUartPath, (S8 *)UartName);
        CatFileName(Name, (S8 *)TempUartPath, (S8 *)UartName);
        SUartFd = CrUartOpen(Name, O_RDWR | O_CREAT, PERMISSION); //在mode中对文件进行授权
        if ( SUartFd < 0 )
        {
            CrUartClose(SUartFd);
            return -1;
        }
    }
    U32Flag = ( Attr->OutPutDevice >> 1) & VALUEFLAG;
    if ( U32Flag > 0 )
    {
        strcpy((S8 *)TempFilePath, (S8 *)(Attr->FileName));
        DeleteFileOverTheLimit((S8 *)TempFilePath, (S8 *)FileName);
        CatFileName(Name, (S8 *)TempFilePath, (S8 *)FileName);
        SFileFd = CrFileOpen(Name, O_RDWR | O_CREAT, PERMISSION);
        //printf("File Name : %s and fd : %d\n", Name, SFileFd);
        if ( SFileFd < 0 )
        {
            CrFileClose(SFileFd);
            return -1;
        }
    }

    U32Flag = ( Attr->OutPutDevice >> 2 ) & VALUEFLAG;
    if ( U32Flag > 0 )
    {
        strcpy((S8 *)TempVcpPath, (S8 *)(Attr->VcpName));
        //chmod(GVcpPath, PERMISSION); //给文件夹授权
        DeleteFileOverTheLimit((S8 *)TempVcpPath, (S8 *)VcpName);
        CatFileName(Name, (S8 *)TempVcpPath, (S8 *)VcpName);
        SVcpFd = CrVcpOpen(Name, O_RDWR | O_CREAT, PERMISSION);
        //printf("Vcp file Name : %s and fd : %d\n", Name, SVcpFd);
        if ( VcpFd < 0 )
        {
            CrVcpClose(SVcpFd);
            CrUartClose(SUartFd);
            CrFileClose(SFileFd);
            return -1;
        }
    }

    U32Flag = ( Attr->OutPutDevice >> 3 ) & VALUEFLAG;
    if ( U32Flag > 0 )
    {
        strcpy((S8 *)TempTermPath, (S8 *)(Attr->TermName));
        //chmod(GTermPath, PERMISSION); //给文件夹授权
        DeleteFileOverTheLimit((S8 *)TempTermPath, (S8 *)TermName);
        CatFileName(Name, (S8 *)TempTermPath, (S8 *)TermName);
        STermFd = CrTermOpen(Name, O_RDWR | O_CREAT, PERMISSION);
        //printf("Uart file Name : %s and fd : %d\n", Name, STermFd);
        if ( TermFd < 0 )
        {
            CrTermClose(STermFd);
            CrUartClose(SUartFd);
            CrFileClose(SFileFd);
            CrVcpClose(SVcpFd);
            return -1;
        }
        //fchmod(TermFd, PERMISSION); //给文件授权(在子线程中就会出特殊的权限)
    }

    if ( SUartFd >= 0 )
    {
        CrUartClose(UartFd);
        UartFd = SUartFd;
        strcpy((S8 *)LogAttr.UartDevName, (S8 *)TempUartPath);
    }

    if ( SFileFd >= 0 )
    {
        CrFileClose(FileFd);
        FileFd = SFileFd;
        strcpy((S8 *)LogAttr.FileName, (S8 *)TempFilePath);
   }

    if ( SVcpFd >= 0 )
    {
        CrVcpClose(VcpFd);
        VcpFd = SVcpFd;
        strcpy((S8 *)LogAttr.VcpName, (S8 *)TempVcpPath);
    }

    if ( STermFd >= 0 )
    {
        CrTermClose(TermFd);
        TermFd = STermFd;
        strcpy((S8 *)LogAttr.TermName, (S8 *)TempTermPath);
    }

    return 0;
}

U32 CrLogIoctl()
{
    if ( LogAttr.OutPutDevice == 0 )
    {
        LogAttr.UsingMutex = 1;
        LogAttr.OutPutDevice = 15;
        strcpy((S8 *)LogAttr.TermName, "./");
        strcpy((S8 *)LogAttr.UartDevName, "./");
        strcpy((S8 *)LogAttr.FileName, "./");
        strcpy((S8 *)LogAttr.VcpName, "./");
    }
    return CrLogInit(&LogAttr);
}

U32 CrLogOpen(S8 *LogPath, S32 Flags)    //Flag,加入使用LOG_TO_FILE
{
    if ( !LogPath || Flags <= 0 )
        return 0;

    CrPthreadMutexInit(&MutexLog, NULL);

    S8 Name[MAX_SIZE] = {0};
    U8   TempUartPath[PATH_LENGTH] = {0};
    U8   TempFilePath[PATH_LENGTH] = {0};
    U8   TempVcpPath[PATH_LENGTH] = {0};
    U8   TempTermPath[PATH_LENGTH] = {0};
    S32  SUartFd = 0;
    S32  SFileFd = 0;
    S32  SVcpFd = 0;
    S32  STermFd = 0;
    
    LogAttr.OutPutDevice = Flags;
    LogAttr.UsingMutex = 1;
    BoolUsingMutexFlag = LogAttr.UsingMutex;
    /* Log To Uart.*/
    if ( Flags & LOG_TO_UART )
    {
        strcpy((S8 *)TempUartPath, (S8 *)LogPath);
        DeleteFileOverTheLimit((S8 *)TempUartPath, (S8 *)UartName);
        CatFileName(Name, (S8 *)TempUartPath, (S8 *)UartName);
        //SUartFd = CrUartOpen(Name, O_RDWR | O_CREAT, S_IRWXG | S_IRWXO | S_IRWXU); //在mode中对文件进行授权
        SUartFd = CrUartOpen(Name, O_RDWR | O_CREAT, PERMISSION); //在mode中对文件进行授权

        if ( SUartFd < 0 )
        {
            CrUartClose(SUartFd);
        }
        else
        {
            close(UartFd);
            UartFd = SUartFd;
            strcpy((S8 *)LogAttr.UartDevName, (S8 *)TempUartPath);
        }
    }
    /* Log To File.*/
    if ( Flags & LOG_TO_FILE )
    {
        strcpy((S8 *)TempFilePath, (S8 *)LogPath);
        //strcat(TempFilePath, "./FileLog/");
        //chmod(GFilePath, PERMISSION); //给文件夹授权
        DeleteFileOverTheLimit((S8 *)TempFilePath, (S8 *)FileName);
        CatFileName(Name, (S8 *)TempFilePath, (S8 *)FileName);
        SFileFd = CrFileOpen(Name, O_RDWR | O_CREAT, PERMISSION);
        if ( SFileFd < 0 )
        {
            CrFileClose(SFileFd);
        }
        else
        {
            close(FileFd);
            FileFd = SFileFd;
            strcpy((S8 *)LogAttr.FileName, (S8 *)TempFilePath);
        }
    }
    /* LOG TO VCP.*/
    if ( Flags & LOG_TO_VCP )
    {
        strcpy((S8 *)TempVcpPath, (S8 *)LogPath);
        DeleteFileOverTheLimit((S8 *)TempVcpPath, (S8 *)VcpName);
        CatFileName(Name, (S8 *)TempVcpPath, (S8 *)VcpName);
        SVcpFd = CrVcpOpen(Name, O_RDWR | O_CREAT, PERMISSION);
        //VcpFd = CrVcpOpen(Name, O_RDWR | O_CREAT, 0);
        //printf("Vcp file Name : %s and fd : %d\n", Name, SVcpFd);
        if ( SVcpFd < 0 )
        {
            CrVcpClose(SVcpFd);
        }
        else
        {
            close(VcpFd);
            VcpFd = SVcpFd;
            strcpy(LogAttr.VcpName, TempVcpPath);
        }
    }
    /* LOG TO TERM.*/
    if ( Flags & LOG_TO_TERM )
    {
        strcpy((S8 *)TempTermPath, (S8 *)LogPath);
        //chmod(GTermPath, PERMISSION); //给文件夹授权
        DeleteFileOverTheLimit((S8 *)TempTermPath, (S8 *)TermName);
        CatFileName(Name, (S8 *)TempTermPath, (S8 *)TermName);
        STermFd = CrTermOpen(Name, O_RDWR | O_CREAT, PERMISSION);
        //TermFd = CrTermOpen(Name, O_RDWR | O_CREAT, 0);
        //printf("Term file Name : %s and fd : %d\n", Name, STermFd);
        if ( STermFd < 0 )
        {
            CrTermClose(STermFd);
        }
        else
        {
            close(TermFd);
            TermFd = STermFd;
            strcpy((S8 *)LogAttr.TermName, (S8 *)TempTermPath);
        }
    }
    return 1;
}

U32 CrLogWrite(S8 *Buff, U32 Size)
{
    //to do 根据Fd的值，大于0的全部写入
    //如果是写文件，注意检查文件大小与文件个数，如果+size超过大小，则更换文件编写，如果超过文件个数，则自动删除最旧一个
    S16 DeleteAndCreateFlag = 0;
    S32 ret = -1;
    if ( UartFd >= 0)
    {
        CrUartWrite(UartFd, Buff, Size);
        if ( IsChangedFile(UartFd, MAXFILELENGTH) )
        {
            if(CrUartClose(UartFd) < 0)
            {
                //printf("**************close failed file : %d ************\n", UartFd);
            }
            printf("Uart log should change log file\n");
            S8 Name[MAX_SIZE] = {0};
            CatFileName(Name, (S8 *)LogAttr.UartDevName, (S8 *)UartName);
            UartFd = CrUartOpen(Name, O_RDWR | O_CREAT, PERMISSION);
            //UartFd = CrUartOpen(Name, O_RDWR | O_CREAT, 0x0666);
            if ( UartFd < 0 )
            {
                //printf("**************************open current : %s  failed, error : %s\n", Name, strerror(errno));
                CrUartClose(UartFd);
            }
            else
            {
                //fchmod(UartFd, PERMISSION);
                //删除文件
                DeleteFileOverTheLimit((S8 *)LogAttr.UartDevName, (S8 *)UartName);
            }
        }
    }

    if ( LogAttr.OutPutDevice & LOG_TO_FILE )
    {
        /* if file is not opened,open file again.*/
        if ( FileFd < 0 )
        {
            U32 Flag = LogAttr.OutPutDevice;
            CrLogOpen((S8 *)LogAttr.FileName, LOG_TO_FILE);
            LogAttr.OutPutDevice = Flag;
        }
        if ( FileFd >= 0 )
        {
            ret = CrFileWrite(FileFd, Buff, Size);
            if ( ret < Size )
            {
                //printf("Log Write failed! ret:%d  Size:%d\n",ret,Size);
            }
            if ( IsChangedFile(FileFd, MAXFILELENGTH) )
            {
                CrFileClose(FileFd);
                //printf("File log should change log file\n");
                S8 Name[MAX_SIZE] = {0};
                CatFileName(Name, (S8 *)LogAttr.FileName, (S8 *)FileName);
                FileFd = CrFileOpen(Name, O_RDWR | O_CREAT, PERMISSION);
                //FileFd = CrFileOpen(Name, O_RDWR | O_CREAT, S_IRWXG | S_IRWXO );
                if ( FileFd < 0 )
                {
                   //printf(">>open file: %s  failed, error : %s\n", Name, strerror(errno));
                   CrFileClose(FileFd);
                }
                else
                {
                   //fchmod(FileFd, PERMISSION);
                   //删除文件
                   DeleteFileOverTheLimit((S8 *)LogAttr.FileName, (S8 *)FileName);
                }
            }
        }
    }
    
    if ( VcpFd >= 0 )
    {
        CrVcpWrite(VcpFd, Buff, Size);
        if ( IsChangedFile(VcpFd, MAXFILELENGTH) )
        {
            CrVcpClose(VcpFd);
            //printf("Vcp log should change log file\n");
            S8 Name[MAX_SIZE] = {0};
            CatFileName(Name, (S8 *)LogAttr.VcpName, (S8 *)VcpName);
            VcpFd = CrVcpOpen(Name, O_RDWR | O_CREAT, PERMISSION);
            //VcpFd = CrVcpOpen(Name, O_RDWR | O_CREAT, 0);
            if ( VcpFd < 0 )
            {
                //printf(">>open Vcp : %s  failed, error : %s\n", Name, strerror(errno));
                CrVcpClose(VcpFd);
            }
            else
            {
                //删除文件
                DeleteFileOverTheLimit((S8 *)LogAttr.VcpName, (S8 *)VcpName);
            }
        }
    }
    
    if ( TermFd  >=0 )
    {
        CrTermWrite(TermFd, Buff, Size);
        if ( IsChangedFile(TermFd, MAXFILELENGTH) )
        {
            CrTermClose(TermFd);
            //printf("Term log should change log file\n");

            S8 Name[MAX_SIZE] = {0};
            CatFileName(Name, (S8 *)LogAttr.TermName, (S8 *)TermName);
            TermFd = CrTermOpen(Name, O_RDWR | O_CREAT, PERMISSION);
            //TermFd = CrTermOpen(Name, O_RDWR | O_CREAT, 0);
            if ( TermFd < 0 )
            {
                //printf(">>open term : %s  failed, error : %s\n", Name, strerror(errno));
                CrTermClose(TermFd);
            }
            else
            {
                //fchmod(TermFd, PERMISSION);
                //删除文件
                DeleteFileOverTheLimit((S8 *)LogAttr.TermName, (S8 *)TermName);
            }
        }
    }

    return Size;
}


U32 CrLogClose()
{
    //to do：close 所有Fd
    CrUartClose(UartFd);
    CrFileClose(FileFd);
    CrVcpClose(VcpFd);
    CrTermClose(TermFd);

    CrPthreadMutexDestroy(&MutexLog);
    
    return 0;
}

U32 CrLogLock()
{
    if ( BoolUsingMutexFlag )
    {
       return CrPthreadMutexLock(&MutexLog);
    }

    return 1;
}


U32 CrLogUnLock()
{
    if ( BoolUsingMutexFlag )
    {
        return CrPthreadMutexUnLock(&MutexLog);
    }

    return 1;
}



U32 CrUartWrite(U32 Fd, const S8 *Buff, const U32 Size)
{
    if ( Fd < 0 )
    {
        return -1;
    }

    U32 U32Len = write(Fd, Buff, Size);

    return U32Len;
}

U32 CrFileWrite(U32 Fd, const S8 *Buff, const U32 Size)
{
    if ( Fd < 0 )
    {
        return -1;
    }
    U32 U32Len = write(Fd, Buff, Size);
    fsync( Fd );
    
    return U32Len;
}


U32 CrVcpWrite(U32 Fd, const S8 *Buff, const U32 Size)
{
    if ( Fd < 0 )
    {
        return -1;
    }

    U32 U32Len = write(Fd, Buff, Size);

    return U32Len;
}

U32 CrTermWrite(U32 Fd, const S8 *Buff, const U32 Size)
{
    if ( Fd < 0 )
    {
        return -1;
    }

    U32 U32Len = write(Fd, Buff, Size);


    return U32Len;
}


U32 CrUartOpen(const S8* Name, U32  access, U32 mode)
{
    if ( !Name )
        return -1;

    U32 U32Fd = open(Name, access, mode);

    return U32Fd;
}

U32 CrFileOpen(const S8* Name, U32  access, U32 mode)
{
    if ( !Name )
        return -1;

    U32 U32Fd = open(Name, access, mode);

    return U32Fd;
}

U32 CrVcpOpen(const S8* Name, U32  access, U32 mode)
{
    if ( !Name )
        return -1;

    U32 U32Fd = open(Name, access, mode);

    return U32Fd;
}


U32 CrTermOpen(const S8* Name, U32  access, U32 mode)
{
    if ( !Name )
        return -1;

    U32 U32Fd = open(Name, access, mode);

    return U32Fd;
}

U32 CrUartClose(U32 Fd)
{
    return close(Fd);
}

U32 CrFileClose(U32 Fd)
{
    return close(Fd);
}

U32 CrVcpClose(U32 Fd)
{
    return close(Fd);
}

U32 CrTermClose(U32 Fd)
{
    return close(Fd);
}



S16 IsChangedFile(U32 Fd, S32 FileMaxSize)
{
    S32 Len = lseek(Fd, 0, SEEK_END);
    S16 Changed = 0;
    if ( Len >= FileMaxSize )
        Changed = 1;

    return Changed;
}


S16 CatFileName(U8* FileName, const U8* Path, const U8* LogName)
{
    if ( !FileName || !Path || !LogName )
        return -1;

    time_t CurTime = time(0);
    struct tm Time;
    localtime_r(&CurTime, &Time);

    S8 Name[100] = {0};
    S8 Buff[40] = {0};
    strcpy(Name, (S8 *)Path);
    rt_tick_t tick = rt_tick_get();
    U32 Hour = tick / 3600;
    U32 Min = (tick % 3600) / 60;
    U32 Sec = (tick % 3600) % 60;
    //sprintf(Buff ,"%s_%04d%02d%02d_%02d%02d%02d.log", LogName, Time.tm_year + 1900, Time.tm_mon, Time.tm_mday, Time.tm_hour, Time.tm_min, Time.tm_sec);
    sprintf(Buff ,"%s_%02d_%02d_%02d.log", LogName, Hour, Min, Sec);
    strcat(Name, Buff);
    strcpy((S8 *)FileName, Name);

    return 0;
}


//统计文件夹下的文件个数
static S16 CountLogFile(const S8* Path, const S8* Filter)
{
    if ( !Path )
        return -1;

    errno = 0;
    DIR *PathDir;
    PathDir = opendir(Path); /* 打开目录 多线程操作，没加锁的状态会导致路径是对的，但是报无效参数的错误*/
    if ( PathDir == NULL )
    {
        closedir(PathDir);
        //printf("CountLogFile ++++ opendir : %s , errNo = %d, error : %s\n", Path, errno, strerror(errno));
        return -1;
    }
    
    errno = 0;
    struct dirent * Ptr;
    S32 Total = 0;
    while ( ( Ptr = readdir(PathDir) ) != NULL )
    {
        //顺序读取每一个目bai录项；
        //跳过“du..”和“.”两个目录
        if ( strcmp(Ptr->d_name,".") == 0 || strcmp(Ptr->d_name,"..") == 0 )
        {
            continue;
        }
        if ( Ptr->d_type == DT_REG )
        {
            if ( !Filter ) //无筛选的内容
            {
                Total++;
                continue;
            }

            if ( strstr(Ptr->d_name, Filter) != NULL ) //有筛选的内容且匹配
            {
                Total++;
            }
        }
    }

    
    closedir(PathDir);

    //printf("count Log : %s , error : %s\n", Path, strerror(errno));

    if ( errno != 0 )
    {
        Total = -1;
    }

    return Total;
}


/********
 fstat(int fd,struct stat *)接收的已open的文件描述符



stat(char *filename,struct stat *)接收的路径名， 需要注意的是 能处理符号链接，但处理的是符号链接指向的文件。


lstat(char *filename,struct stat *)接收的路径名  ，需要注意的是，也能能处理符号链接，但处理的是符号链接本身（自身）文件。 


***********/

// 获取修改时间最早的那个文件ming
static S16 GetMinModificationTimeFileName(S8* Name, const S8* Path, const S8* Filter)
{
    if ( !Name || !Path  )
        return -1;

    DIR *PathDir;
    PathDir = opendir(Path); /* 打开目录*/
    if ( PathDir == NULL )
    {
        closedir(PathDir);
        //printf("opendir : %s , errNo = %d , error : %s\n", Path, errno, strerror(errno));
        return -1;
    }

    U64 MinTime = 0;
    S8 MinPath[MAX_SIZE] = {0};
    struct dirent * Ptr;

    errno = 0;

    while ( ( Ptr = readdir(PathDir) ) != NULL )
    {
        //顺序读取每一个目bai录项；
        //跳过“du..”和“.”两个目录
        if ( strcmp(Ptr->d_name,".") == 0 || strcmp(Ptr->d_name,"..") == 0 )
        {
            continue;
        }

        if ( Ptr->d_type == DT_REG )
        {
            if ( Filter ) //有筛选的内容
            {
                if( strstr(Ptr->d_name, Filter) == NULL ) //不匹配
                    continue;
            }

            //获取文件的时间
            S8 FilePath[100] = {0};
            strcpy(FilePath, Path);
            strcat(FilePath, Ptr->d_name);
            Stat_t FileStat;
            if( stat(FilePath, &FileStat) < 0 )
            {
                continue;
            }

            if ( MinTime == 0 )
            {
                MinTime = FileStat.st_mtime;
                strcpy(MinPath, FilePath);
            }
            else
            {
                if( MinTime > FileStat.st_mtime )
                {
                    MinTime = FileStat.st_mtime;
                    strcpy(MinPath, FilePath);
                }
            }

        }
    }

    //printf("get file name : %s , error : %s\n", Path, strerror(errno));
    closedir(PathDir);
    
    if ( errno != 0 )
    {
        return -1;
    }
    
    strcpy(Name, MinPath);

    return 0;
}

// 删除文件夹下超过文件个数限制的文件
static VOID DeleteFileOverTheLimit(const S8* Path, const S8* Filter)
{
    if ( !Path )
        return;
    int Count = CountLogFile(Path, Filter);

    if ( Count >= MAXFILECOUNT )
    {
        int i = Count;
        for ( i = Count; i > MAXFILECOUNT; i-- )
        {
            S8 Name[200] = {0};
            if ( GetMinModificationTimeFileName(Name, Path, Filter) >= 0 )
            {
                remove(Name);
            }
        }
    }
    return;
}










