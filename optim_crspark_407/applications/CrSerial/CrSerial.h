#include <rtdef.h>
#include "CrInc/CrType.h"
#include <rtdevice.h>

#ifdef __cplusplus
extern "C" {
#endif

/********串口参数宏定义表***********/

/* 波特率可取值 */
#define BAUD_RATE_2400                  2400
#define BAUD_RATE_4800                  4800
#define BAUD_RATE_9600                  9600
#define BAUD_RATE_19200                 19200
#define BAUD_RATE_38400                 38400
#define BAUD_RATE_57600                 57600
#define BAUD_RATE_115200                115200
#define BAUD_RATE_230400                230400
#define BAUD_RATE_460800                460800
#define BAUD_RATE_921600                921600
#define BAUD_RATE_2000000               2000000
#define BAUD_RATE_3000000               3000000
/* 数据位可取值 */
#define DATA_BITS_5                     5   //有效数据的位数为5位
#define DATA_BITS_6                     6   //有效数据的位数为6位
#define DATA_BITS_7                     7   //有效数据的位数为7位
#define DATA_BITS_8                     8   //有效数据的位数为8位(通常设置有效数据位为8位)
#define DATA_BITS_9                     9   //有效数据的位数为9位
/* 停止位可取值 */
#define STOP_BITS_1                     0   //单个数据包的最后一位为0，表示传输结束。
#define STOP_BITS_2                     1   //单个数据包的最后一位为1，表示传输结束。(通常设置为一个停止位)
#define STOP_BITS_3                     2   //单个数据包的最后一位为2，表示传输结束。
#define STOP_BITS_4                     3   //单个数据包的最后一位为3，表示传输结束。
/* 校验位可取值 */
#define PARITY_NONE                     0   //无校验位
#define PARITY_ODD                      1   //奇校验
#define PARITY_EVEN                     2   //偶校验
/* 高低位顺序可取值 */
#define BIT_ORDER_LSB                   0   //发送顺序是低位先发(通常设置为低位先发)
#define BIT_ORDER_MSB                   1   //发送顺序是高位先发
/* 模式可取值 */
#define NRZ_NORMAL                      0   /* 正常模式：传输后电平归零 (通常设置为正常模式)*/
#define NRZ_INVERTED                    1   /* 不归零模式：传输后电平不归零 */
/* 接收数据缓冲区默认大小 */
#if defined(RT_SERIAL_RB_BUFSZ)
#undef  RT_SERIAL_RB_BUFSZ
#define RT_SERIAL_RB_BUFSZ              128 //数据缓冲区大小
#endif
/********串口参数宏定义表***********/

/********打开方式**********/
#define RT_DEVICE_FLAG_STREAM       0x040     /* 流模式      */
/* 接收模式参数 */
#define RT_DEVICE_FLAG_INT_RX       0x100     /* 中断接收模式 */
#define RT_DEVICE_FLAG_DMA_RX       0x200     /* DMA 接收模式 */
/* 发送模式参数 */
#define RT_DEVICE_FLAG_INT_TX       0x400     /* 中断发送模式 */
#define RT_DEVICE_FLAG_DMA_TX       0x800     /* DMA 发送模式 */
/********打开方式**********/


/*****Serial Command*****/

#define GET_SERIAL_ATTR     0x01            //获取串口配置参数
#define SET_SERIAL_ATTR     0x02            //设置串口配置参数
#define SERIAL_AVAILABLE    0x03            //缓冲区是否可读
#define SERIAL_RX_FLASH     0x04            //刷新串口读缓冲区
#define SERIAL_TX_FLASH     0x05            //刷新串口写缓冲区
#define GET_SERIAL_NAME     0x06            //获取串口名字
#define SET_SERIAL_NAME     0x07            //设置串口名字

/*****Serial Command*****/

struct CrSerialConfig_t
{
    U32 BaudRate;               //波特率
    U32 DataBits    :4;         //数据位
    U32 StopBits    :2;         //停止位
    U32 Parity      :2;         //奇偶校验位
    U32 BitOrder    :1;         //高位在前或者低位在前
    U32 Invert      :1;         //模式
    U32 BufSize     :16;        //接收数据缓冲区大小
    U32 Reserved    :6;         //保留位
};


//rt_device_t CrSerialFind(char* name);
//S32 CrSerialInit(rt_device_t serial);

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
S32 CrSerialOpen(S8 *SerialName, S32 Flag, S32 Mode);

/*
  * 关闭串口函数
  * 通过串口号，对串口进行初始化
  * 输入：
  * Fd          设备句柄
  * 输出：
  * 0：                   关闭成功
  * -1：                 Fd异常
 */
S32 CrSerialClose(S32 Fd);

/*
 * 读取串口数据到缓冲区
 * Fd:句柄
 * Buff:缓冲区数组指针
 * Size:读取的数据数量
 * 返回：读取成功则返回读取的个数，失败返回0
 */
U32 CrSerialRead(S32 Fd, VOID *Buff, S32 Size);

/*
 * 从缓冲区写入串口数据
 * Fd:句柄
 * Buff:缓冲区数组指针
 * Size:写入的数据数量
 * 返回：写入成功则返回写入的个数，失败返回0
 */
U32 CrSerialWrite(S32 Fd, VOID *Buff, S32 Size);

/*
 * 对串口设备重新配置参数
 * 输入：串口名、串口配置的结构体
 * 输出：成功输出1，失败输出0
 */
S32 CrSerialIoctl(S32 Fd, S32 Cmd, VOID *Param);

#ifdef __cplusplus
}
#endif
