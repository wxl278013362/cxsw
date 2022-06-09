/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     SpecialDisplay.cpp
  Author :        Chenli
  Version:        1.0
  Description:    菜单特殊显示
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          Chenli        2020-10-15     V1.0.0.1
****************************************************************************/
#include "SpecialDisplay.h"
#include "../Show/ShowAction.h"
#include "CrInc/CrLcdShowStruct.h"
#include "../language/Language.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

S32 SpecialMenuDisplay(VOID *Arg, S8 *String)
{
    if ( !Arg || !String )
        return -1;

    ShowItemName(Arg);
    struct MenuShowInfo_t *InfoPtr = (struct MenuShowInfo_t *)Arg;
    LcmShow *Show = GetShowObject();
    return Show->WriteString(String, (InfoPtr->Index + 1) * Show->GetRowPixelHeight() , (Show->GetPixelWidth() - (strlen(String) * 6)), 0);
}

S32 SpecialContenDisplay(MenuNameId_e NameId, U16 Value, S8 *String)
{
    LcmShow *Show = GetShowObject();
    if ( !Show || NameId == MENU_ITEM_FINISH )
        return 0;

    S32 Row = Show->GetPixelHeight() / 2;

    S8 RecvBuf[40] = {0};

    if ( Languages::FindMenuItemNameById(NameId, RecvBuf, sizeof(RecvBuf)) < 0 )
        return -1;

    S32 Param = 1;
    Show->Ioctl(LCD_CLEAN, (VOID *)&Param);

    Show->WriteString(RecvBuf, Row + 3 , 0, 0);;

    if ( !String )
    {
        S8 Temp[6] = {0};
        snprintf(Temp, sizeof(Temp),"%d", Value);
        Show->WriteString(Temp, Row + 3 , (Show->GetPixelWidth() - (strlen(Temp) * 6)), 0);
    }
    else
    {
        Show->WriteString(String, Row + 3, (Show->GetPixelWidth() - (strlen(String) * 6)), 0);
    }

    Show->Ioctl(LCD_FLUSH, (VOID *)&Param);

    return 1;
}
