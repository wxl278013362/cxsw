/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     AboutPrinter.cpp
  Author :        Chenli
  Version:        1.0
  Description:    打印机信息
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          Chenli        2020-10-15     V1.0.0.1
****************************************************************************/
#include "AboutPrinterMenuItem.h"
#include <rtthread.h>
#include <string.h>
#include "CrView/CrShowView.h"
#include "../Action/MenuDefaultAction.h"
#include "../Show/LcmShow.h"
#include "../Show/Lcm12864Show.h"
#include "../Show/Lcm12864Driver.h"
#include "../Show/ShowAction.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrLcdShowStruct.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

#if 0
static S8 *FwName = (S8 *)FIRMWARE_NAME;
static S8 *VersionNumber = (S8 *)DEVICE_FW_VERSION;
static S8 *Date = (S8 *)FIRMWARE_DATE;
//static S8 *PrinterName = (S8 *)DEVICE_MODE_NAME;
static S8 *DomainName = (S8 *)COMPANY_DOMAIN;
static S8 ExtruderNum[15] = {0};
static S8 BaudRate[20] = {0};

S8* PrintfInfo[]=
{
    FwName,
    VersionNumber,
    Date,
//    PrinterName,
    DomainName,
    ExtruderNum,
    BaudRate
};
#endif

#define STR1(R)                 #R
#define STR2(R)                 STR1(R)

#define STR_EXTRUDER_NUM        "Extruders:" STR2(EXTRUDER_NUM)
#define STR_UART_BAUDRATE       "Baudrate:" STR2(UART_BAUDRATE)

static S8* PrintfInfo[]=
{
    (S8*)FIRMWARE_NAME,
    (S8*)DEVICE_FW_VERSION,
    (S8*)FIRMWARE_DATE,
//    PrinterName,
    COMPANY_DOMAIN,
    (S8*)STR_EXTRUDER_NUM,
    (S8*)STR_UART_BAUDRATE
};

static S32 Flag = LCD_PIXEL_HEIGHT / 5;
static U8 Index = 0;

#define GET_LCD_MAX_ROW()  GetShowObject()->GetMaxRow()

S32 CenterDisplayX(S8 *DisplayPtr)
{
    LcmShow *Show = GetShowObject();
    if ( Show )
        return ((Show->GetMaxCol() - strlen(DisplayPtr)) / 2) * (LCD_PIXEL_WIDTH / Show->GetMaxCol());

    return 0;
}

S32 AboutPrinterMenuShow(VOID *Arg)
{
    LcmShow *Show = GetShowObject();
    if ( !Show )
        return 0;

    S32 Param = 1;

#if 0
    snprintf(ExtruderNum, sizeof(ExtruderNum), "Extruders:%d", EXTRUDER_NUM);
    snprintf(BaudRate, sizeof(BaudRate), "Baudrate:%d", UART_BAUDRATE);
#endif

    Show->Ioctl(LCD_CLEAN, (VOID *)&Param);
    Param = Flag;

    for ( U8 Temp = Index, Count = 1; Count <= Show->GetMaxRow() ; Temp++, Count++ )
    {
        Show->WriteString(PrintfInfo[Temp], Param, CenterDisplayX(PrintfInfo[Temp]), 0);
        Param += LCD_PIXEL_HEIGHT / Show->GetMaxRow();
    }

    Show->Ioctl(LCD_FLUSH, (VOID *)&Param);

    return 1;
}

VOID *AboutPrinterUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( Flag < LCD_PIXEL_HEIGHT / GET_LCD_MAX_ROW() )
        Flag = Flag + (LCD_PIXEL_HEIGHT / GET_LCD_MAX_ROW());
    if ( Index > 0 )
            Index--;
        return NULL;
}

VOID* AboutPrinterDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( Flag > LCD_PIXEL_HEIGHT / GET_LCD_MAX_ROW() )
        Flag = Flag - (LCD_PIXEL_HEIGHT / GET_LCD_MAX_ROW());
    if ( Index < sizeof(PrintfInfo) / sizeof(PrintfInfo[0]) - GET_LCD_MAX_ROW())
        Index++;
    return NULL;
}

VOID* AboutPrinterOkAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;
//    CrBeepAppShortBeeps(); /* 短鸣 */

    S32 *BackParent = (S32 *)ArgOut;
    Index = 0;
    Flag = LCD_PIXEL_HEIGHT / GET_LCD_MAX_ROW();

    *BackParent = BACK_PARENT;

    return (VOID *)&Index;
}

