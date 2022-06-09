#include "Lcm12864Driver.h"
#include <stdio.h>
#include "CrLcd/LcdDriver/Src/CrLcm12864Show.h"
#include <stdarg.h>
#include <rtthread.h>

#define BYTE_PIXELS   6

Lcm12864Driver::Lcm12864Driver(S32 SpiFd, S32 MaxRow, S32 MaxCol)
    : RowFixelWidth(LCD_PIXEL_WIDTH)
{
    DriverSpiFd = SpiFd;
    if ( MaxRow > LCD_PIXEL_HEIGHT )
    {
        MaxRow = LCD_PIXEL_HEIGHT;
    }
    else if (MaxRow <= 0)
    {
        MaxRow = 1;
    }

    PageMaxRow = MaxRow;

    S32 ElePixelWidth =  LCD_PIXEL_WIDTH / MaxCol;
    S32 Width = ElePixelWidth / BYTE_PIXELS;
    if ( ElePixelWidth % BYTE_PIXELS )
    {
        Width += 1;
    }
    PageMaxCol = LCD_PIXEL_WIDTH / (Width * BYTE_PIXELS);

    RowFixelHeight = LCD_PIXEL_HEIGHT / PageMaxRow;

}

S32  Lcm12864Driver::Open(S8 *Name, S32 Flag, S32 Mode)
{
    if ( !Name || (DriverSpiFd < 0) )
        return -1;

//    rt_kprintf("fun = %s line = %d  name = %s\n", __FUNCTION__, __LINE__, Name);
    S32 LcdFd = 0;
    LcdFd = CrLcm12864Open(Name, Flag , Mode);
//    rt_kprintf("open 12864 drive fd = %d \n", LcdFd);
    if ( LcdFd < 0 )
    {
        rt_kprintf("Lcd Open Failed\n");
        return LcdFd;
    }

//    rt_kprintf("init Spi fd = %d , Lcd Fd = %d\n", DriverSpiFd, LcdFd);
    CrLcm12864Ioctl(LcdFd, LCD_SET_SPI, (struct LcdCtrlParam_t *)&DriverSpiFd);
//    rt_kprintf("init spi fd finished\n");

    return LcdFd;
}

S32  Lcm12864Driver::Write(S32 Fd, VOID *Data, S32 Len)
{
//    rt_kprintf("fun = %s line = %d  len = %d, Fd = %d, spi Fd = \n", __FUNCTION__, __LINE__, Len, Fd, Len);
    if ( (Fd < 0) || !Data || (Len <= 0) || (DriverSpiFd < 0) )
        return 0;

    //printf("file = %s  fun = %s line = %d  len = %d\n", __FILE__, __FUNCTION__, __LINE__, Len);

    S32 Res = 0;
    Res = CrLcm12864Write(Fd, (S8 *)Data, Len);

    return Res;
}

S32  Lcm12864Driver::Read(S32 Fd, VOID *Data, S32 Len)
{
    if ( (Fd < 0) || !Data || (Len < 0) || (DriverSpiFd < 0) )
        return 0;

//    rt_kprintf("file = %s  fun = %s line = %d  len = %d\n", __FILE__, __FUNCTION__, __LINE__, Len);

    S32 Res = 0;
    Res = CrLcm12864Read(Fd, (S8 *)Data, Len);

    return Res;
//    return CrLcm12864Read(Fd, (S8 *)Data, Len);
}


S32  Lcm12864Driver::Ioctl(S32 Fd, S32 Cmd,...)
{
//    rt_kprintf("12864 line = %d  fd = %d, SpiFd = %d\n",  __LINE__, Fd, DriverSpiFd);
    if ( (Fd < 0) || (DriverSpiFd < 0) )
        return 0;

    VOID *Param = NULL;
    va_list  ls;
    va_start(ls, Cmd);
    Param = va_arg(ls, VOID*);
    va_end(ls);

//    rt_kprintf("driver ioctl param ptr = %p, Cmd = %0x\n", Param, Cmd);

    if ( !Param )
        return 0;

    S32 Res = 0;
    switch (Cmd) {
        case LCD_MOVE_ELEMENT_ROW_AND_COLUMN:
        {
            struct CoordinateParam_t *CoorParam = (struct CoordinateParam_t *)Param;
            struct LcdCtrlParam_t Ctrl;
            Ctrl.X = CoorParam->X_Pos * GetColumnPixelWidth();
            Ctrl.Y = CoorParam->Y_Pos * GetRowPixelHeight();
//            Ctrl.Width = GetRowPixelWidth();
//            Ctrl.Height = GetColumnPixelHeight();
            Res = CrLcm12864Ioctl(Fd, LCD_WRITE_MAP, &Ctrl);
            break;
        }
        case LCD_MOVE_PIXEL_ROW_AND_COLUMN:
        {
            struct CoordinateParam_t *CoorParam = (struct CoordinateParam_t *)Param;
            struct LcdCtrlParam_t Ctrl;
            Ctrl.X = CoorParam->X_Pos;
            Ctrl.Y = CoorParam->Y_Pos;
//            Ctrl.Width = GetRowPixelWidth();
//            Ctrl.Height = GetColumnPixelHeight();
            Res = CrLcm12864Ioctl(Fd, LCD_WRITE_MAP, &Ctrl);
            break;
        }
        default:
        {
            Res = CrLcm12864Ioctl(Fd, Cmd, (struct LcdCtrlParam_t *)Param);
            break;
        }
    }

    return Res;
}


VOID Lcm12864Driver::Close(S32 Fd)
{
    if ( (Fd < 0) || (DriverSpiFd < 0) )
        return;

//    rt_kprintf("file = %s  fun = %s line = %d  fd = %d\n", __FILE__, __FUNCTION__, __LINE__, Fd);
    CrLcm12864Close(Fd);
    DriverSpiFd = -1;

    return ;
}











