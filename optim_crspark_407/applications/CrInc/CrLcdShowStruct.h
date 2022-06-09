#ifndef _CRLCDSHOWSTRYCT_H
#define _CRLCDSHOWSTRYCT_H

//Lcm12864屏幕的操作命令参数
struct LcdCtrlParam_t
{
    S32     X;
    S32     Y;
    S32     Width;
    S32     Height;
};

/*****************************Lcm12864Show的操作命令(底层硬件)*****************************************/
#define     LCD_USING_ASCII_CHARATER_SET    0x01    //设置使用ascii字符集
#define     LCD_USING_UTF8_CHARATER_SET     0x02    //使用UTF8字符集
#define     LCD_WRITE_HOR_LINE              0x04    //画横线
#define     LCD_WRITE_VER_LINE              0x08    //画竖线
#define     LCD_WRITE_RECTANGLE             0x10    //画矩形
#define     LCD_WRITE_MAP                   0x20    //画图像(同时设置写数据的位置和图片的宽高）
#define     LCD_WRITE_STRING                0x40    //写字符串(同时设置写数据的位置）
#define     LCD_CLEAN                       0x80    //清屏幕
#define     LCD_FLUSH                       0x0100  //将LCD缓冲区的内容刷新到屏幕上
#define     LCD_SET_SPI                     0x0200  //设置Spi对象, 配合完成初始化
#define     LCD_GET_FONT_WIGTH_OFCURRENT    0x2000  //获取字体宽度，注意别和其他命令的值冲突
/******************************Lcm12864Show的操作命令(底层硬件)****************************************/

//12864驱动移动参数
struct CoordinateParam_t
{
    S32     X_Pos;
    S32     Y_Pos;
};

/************************12864驱动操作命令********************************/
#define     LCD_MOVE_ELEMENT_ROW_AND_COLUMN       0x0400            //移动字符的行和列
#define     LCD_MOVE_PIXEL_ROW_AND_COLUMN         0x0800            //移动像素的行和列

/************************12864驱动操作命令********************************/


#define MAX_NAME_SIZE 7

//SPI的引脚的信息
struct SpiPinInit_t
{
      S8 GpioPinName[MAX_NAME_SIZE];      //GPIOname   参考CrGpio.h
      S8  PinState;                       //电平状态:0 对应的是 GPIO_PIN_RESET = 0u, 1对应的是 GPIO_PIN_SET
      U32 Mode;                           //引脚模式
      U32 Pull;                           //上拉接VCC  下拉接GND
      U32 Speed;                          //引脚速度
};

//SPI初始化参数
struct  SpiInitParam_t
{
    U32     Mode;                       //工作模式  ： 作为从机 还是主机
    U32     Duplex;                     //全双工or半双工  Duplex SPI_DIRECTION_2LINES, SPI_DIRECTION_2LINES_RXONLY, SPI_DIRECTION_1LINE
    U32     DataSize;                   //数据大小
    U32     CLKPolarity;                //时钟极性：SPI_POLARITY_LOW表示SPI总线在空闲时SCK为低电平，SPI_POLARITY_HIGH表示SPI总线在空闲时SCK为高电平
    U32     CLKPhase;                   //时钟相位：SPI_PHASE_1EDGE表示在第一个时钟延采样，SPI_PHASE_2EDGE表示在第二个时钟延采样
    U32     NSS;                        //SPI_NSS_SOFT 表示使用软件片选（随便一个IO口都可以） SPI_NSS_HARD_OUTPUT 或 SPI_NSS_HARD_INTPUT 表示使用硬件片选，引脚必须为SPI_NSS
    U32     BaudRatePrescaler;          //分频系数，控制SPI总线的速度
    U32     Endian;                     //SPI_FIRSTBIT_MSB表示数据高位先传，SPI_FIRSTBIT_LSB 表示数据低位先传
    U32     TIMode;                     //
    U32     CRCCalculation;             //是否使用校验  enable or disable
    U32     CRCPolynomial;              //校验位  (校验寄存器中的校验函数指针)

    struct SpiPinInit_t SpiSck;         //SPI时钟线
    struct SpiPinInit_t CsPin;          //SPI片选
    struct SpiPinInit_t MosiPin;        //SPI主机输出从机输入信号线
    struct SpiPinInit_t MisoPin;        //SPI从机输出主机输入信号线
};

/*********************SPI操作命令********************************/
#define     SPI_INIT    0x01    //SPI初始化

/*****************************************************/

#endif
