#include "CrSdCardAppInterface.h"
#include "Common/Include/CrDirentAndFile.h"
#include "dfs.h"
#include "dfs_fs.h"
#include "dfs_elm.h"
#include "dfs_posix.h"

static S32 FilePointerOffsetIndex(DIR *Dirp, S32 Page, S32 MaxCount, struct FileInfo_t *FileList)
{
    if ( Page < 0 || MaxCount <= 0 || !Dirp || !FileList )
        return -1;

    struct dirent *DirHand = RT_NULL;
    S32 Cnt = 0;
    S32 Var = 0;
    S32 FilesNum = 0;

    while( (DirHand = CrReadDir(Dirp)) != RT_NULL ) //目录最后一个
    {
        if ( (++Cnt >= Page * MaxCount) && (Var < MaxCount) )
        {
            memcpy((FileList[Var].Name), DirHand->d_name, sizeof(FileList[Var].Name));
            FileList[Var++].IsDirectory = (DirHand->d_type == 2) ? 1 : 0;
        }
    }

    return (FilesNum + Cnt);
}

static S32 ReturnFIle(CrDir_t *Dirp, S32 Page, S32 MaxCount, struct FileInfo_t *FileList, S8 *Filter)
{
    S32 Ret = -1;
    if ( Page < 0 || MaxCount <= 0 )
        return Ret;

    S32 FilesNum = 1;
    S32 Var = 0;

    if ( Page == 0 )
    {
        struct FileInfo_t Temp = {"..", 1};
        memcpy(&(FileList[Var++]), &Temp, sizeof(struct FileInfo_t));
    }

    if ( Filter )
    {
        struct dirent *DirHand = RT_NULL;
        S32 Cnt = 0;

        while( (DirHand = CrReadDir(Dirp)) != RT_NULL ) //目录最后一个
        {
            //判断文件长度是否大于比较的字符串长度
            if ( (DirHand->d_namlen >= strlen(Filter) && (1 == DirHand->d_type)) || (2 == DirHand->d_type) )
            {
                //判断找到的文件中后缀是否与比较字符串相等
                if ( (strcmp(DirHand->d_name + DirHand->d_namlen - strlen(Filter), Filter ) == 0) \
                        || (2 == DirHand->d_type))
                {
                    FilesNum++;
                    if (  (Var == MaxCount) )
                        break;

                    if ( (++Cnt >= (Page * MaxCount)))
                    {
                        memcpy((FileList[Var].Name), DirHand->d_name, sizeof(FileList[Var].Name));
                        FileList[Var++].IsDirectory = (DirHand->d_type == 2) ? 1 : 0;
                    }
                }
            }
        }
    }
    else
    {
        FilesNum += FilePointerOffsetIndex(Dirp, Page, MaxCount, FileList);
    }

    return FilesNum;
}

//返回经过筛选后的当前文件夹下的文件总个数
S32 CrSdCardGetFileNameToBeShow(S32 Page, struct FileInfo_t  *FileList, S32 MaxCount, S8 *Filter)
{
    S32 Ret = -1;
    
    if ( !FileList )
        return Ret;

    S8 CurrentPath[DFS_PATH_MAX] = {0};

    if ( !getcwd(CurrentPath, sizeof(CurrentPath)) )          //获取当前工作路径
        return Ret;

    CrDir_t *Dirp = RT_NULL;
    if ( !(Dirp = CrOpenDir((const S8 *)CurrentPath)) )
        return Ret;

    Ret = ReturnFIle(Dirp, Page, MaxCount, FileList, Filter);
    CrCloseDir(Dirp);

    return Ret;
}

static S8 *ReturnLongNameFIle(CrDir_t *Dirp, S32 Index, S8 *Filter)
{
    if ( Index < 0 || !Dirp  )
        return NULL;

    S32 FilesNum = 0;

    if ( Filter )
    {
        struct dirent *DirHand = RT_NULL;
        while( (DirHand = CrReadDir(Dirp)) != RT_NULL ) //目录最后一个
        {
            if ( (DirHand->d_namlen >= strlen(Filter) && (1 == DirHand->d_type)) || (2 == DirHand->d_type) )
            {
                if ( (strcmp(DirHand->d_name + DirHand->d_namlen - strlen(Filter), Filter ) == 0) \
                        || (2 == DirHand->d_type))
                {
                    if ( ++FilesNum == Index )
                        return DirHand->d_name;
                }
            }
        }
    }
    else
    {
//        FilesNum += FilePointerOffsetIndex(Dirp, Page, MaxCount, FileList);
    }

    return NULL;
}


/* Modify by Charley : Special handling of long name files */
S8 *CrSdCardGetFileForIndex(S32 Index, S8 *Path, S8 *Filter)
{
    if ( Index < 0 || !Path )
        return NULL;

    CrDir_t *Dirp = RT_NULL;
    if ( !(Dirp = CrOpenDir((const S8 *)Path)) )
        return NULL;

    return ReturnLongNameFIle(Dirp, Index, Filter);
}


VOID CrSdCardGetFileNameToBeShowTest()
{
    struct FileInfo_t FileList[20] = {0};
    CrSdCardGetFileNameToBeShow(0, FileList, 10, ".gcode");

    S32 Var = 0;
    while( Var < 20 )
    {
        printf("FileList[%d]: %s\n", Var, FileList[Var].Name);
        Var++;
    }
}

MSH_CMD_EXPORT(CrSdCardGetFileNameToBeShowTest, Test File)
