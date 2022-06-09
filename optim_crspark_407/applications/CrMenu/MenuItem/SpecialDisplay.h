/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     SpecialDisplay.h
  Author :        Chenli
  Version:        1.0
  Description:    菜单特殊显示
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          Chenli        2020-10-15     V1.0.0.1
****************************************************************************/
#ifndef _SPECIALDISPLAY_H_

#define _SPECIALDISPLAY_H_

#include "CrInc/CrType.h"
#include "CrView/CrShowView.h"
#include "CrInc/CrConfig.h"

/*Check the interval between getting parameters */
/*if ( Val != 0 ) make sure it won’t take too long for the first display*/
#define CHECK_TIMEOUT(Val, Time)\
do {\
    if ( Val != 0 )\
    {\
        if ( (Time - Val) < MENUITEM_STAY_TIME )\
        {\
            Val = Time;\
            return ;\
        }\
    }\
    Val = Time;\
}while(0);

#ifdef __cplusplus
extern "C"
{
#endif

S32 SpecialMenuDisplay(VOID *Arg, S8 *String);
S32 SpecialContenDisplay(MenuNameId_e NameId, U16 Value, S8 *String);
// static S32 SpecialContenDisplayFloat(S8 *String, float Value);

#ifdef __cplusplus
}
#endif

#endif /* _SPECIALDISPLAY_H_ */
