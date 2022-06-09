#include "CrSerial.h"
#include <rtthread.h>
#include <map>

#include "CrInc/CrType.h"


/*
 * RT Thread 自有的接口
 * rt_device_find()         查找设备
 * rt_device_open()         打开设备
 * rt_device_read()         读取数据
 * rt_device_write()        写入数据
 * rt_device_control()      控制设备
 * rt_device_set_rx_indicate()  设置接收回调函数
 * rt_device_set_tx_complete()  设置发送完成回调函数
 * rt_device_close()            关闭设备
 * */
struct CrSerial_t
{
    S8              *Name;
    rt_device_t     SerialDevice;
    S32             Fd;
    S8              IsUsed;
};

static CrSerial_t CrSerial[10] = {0};  //维护一个结构体数组

/*
  * 打开串口函数
  * 通过串口号，对串口进行初始化
  * 输入：
  * SerialName  串口号
  * Flag:       设备模式标志
  * Mode:       暂时未使用
  * 输出：
  * 0及以上：        Fd
  * -1：                 串口号为空
  * -2：                 未找到串口设备
  * -3:         设备不允许重复打开
  * -4：                 打开失败
 */

S32 CrSerialOpen(S8 *SerialName, S32 Flag, S32 Mode)
{
    if( SerialName == NULL )
        return -1;

    rt_device_t Serial = rt_device_find(SerialName);
    if ( Serial == RT_NULL )    //没有找到串口设备
        return -2;

    S32 Result = rt_device_open(Serial,Flag);
    if ( Result == RT_EOK ) //打开成功添加到当前串口设备数组中
    {
        for( int i = 0; i < 10 ; i++ )
        {
            if ( CrSerial[i].IsUsed == false )  //未被使用
            {
                CrSerial[i].Name = SerialName;
                CrSerial[i].SerialDevice = Serial;
                CrSerial[i].Fd = i;
                CrSerial[i].IsUsed = true;

                return CrSerial[i].Fd;
            }
        }
    }
    if ( Result == -RT_EBUSY )  //驱动不允许重复打开
    {
        return -3;
    }
    return -4;              //打开失败
}

/*
  * 关闭串口函数
  * 通过串口号，对串口进行初始化
  * 输入：
  * Fd          设备句柄
  * 输出：
  * 0：                   关闭成功
  * -1：                 Fd异常
 */
S32 CrSerialClose(S32 Fd)
{
    if( Fd > 10 || Fd < 0) //超出正常范围
    {
        return  -1;
    }

    if ( CrSerial[Fd].IsUsed == true )  //说明已经存储了数据
    {
        S32 Reslut = rt_device_close(CrSerial[Fd].SerialDevice);
        if ( Reslut == RT_EOK )
        {
            CrSerial[Fd].Fd = -1;
            CrSerial[Fd].IsUsed = false;
            CrSerial[Fd].Name = NULL;
            CrSerial[Fd].SerialDevice = NULL;
            return 0;
        }
    }
    else
    {
        return -1;
    }

}
/*
 * 读取串口数据到缓冲区
 * Fd:句柄
 * Buff:缓冲区数组指针
 * Size:读取的数据数量
 * 返回：读取成功则返回读取的个数，失败返回0
 */
U32 CrSerialRead(S32 Fd, VOID *Buff, S32 Size)
{
    S32 Len = 0;
    Len = rt_device_read( CrSerial[Fd].SerialDevice, 0,Buff,Size);
    return Len;
}

/*
 * 从缓冲区写入串口数据
 * Fd:句柄
 * Buff:缓冲区数组指针
 * Size:写入的数据数量
 * 返回：写入成功则返回读取的个数，失败返回0
 */
U32 CrSerialWrite(S32 Fd, VOID *Buff, S32 Size)
{
    S32 Len = 0;
    Len = rt_device_write( CrSerial[Fd].SerialDevice,0,Buff,Size);
    return Len;
}

/*
 * 对串口设备重新配置参数
 * 输入：串口名、串口配置的结构体
 * 输出：成功输出1，失败输出0
 */
 S32 CrSerialIoctl(S32 Fd, S32 Cmd, VOID *Param)
{
    S32 Reslut = 1;
    switch ( Cmd )
    {
        case GET_SERIAL_ATTR:
        {
           struct rt_serial_device *Serial;
           struct serial_configure *Temp = (struct serial_configure *)Param;
           Serial = (struct rt_serial_device *)CrSerial[Fd].SerialDevice;
           *Temp = (Serial->config);
           Reslut = 0;
           break;
        }
        case SET_SERIAL_ATTR:
        {
            Reslut = rt_device_control(CrSerial[Fd].SerialDevice, Cmd, Param); //RT_DEVICE_CTRL_CONFIG为修改串口参数命令符
            break;
        }
        case SERIAL_AVAILABLE:
        {
            struct rt_serial_device *Serial;
            Serial = (struct rt_serial_device *)CrSerial[Fd].SerialDevice;
            RT_ASSERT(Serial);
            rt_serial_rx_fifo *rx_fifo = (struct rt_serial_rx_fifo*)Serial->serial_rx;
            Reslut = rx_fifo->get_index == rx_fifo->put_index ? 1 : 0;
            break;
        }
        case SERIAL_RX_FLASH:
        {
            struct rt_serial_device *Serial;
            Serial = (struct rt_serial_device *)CrSerial[Fd].SerialDevice;
            RT_ASSERT(Serial);
            rt_serial_rx_fifo *rx_fifo = (struct rt_serial_rx_fifo*)Serial->serial_rx;
            RT_ASSERT(rx_fifo);
            rx_fifo->put_index = rx_fifo->get_index;
            break;
        }
        case SERIAL_TX_FLASH:
        {
            struct rt_serial_device *Serial;
            Serial = (struct rt_serial_device *)CrSerial[Fd].SerialDevice;
            RT_ASSERT(Serial);
            rt_serial_tx_fifo *tx_fifo = (struct rt_serial_tx_fifo*)Serial->serial_tx;
            RT_ASSERT(tx_fifo);
            tx_fifo->completion.suspended_list.next = (rt_list_t *)&(tx_fifo->completion.suspended_list);
            tx_fifo->completion.suspended_list.prev = (rt_list_t *)&(tx_fifo->completion.suspended_list);
            break;
        }
        case GET_SERIAL_NAME:
        {
            RT_ASSERT(Param);
            rt_strncpy((char *)Param, (char *)CrSerial[Fd].Name, rt_strlen(CrSerial[Fd].Name));
            Reslut = RT_EOK;
            break;
        }
        case SET_SERIAL_NAME:
        {
            RT_ASSERT(CrSerial[Fd].Name);
            rt_strncpy((char *)CrSerial[Fd].Name, (char *)Param, rt_strlen((char *)Param));
            Reslut = RT_EOK;
            break;
        }
        default:
            break;
    }

    if ( Reslut == RT_EOK )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
