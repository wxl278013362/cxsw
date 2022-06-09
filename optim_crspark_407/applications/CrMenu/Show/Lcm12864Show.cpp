#include "Lcm12864Show.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <rtthread.h>

#if 0
static S32 MoveRowAndCol(Lcm12864Driver* DriverPtr, S32 Fd, S32 Row, S32 Col)
{
    if ( !DriverPtr || (Fd < 0)
        || (Row < 0) || (Row >= DriverPtr->GetMaxRow())
        || (Col < 0) || (Col >= DriverPtr->GetMaxCol()) )
        return 0;

    return 1;
}

static S32 HasChineseChar(S8 *Str)   //包含有中文字符
{
    if ( !Str )
        return 0;

    S32 HasChinese = 0, i = 0, Count = 0;
    while ( Str[i] )
    {
        if ( Str[i] & 0x80 )
        {
            Count++;
            if ( Count == 2 )
            {
                HasChinese = 1;
                Count = 0;
                break;
            }
        }
        else
        {
            Count = 0;
        }
        i++;
    }

    return HasChinese;
}
#endif

Lcm12864Show::Lcm12864Show(Lcm12864Driver* Driver)
    : DriverPtr(Driver)
{
}

Lcm12864Show::~Lcm12864Show()
{
    //Close();  //如果进行close，屏幕的内容就会被清除
}

S32  Lcm12864Show::Open(S8 *Name, S32 Flag, S32 Mode)
{
    if ( !Name || !DriverPtr)    //驱动不为空
    {
        return -1;
    }

    if(GetFd() >= 0)    //显示已经打开
    {
        return Fd;
    }

    //初始化设备
    Fd = DriverPtr->Open(Name, Flag, Mode);
    
    return Fd;
}

VOID  Lcm12864Show::Close()
{
    if ( (GetFd() >= 0) && DriverPtr )
    {
        DriverPtr->Close(GetFd());
        Fd = -1;
    }
    
//    printf("file = %s  fun = %s line = %d  data len = %d\n", __FILE__, __FUNCTION__, __LINE__, GetFd());

    return ;
}

S32  Lcm12864Show::Write(VOID *Data, S32 Len)
{
    if ( (GetFd() < 0) || (Len <= 0) || !Data || !DriverPtr)
        return 0;

    return DriverPtr->Write(Fd, Data, Len);
}

S32  Lcm12864Show::WriteRow(VOID *Data, S32 Len, S32 Row, S32 Col)
{
    if ( (GetFd() < 0) || (Len <= 0) || !Data || !DriverPtr 
        || (Row < 0) || (Col < 0) )
        return 0;

    struct CoordinateParam_t Param;
    Param.X_Pos = Col;
    Param.Y_Pos = Row;

    DriverPtr->Ioctl(GetFd(), LCD_MOVE_PIXEL_ROW_AND_COLUMN, (VOID *)&Param);

    return DriverPtr->Write(Fd, Data, Len);
}

S32  Lcm12864Show::Read(VOID *Data, S32 Len)
{
    if ( (GetFd() < 0) || (Len <= 0) || !Data || !DriverPtr)
        return 0;

    return DriverPtr->Read(Fd, Data, Len);
}

S32  Lcm12864Show::ReadRow(VOID *Data, S32 Len, S32 Row, S32 Col)
{
    if ( (GetFd() < 0) || (Len <= 0) || !Data || !DriverPtr || (Row < 0) || (Col < 0) )
        return 0;

    struct CoordinateParam_t Param;
    Param.X_Pos = Col;
    Param.Y_Pos = Row;

    DriverPtr->Ioctl(GetFd(), LCD_MOVE_PIXEL_ROW_AND_COLUMN, (VOID *)&Param);

    return DriverPtr->Read(Fd, Data, Len);
}

S32  Lcm12864Show::ClearRow(S32 Row)
{
    if ( (GetFd() < 0) || !DriverPtr || (Row < 0) || (Row >= DriverPtr->GetMaxRow()) )
        return 0;

    S8 Buff[DriverPtr->GetMaxCol()];
    memset(Buff, 0, sizeof(Buff));
    DriverPtr->Write(Fd, Buff, DriverPtr->GetMaxCol());

    return 1;
}

S32  Lcm12864Show::Clear()
{
    if ( (GetFd() < 0) || !DriverPtr )
        return 0;

    S32 i = 0;
    for ( i = 0; i < DriverPtr->GetMaxRow() ; i++ )
    {
        ClearRow(i);
    }

    return 1;
}

