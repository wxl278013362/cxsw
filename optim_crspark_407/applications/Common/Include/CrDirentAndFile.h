#ifndef _CRDIRENT_H
#define _CRDIRENT_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USING_OS_LINUX
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/vfs.h>
#include <stdio.h>

typedef DIR  CrDir_t;
typedef struct dirent CrDirent_t;

#endif

#ifdef USING_OS_RTTHREAD
#include <dfs_posix.h>
typedef DIR  CrDir_t;
typedef struct dirent CrDirent_t;
typedef off_t CrOff_t;
typedef mode_t CrMode_t;
typedef struct stat CrStat_t;
typedef struct statfs CrStatfs_t;
#endif

//目录操作
S32 CrCloseDir(CrDir_t *Dirp);//
CrDir_t *CrOpenDir(const S8 *Name);//
CrDirent_t *CrReadDir(CrDir_t *Dirp);//
//S32 ReadDir_R(CrDir_t *Dirp, CrDirent_t *Entry, CrDirent_t **Result);
VOID CrRewindDir(CrDir_t *Dirp);//
VOID CrSeekDir(CrDir_t *Dirp, U64 Offset);//
U64 CrTellDir(CrDir_t *Dirp);//

S32 CrMkDir(const S8 *Path, CrMode_t Mode);//
S32 CrRmDir(const S8 *Path); //
S32 CrChDir(const S8 *Path);//
S8 *CrGetCwd(S8 *Buf, S32 Size);//

S32 CrUnlink(const S8 *PathName);   // 删除目录项

//文件操作
S32 CrOpen(const S8 *FileName, S32 Flags, S32 Mode);//
S32 CrClose(S32 Fd);//
S32 CrRead(S32 Fd, VOID *Buf, S32 Len);//
S32 CrWrite(S32 Fd, const VOID *Buf, S32 Len);//
S32 CrFsync(S32 Fildes);//
S32 CrFcntl(S32 Fildes, S32 Cmd, VOID *Arg);//
S32 CrIoctl(S32 Fildes, S32 Cmd, VOID *Arg);//
CrOff_t CrLseek(S32 Fd, CrOff_t Offset, S32 Whence);  //
S32 CrStatfs(const S8 *Path, CrStatfs_t *Buf);//

S32 CrAccess(const S8 *PathName, S32 Mode);//
S32 CrPipe(S32 Fildes[2]);//
S32 CrMkFifo(const S8 *Path, CrMode_t Mode);//

S32 CrRename(const S8 *From, const S8 *To);//

S32 CrFtruncate(S32 Fd, CrOff_t Length);//
S32 CrStat(const S8 *File, CrStat_t *Buf);//
S32 CrFstat(S32 Fildes, CrStat_t *Buf);//

#ifdef __cplusplus
}
#endif

#endif
