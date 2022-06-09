/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-25     chenli       the first version
 */
#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include "CrInc/CrType.h"
#include "CrView/CrShowView.h"

//#define STRINGIFY_(M) #M
//#define LANGUAGE_INCL_(M) STRINGIFY_(applications/CrMenu/language/language##M.h)
//#define LANGUAGE_INCL(M)  LANGUAGE_INCL_(M)

class Languages {

public:
    Languages(){}
    ~Languages(){}

static S32 LanguageMapInit();
static S32 FindMenuItemNameById(MenuNameId_e ItemNameId, S8 *RecvBuf, S32 BufLen);

};


#endif /* _LANUAGE_H_ */