S32 MixChineseAndEnglishDisplay(S8 *String, Lcm12864Driver  *DriverPtr, S32 Fd, S16 *X, S16 Y)
{
    S32 ChineseCharCount = 0;
    S32 EnglishCharCount = 0;
    S32  CurentXPos = 0;

    if ( X )
        CurentXPos = *X;

//    printf("CurentXPos = %d\n", CurentXPos);

    S8  Temp[4] = {0};
    memset(Temp, 0, sizeof(Temp));
    struct LcdCtrlParam_t Lcd = {0};

    for (S32 i = 0; String[i] != '\0'; i++)
    {
        Lcd.X = CurentXPos;
        Lcd.Y = Y;
        DriverPtr->Ioctl(Fd, LCD_WRITE_STRING, (VOID *)&Lcd);

        if ( (String[i] & 0x80) && (String[i + 1] & 0x80) && (String[i + 2] & 0x80) )
        {
            i += 2;
            DriverPtr->Ioctl(Fd, LCD_USING_UTF8_CHARATER_SET, String[i]);
            snprintf(Temp, 4, "%s", (String + (ChineseCharCount * 3) + (EnglishCharCount)));
            ChineseCharCount++;
            CurentXPos += DriverPtr->Ioctl(Fd, LCD_GET_FONT_WIGTH_OFCURRENT, String[i]) + 1;
        }
        else
        {
            DriverPtr->Ioctl(Fd, LCD_USING_ASCII_CHARATER_SET, (VOID *)String);
            snprintf(Temp, 2, "%s", (String + (ChineseCharCount * 3) + (EnglishCharCount)));
            EnglishCharCount++;
            if (!((String[i] & 0x80) && (String[i + 1] & 0x80))) //非特殊英文字符
                CurentXPos += DriverPtr->Ioctl(Fd, LCD_GET_FONT_WIGTH_OFCURRENT, String[i]);
        }

        DriverPtr->Write(Fd, Temp, 1);
    }

    return 0;
}

S32  Lcm12864Show::ShowString(S8 *String, S32 Row, S32 Col, S32 HeightLight)
{
    if ( (GetFd() < 0) || !String || !DriverPtr 
        || (Row < 0) || (Col < 0) )
        return 0;

   // ClearRow(Row);
    S32 Len = strlen(String);
    if ( Len < 0)
        return 0;

    if ( (Col + Len) >= DriverPtr->GetMaxCol()  )
        Len = DriverPtr->GetMaxCol() - Col;

    struct LcdCtrlParam_t Lcd = {0};
    Lcd.Y = (Row + 1) * DriverPtr->GetRowPixelHeight();

    if ( Row < DriverPtr->GetMaxRow() )
    {
        MixChineseAndEnglishDisplay(String, DriverPtr, GetFd(), NULL, Lcd.Y);
    }

    if ( HeightLight )
    {
        S32 Y = 0;
        Y = (Row) * DriverPtr->GetRowPixelHeight() + 2;     //像素的行
        S32 Width = 0, Height = 0;

        /*按照ender 3 marlin的菜单修改*/
        Width = DriverPtr->GetRowPixelWidth() + 2;
        Height = DriverPtr->GetRowPixelHeight() + 1;

        struct LcdCtrlParam_t Param;
        Param.X = -1;
        Param.Y = Y;
        Param.Width = Width;
        Param.Height = Height;
        DriverPtr->Ioctl(GetFd(), LCD_WRITE_RECTANGLE, (VOID *)&Param);
    }

    return 1;
}

S32  Lcm12864Show::WriteString(S8 *String, S32 PixelRow, S32 PixelCol, S32 HeightLight)
{
    if ( (GetFd() < 0) || !String || !DriverPtr
        || (PixelRow < 0) || (PixelCol < 0) )
        return 0;

//    rt_kprintf("data = %s\n", String);
    S32 Len = strlen(String);

    if ( Len < 0 )
        return 0;

//    printf("PixelCol %d \n", PixelCol);
    MixChineseAndEnglishDisplay(String, DriverPtr, GetFd(), (S16 *)&PixelCol,PixelRow);
    //将外框设置为高亮
    if ( HeightLight )
    {
        S32 Width = 0, Height = 0, X = 0, Y = 0;
        Width = DriverPtr->GetRowPixelWidth();
        Height = DriverPtr->GetRowPixelHeight();
        X = PixelCol;    //像素的列
        Y = PixelRow;     //像素的行
        struct LcdCtrlParam_t Param;
        Param.X = X;
        Param.Y = Y;
        Param.Width = Width;
        Param.Height = Height;
        DriverPtr->Ioctl(GetFd(), LCD_WRITE_RECTANGLE, (VOID *)&Param);
    }

    return 1;
}

S32  Lcm12864Show::GetMaxRow()
{
    if ( !DriverPtr )
        return 0;

    return DriverPtr->GetMaxRow();
}


S32  Lcm12864Show::GetMaxCol()
{
    if ( !DriverPtr )
        return 0;

    return DriverPtr->GetMaxCol();
}

S32  Lcm12864Show::Ioctl(S32 Cmd, ...)
{
    if ( !DriverPtr )
    {
        return 0;
    }

    VOID *Param = NULL;
    va_list  ls;
    va_start(ls, Cmd);
    Param = va_arg(ls, VOID*);
    va_end(ls);

    if ( !Param )
        return 0;

    return DriverPtr->Ioctl(GetFd(), Cmd, Param);
}

S32  Lcm12864Show::GetColumnPixelWidth()
{
    if ( !DriverPtr )
        return 0;

    return DriverPtr->GetColumnPixelWidth();
}
S32  Lcm12864Show::GetRowPixelHeight()
{
    if ( !DriverPtr )
        return 0;

    return DriverPtr->GetRowPixelHeight();
}

S32  Lcm12864Show::GetPixelWidth()
{
    if ( !DriverPtr )
        return 0;

    return DriverPtr->GetRowPixelWidth();
}

S32  Lcm12864Show::GetPixelHeight()
{
    if ( !DriverPtr )
        return 0;

    return DriverPtr->GetColumnPixelHeight();
}
