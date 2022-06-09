#include "CrSdCardDriver.h"
#include <stm32f4xx_hal.h>
#include <rtdef.h>
#include <dfs_fs.h>
#include <dfs_posix.h>
#include "CrGpio/CrGpio.h"
#include <drv_sdio.h>
#include <dfs_elm.h>

static S32 SdCmdGetSdState(S32 Fd, VOID *Arg)
{
    return CrGpioIoctl(Fd, GPIO_GET_PINSTATE, Arg);
}

static S32 SdCmdMountSD(VOID *Arg)
{
    if ( !Arg )
        return -1;

    S32 Result = -1;

    struct SdCartMountParam_t *Info = (struct SdCartMountParam_t *)Arg;
    static S8 InitFlag = 0;

    if ( InitFlag != 0 )
    {
#ifdef BSP_USING_SDIO
        stm32_mmcsd_change(); //实现热插拔
#endif
        rt_thread_delay(500);
    }
    else
        rt_thread_delay(100);

    rt_device_t Dev;
    Dev = rt_device_find(Info->SdName);
    if ( Dev )
    {
        InitFlag = 1;
        Result = dfs_mount(Info->SdName, Info->SdRootDir, Info->FileSystemType, 0, 0);

        //if ( Result != 0 )
            //printf("current errno is %d, err string : %s\n", rt_get_errno(), strerror(rt_get_errno()));
    }
    else
    {
        //printf("Do not Found Devices %s\n", Info->SdName);

        if ( 0 == InitFlag )
        {
#ifdef BSP_USING_SDIO
            stm32_mmcsd_change();
#endif
        }
    }

    rt_thread_delay(50);

    return Result;
}

static S32 SdCmdUnMountSD(VOID *Arg)
{
    if ( !Arg )
        return -1;

    S32 Ret = -1;

    struct SdCardUnmountParam_t *Info = (struct SdCardUnmountParam_t *)Arg;

    rt_device_t Dev;
    Dev = rt_device_find("sd0");
    if ( Dev )
    {
        Ret = dfs_unmount(Info->SfRootDir);
        if ( !Ret )
        {
#ifdef BSP_USING_SDIO
            stm32_mmcsd_change();
#endif
            rt_thread_delay(50);
        }
    }

    return Ret;
}

/*该函数根据各自主板实际情况编写  名称不可更改, 有board文件直接调用，不能人为调用此接口, 注意引脚可能有变化*/
#if 1
void HAL_SD_MspInit(SD_HandleTypeDef* Hsd)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if ( Hsd->Instance == SDIO )
    {
        __HAL_RCC_SDIO_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        /**SDIO GPIO Configuration
        PC8     ------> SDIO_D0
        PC9     ------> SDIO_D1
        PC10     ------> SDIO_D2
        PC11     ------> SDIO_D3
        PC12     ------> SDIO_CK
        PD2     ------> SDIO_CMD
        */
        GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                       |GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    }
}
#endif

S32 SdCardDriverOpen(S8 *Name, S32 Flag, S32 Mode)  //注意此处穿的是引脚的名称，参考CrGpio.h  SD卡检测脚
{
    struct PinInitParam_t PinInit = {0};

    PinInit.Mode = GPIO_MODE_INPUT;
    PinInit.Pull = GPIO_NOPULL;
    PinInit.Speed = GPIO_SPEED_FREQ_HIGH;

    S32 Fd = CrGpioOpen(Name, Flag, Mode);
    CrGpioIoctl(Fd, GPIO_INIT, (VOID *)&PinInit);

    return Fd;
}

VOID SdCardDriverClose(S32 Fd)
{
    CrGpioClose(Fd);
}

S32 SdCardDriverIoctl(S32 Fd, U32 Cmd, VOID *Arg)        //注意要将SD卡挂载了之后才能进行路径的操作
{
    S32 Ret = -1;
    if ( Fd < 0 || !Arg )
        return Ret;

    switch ( Cmd )
    {
        case CMD_SD_CARD_GET_STATUE:
        {
            Ret = SdCmdGetSdState(Fd, Arg);
            break;
        }
        case CMD_SD_CARD_MOUNT:
        {
            Ret = SdCmdMountSD(Arg);
            break;
        }
        case CMD_SD_CARD_UNMOUNT:
        {
            Ret = SdCmdUnMountSD(Arg);
            break;
        }
        default :
            break;
    }

    return Ret;
}
