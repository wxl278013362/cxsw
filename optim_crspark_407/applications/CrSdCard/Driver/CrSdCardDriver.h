#ifndef _CRSDCARDDRIVER_H
#define _CRSDCARDDRIVER_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef USING_OS_RTTHREAD

//SD的状态引脚
//#define SD_DETECT_PIN       "PC7"
#define SD_DETECT_PIN       "PD3"  //407芯片

//SD卡相关的参数
#define SD_CARD_NAME                "sd0"
#define SD_CARD_NAME_SIZE           (sizeof(SD_CARD_NAME))
#define SD_CARD_ROOT_DIR            "/"
#define SD_CARD_ROOT_DIR_SIZE       (sizeof(SD_CARD_ROOT_DIR))
#define SD_CARD_FILE_SYSTEM_TYPE    "elm"
#define SD_CARD_FILE_SYSTEM_SIZE    (sizeof(SD_CARD_FILE_SYSTEM_TYPE))

#endif

//Sd操作命令
#define CMD_SD_CARD_GET_STATUE  0x01        //获取SD卡状态
#define CMD_SD_CARD_MOUNT       0x02        //挂载
#define CMD_SD_CARD_UNMOUNT     0x04        //卸载


#ifdef __cplusplus
extern "C" {
#endif

//Sd卡挂载参数
struct SdCartMountParam_t
{
    S8      SdName[SD_CARD_NAME_SIZE];             //SD卡名称(驱动中的名称)
    S8      SdRootDir[SD_CARD_ROOT_DIR_SIZE];           //SD卡的根路径
    S8      FileSystemType[SD_CARD_FILE_SYSTEM_SIZE];     //SD卡的文件系统名称
};

//Sd卡卸载参数
struct SdCardUnmountParam_t
{
    S8      SfRootDir[SD_CARD_ROOT_DIR_SIZE];           //SD卡的根路径
};

S32 SdCardDriverOpen(S8 *Name, S32 Flag, S32 Mode);  //注意此处穿的是引脚的名称，参考CrGpio.h SD卡检测脚
VOID SdCardDriverClose(S32 Fd);

//S32 SdCardDriverWrite(S32 Fd, S8 *Data, S32 DataLen);     //此接口暂时不实现
//S32 SdCardDriverRead(S32 Fd, S8 *Data, S32 DataLen);      //此接口暂时不实现
S32 SdCardDriverIoctl(S32 Fd, U32 Cmd, VOID *Arg);        //注意要将SD卡挂载了之后才能进行路径的操作

#ifdef TEST_SD_CARD_DRIVER
VOID TestSdCardDriver();
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif
