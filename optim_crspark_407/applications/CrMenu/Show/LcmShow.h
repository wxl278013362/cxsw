#ifndef _LCM_SHOW_H
#define _LCM_SHOW_H

#include "CrInc/CrType.h"
//#include "CrMenu.h"

#if 0
//命令的内容
enum CmdType
{
    Clear,          //清楚屏幕的内容
    ClearRow,       //清楚行的内容
    Flush,          //将内容刷新到缓存区。
};
#endif
//使用与非触摸屏(就做的是菜单显示相关的内容，不对菜单数组地址和子菜单地址做保存，对按钮的动作相应放到外面的服务中去做)
class LcmShow
{
public:
    LcmShow();
    virtual ~LcmShow();

    virtual S32  Open(S8 *Name, S32 Flag, S32 Mode) = 0;
    virtual VOID Close() = 0;
    virtual S32  WriteRow(VOID *Data, S32 Len, S32 Row, S32 Col = 0) = 0;
    virtual S32  Write(VOID *Data, S32 Len) = 0;    //从Ioctl中设置的位置写入数据
    virtual S32  ReadRow(VOID *Data, S32 Len, S32 Row, S32 Col = 0) = 0;
    virtual S32  Read(VOID *Data, S32 Len) = 0;    //从Ioctl中设置的位置读数据
    virtual S32  ClearRow(S32 Row) = 0;    //清楚某一行的内容
    virtual S32  Clear() = 0;    //清楚整个屏幕的内容
    virtual S32  ShowString(S8 *Name, S32 Row, S32 Col = 0, S32 HeightLight = 0) = 0;  //Type是:回退菜单，叶子菜单和非叶子菜单
    virtual S32  WriteString(S8 *String, S32 PixelRow, S32 PixelCol = 0, S32 HeightLight = 0) = 0;   //从某个像素点的位置写入字符串

    virtual S32  GetMaxRow() = 0;    //页的最大行数
    virtual S32  GetMaxCol() = 0;     //页的最大列数
    virtual S32  GetColumnPixelWidth() = 0; //获取一个列的宽度
    virtual S32  GetRowPixelHeight() = 0;   //获取一个行的高度
    virtual S32  GetPixelWidth() = 0;    //获取屏幕的像素的宽度
    virtual S32  GetPixelHeight() = 0;    //获取屏幕的像素的高度

    virtual S32  Ioctl(S32 Cmd, ...) = 0;    //进行IO控制

protected:
//    virtual S32  Ioctl(S32 Cmd,...) = 0;    //设置行列

    S32     GetFd(){return Fd;}
protected:
    S32     Fd;
};

#endif

