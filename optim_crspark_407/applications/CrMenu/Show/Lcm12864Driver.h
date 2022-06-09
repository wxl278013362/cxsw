#ifndef _LCM_12864_DRIVER_H
#define _LCM_12864_DRIVER_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrLcdShowStruct.h"

//#define     LCD_SET_SPI                     0x0200  //12864屏幕操作指令的结尾
//#define   LCD_MOVE_ROW      0x0400        //移动行
//#define   LCD_MOVE_COLUMN   0x0800        //移动列

#define   LCD_PIXEL_WIDTH   128         //注意这个是比特位的位数
#define   LCD_PIXEL_HEIGHT  64          //注意这个是比特位的位数


class Lcm12864Driver
{

public:
    Lcm12864Driver(S32 SpiFd, S32 MaxRow, S32 MaxCol);

    ~Lcm12864Driver(){}

    S32  Open(S8 *Name, S32 Flag, S32 Mode);
    S32  Write(S32 Fd, VOID *Data, S32 Len);
    S32  Read(S32 Fd, VOID *Data, S32 Len);
    S32  Ioctl(S32 Fd, S32 Cmd,...);
    VOID Close(S32 Fd);

    S32  GetMaxRow(){return PageMaxRow;}    //页的最大行数
    S32  GetMaxCol(){return PageMaxCol;}     //页的最大列数
    S32  GetColumnPixelWidth(){return LCD_PIXEL_WIDTH / PageMaxCol;}    //获取一个列的宽度
    S32  GetColumnPixelHeight(){return LCD_PIXEL_HEIGHT;}  //获取列的高度
    S32  GetRowPixelHeight(){return LCD_PIXEL_HEIGHT / PageMaxRow;}     //获取一个行的高度
    S32  GetRowPixelWidth(){return LCD_PIXEL_WIDTH;}                    //获取一个行的宽度

private:
    S32     PageMaxRow;  //一页的最大行数 (显示多少个元素，注意不是屏的像素的高度)
    S32     PageMaxCol;  //页的最大列数(显示字符的个数 注意不是屏的像素的宽度)

    S32     RowFixelHeight;    //行的像素高度          要考虑每个行的位置
    S32     RowFixelWidth;     //行的像素宽度          要考虑每个列的位置

    S32     DriverSpiFd;    //底层驱动Spi的Fd
};



#endif

