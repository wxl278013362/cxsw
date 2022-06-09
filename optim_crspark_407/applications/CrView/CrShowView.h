#ifndef _CRSHOWVIEW_H
#define _CRSHOWVIEW_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrMenu/MenuType.h"

#ifdef __cplusplus
extern "C" {
#endif

/*确认显隐控制标志， 对应HideMask的比特位*/
#define  DEFAULT_HID_MASK                           (0x01 << 0)
#define  MAINMENU_ITEM_PREPARE_HID_MASK             (0x01 << 1)
#define  MAINMENU_ITEM_TUNE_HID_MASK                (0x01 << 2)
#define  MAINMENU_ITEM_NOTFCARD_HID_MASK            (0x01 << 3)
#define  MAINMENU_ITEM_PRINTFFROMTF_HID_MASK        (0x01 << 4)
#define  MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK        (0x01 << 5)
#define  MAINMENU_ITEM_RESUME_PRINTF_HID_MASK       (0x01 << 6)
#define  MAINMENU_ITEM_STOP_PRINTF_HID_MASK         (0x01 << 7)
#define  FILAMENT_ITEM_FIL_DIA_HID_MASK             (0x01 << 8)
#define  MAINMENU_ITEM_LOAD_PARAM_HID_MASK          (0x01 << 9)
#define  MAINMENU_ITEM_RECOVER_PARAM_HID_MASK       (0x01 << 10)
#define  MOVEAXISEMNU_ITEM_AXISE_TOOCOLD_HID_MAKS   (0x01 << 11)
#define  MOVEAXISEMNU_ITEM_AXISE_MAXUNIT_HID_MAKS   (0x01 << 12)
#define  MOVEAXISEMNU_ITEM_AXISE_MIDUNIT_HID_MAKS   (0x01 << 13)
#define  MOVEAXISEMNU_ITEM_AXISE_MINUNIT_HID_MAKS   (0x01 << 14)

extern struct MenuItem_t *RootMenu;
extern U32 HideMask;

VOID CrShowViewInit();

#ifdef __cplusplus
}
#endif

#endif
