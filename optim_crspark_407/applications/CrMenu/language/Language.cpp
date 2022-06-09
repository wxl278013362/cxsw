/*
#include <language/Language.h>
#include <Language.h>
#include <LanuageEN.h>
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-25     cx2470       the first version
 */
#include "CrInc/CrConfig.h"
#include "language.h"
#include <map>
#include <string.h>
#include "stdio.h"

#ifdef LCD_LANGUAGE_CN
#include "LanguageCN.h"
#else
#include "LanguageEn.h"
#endif

//#include LANGUAGE_INCL(LCD_LANGUAGE)

static std::map<S32, S32> LaunageMap;

S32 Languages::LanguageMapInit(VOID)
{
    for ( S32 i = 0; NameIdMap[i].Id != MENU_ITEM_FINISH; i++ )
    {
        LaunageMap[(S32)NameIdMap[i].Id] = i;
    }

    return 0;
}

S32 Languages::FindMenuItemNameById(MenuNameId_e ItemNameId, S8 *RecvBuf, S32 BufLen)
{
    S32 Ret = -1;
    if ( ItemNameId == MENU_ITEM_FINISH || !RecvBuf )
        return Ret;

    std::map<S32 ,S32>::iterator NameIter;
    NameIter = LaunageMap.find(ItemNameId);

    if( NameIter != LaunageMap.end() )
    {
        memset(RecvBuf, 0, BufLen);
        memcpy(RecvBuf, NameIdMap[NameIter->second].Name, strlen(NameIdMap[NameIter->second].Name));
        RecvBuf[strlen(NameIdMap[NameIter->second].Name)] = '\0';
        Ret = 1;
    }
    return Ret;
}

