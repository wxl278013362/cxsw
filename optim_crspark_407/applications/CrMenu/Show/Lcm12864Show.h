
#ifndef _LCM12864_SHOW_H
#define _LCM12864_SHOW_H


#include "LcmShow.h"
#include "Lcm12864Driver.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrLcdShowStruct.h"

class Lcm12864Show : public LcmShow
{
public:
    Lcm12864Show(Lcm12864Driver* Driver);
    virtual ~Lcm12864Show();

    S32  Open(S8 *Name, S32 Flag, S32 Mode);

    S32  WriteRow(VOID *Data, S32 Len, S32 Row, S32 Col = 0);    //注意此处的行是像素的行，不是元素的行号
    S32  Write(VOID *Data, S32 Len);    //从Ioctl中设置的位置写入数据
    S32  ReadRow(VOID *Data, S32 Len, S32 Row, S32 Col = 0);     //注意此处的行是像素的行，不是元素的行号
    S32  Read(VOID *Data, S32 Len);    //从Ioctl中设置的位置读数据
    S32  ClearRow(S32 Row);    //清楚某一行的内容
    S32  Clear();    //清楚整个屏幕的内容
    S32  ShowString(S8 *String, S32 Row, S32 Col = 0, S32 HeightLight = 0);  //注意此处的行列不是像素，而是元素或字符的行列
    S32  WriteString(S8 *String, S32 PixelRow, S32 PixelCol = 0, S32 HeightLight = 0);    //在指定的像素点位置开始写入字符串

    S32  GetMaxRow();    //页的最大行数
    S32  GetMaxCol();     //页的最大列数

    S32  Ioctl(S32 Cmd, ...);           //控制的内容

    S32  GetColumnPixelWidth(); //获取一个列的宽度
    S32  GetRowPixelHeight();   //获取一个行的高度
    S32  GetPixelWidth();    //获取屏幕的像素的宽度
    S32  GetPixelHeight();    //获取屏幕的像素的高度

protected:
    VOID  Close(); //如果进行close，屏幕的内容就会被清除

private:
    Lcm12864Driver      *DriverPtr;    //驱动的指针
};

#endif

