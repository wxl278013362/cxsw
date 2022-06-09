#include "../Include/CrDirentAndFile.h"


S32 CrCloseDir(CrDir_t *Dirp)
{
    return closedir(Dirp);
}

CrDir_t *CrOpenDir(const S8 *Name)
{
    return opendir(Name);
}

CrDirent_t *CrReadDir(CrDir_t *Dirp)
{
    return readdir(Dirp);
}

VOID CrRewindDir(CrDir_t *Dirp)
{
    return rewinddir(Dirp);
}

VOID CrSeekDir(CrDir_t *Dirp, U64 Offset)
{
    return seekdir(Dirp, Offset);
}

U64 TellDir(CrDir_t *Dirp)
{
    return telldir(Dirp);
}

S32 CrMkDir(const S8 *Path, CrMode_t Mode)
{
    return mkdir(Path, Mode);
}

S32 CrRmDir(const S8 *Path)
{
    return rmdir(Path);
}

S32 CrChDir(const S8 *Path)
{
    return chdir(Path);
}

S8 *CrGetCwd(S8 *Buf, S32 Size)
{
    return getcwd(Buf, Size);
}

S32 CrUnlink(const S8 *PathName)
{
    return unlink(PathName);
}

//文件操作
S32 CrOpen(const S8 *FileName, S32 Flags, S32 Mode)
{
    return open(FileName, Flags, Mode);
}

S32 CrClose(S32 Fd)
{
    return close(Fd);
}

S32 CrRead(S32 Fd, VOID *Buf, S32 Len)
{
    return read(Fd, Buf, Len);
}

S32 CrWrite(S32 Fd, const VOID *Buf, S32 Len)
{
    return write(Fd, Buf, Len);
}

S32 CrFsync(S32 Fildes)
{
    return fsync(Fildes);
}

S32 CrFcntl(S32 Fildes, S32 Cmd, VOID *Arg)
{
    return fcntl(Fildes, Cmd, Arg);
}

S32 CrIoctl(S32 Fildes, S32 Cmd, VOID *Arg)
{
    return ioctl(Fildes, Cmd, Arg);
}

CrOff_t CrLseek(S32 Fd, CrOff_t Offset, S32 Whence)
{
    return lseek(Fd, Offset, Whence);
}

S32 CrStatfs(const S8 *Path, CrStatfs_t *Buf)
{
    return statfs(Path, Buf);
}

S32 CrAccess(const S8 *PathName, S32 Mode)
{
    return access(PathName, Mode);
}

S32 CrPipe(S32 Fildes[2])
{
    return pipe(Fildes);
}

S32 CrMkFifo(const S8 *Path, CrMode_t Mode)
{
    return mkfifo(Path, Mode);
}

S32 CrRename(const S8 *From, const S8 *To)
{
    return rename(From, To);
}

S32 CrFtruncate(S32 Fd, CrOff_t Length)
{
    return ftruncate(Fd, Length);
}

S32 CrStat(const S8 *File, CrStat_t *Buf)
{
    return stat(File, Buf);
}

S32 CrFstat(S32 Fildes, CrStat_t *Buf)
{
    return fstat(Fildes, Buf);
}
