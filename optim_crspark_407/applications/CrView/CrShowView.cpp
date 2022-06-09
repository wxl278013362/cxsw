#include "CrShowView.h"
#include "CrMenu/Show/ShowAction.h"
#include "CrInc/CrConfig.h"
#include "CrMenu/Action/MenuDefaultAction.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

#if 1
#include "CrMenu/MenuItem/AboutPrinterMenuItem.h"
#include "CrMenu/MenuItem/SdCardMenuItem.h"
#include "CrMenu/MenuItem/PrintMenuItem.h"
#include "CrMenu/MenuItem/TemperatureMenuItem.h"
#include "CrMenu/MenuItem/MoveAxisMenuItem.h"
#include "CrMenu/MenuItem/DisableStepperMenuitem.h"
#include "CrMenu/MenuItem/PreheatMaterialMenuitem.h"
#include "CrMenu/MenuItem/VelocityMenuItem.h"
#include "CrMenu/MenuItem/AccelerationMenuItem.h"
#include "CrMenu/MenuItem/JerkMenuItem.h"
#include "CrMenu/MenuItem/StepsMenuItem.h"
#include "CrMenu/MenuItem/FilamentMenuItem.h"
//#include "CommonFuncAndMacros.h"
#include "CrMenu/MenuItem/HomeOffsetMenuItem.h"
#include "CrMenu/MenuItem/ParamMenuItem.h"
#include "CrMenu/MenuItem/PrintfSpeed.h"
#include "CrMenu/MenuItem/FlowMenuItem.h"
#endif

U32 HideMask = 0;    //最开始是有隐藏的菜单的,所有菜单显影属性控制标志

VOID *DefaultOK(VOID *ArgOut, VOID *ArgIn)
{
    CrBeepAppShortBeeps();
    return MoveToChild(ArgOut, ArgIn);
}

/**************************************************************************************************/
VOID* (*DefaultOKAction)(VOID *ArgOut, VOID *ArgIn) = DefaultOK;
VOID* (*DefaultUpAction)(VOID *ArgOut, VOID *ArgIn) = MoveToPrevItem;
VOID* (*DefaultDownAction)(VOID *ArgOut, VOID *ArgIn) = MoveToNextItem;
S32 (*DefaultShowMenuItem)(VOID *Arg) = ShowItemName;
/************************************************************************************************/
#if 1
const static struct MenuItem_t X_MaxUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_MAX_UNIT_NAME_ID, NULL
                     , X_AixsMaxUnitMoveMenuDownAction
                     , X_AixsMaxUnitMoveMenuUpAction
                     , X_AixsMoveMenuItemShow
                     , DEFAULT_HID_MASK
                     , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置X轴中等单位移动菜单
const static struct MenuItem_t X_MidUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_MID_UNIT_NAME_ID, NULL
                     , X_AixsMidUnitMoveMenuDownAction
                     , X_AixsMidUnitMoveMenuUpAction
                     , X_AixsMoveMenuItemShow
                     , DEFAULT_HID_MASK
                     , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置X轴最小单位移动菜单
const static struct MenuItem_t X_MinUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_MIN_UNIT_NAME_ID, NULL
                     , X_AixsMinUnitMoveMenuDownAction
                     , X_AixsMinUnitMoveMenuUpAction
                     , X_AixsMoveMenuItemShow
                     , DEFAULT_HID_MASK
                     ,NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置Y轴最大单位移动菜单
const static struct MenuItem_t Y_MaxUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_MAX_UNIT_NAME_ID, NULL
                     , Y_AixsMaxUnitMoveMenuDownAction
                     , Y_AixsMaxUnitMoveMenuUpAction
                     , Y_AixsMoveMenuItemShow
                     , DEFAULT_HID_MASK
                     , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置Y轴中等单位移动菜单
const static struct MenuItem_t Y_MidUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_MID_UNIT_NAME_ID, NULL
                    , Y_AixsMidUnitMoveMenuDownAction
                    , Y_AixsMidUnitMoveMenuUpAction
                    , Y_AixsMoveMenuItemShow
                    , DEFAULT_HID_MASK
                    , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置Y轴最小单位移动菜单
const static struct MenuItem_t Y_MinUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_MIN_UNIT_NAME_ID, NULL
                    , Y_AixsMinUnitMoveMenuDownAction
                    , Y_AixsMinUnitMoveMenuUpAction
                    , Y_AixsMoveMenuItemShow
                    , DEFAULT_HID_MASK
                    , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置Z轴最大单位移动菜单
const static struct MenuItem_t Z_MaxUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_MAX_UNIT_NAME_ID, NULL
                     , Z_AixsMaxUnitMoveMenuDownAction
                     , Z_AixsMaxUnitMoveMenuUpAction
                     , Z_AixsMoveMenuItemShow
                     , DEFAULT_HID_MASK
                     , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置Z轴中等单位移动菜单
const static struct MenuItem_t Z_MidUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_MID_UNIT_NAME_ID, NULL
                     , Z_AixsMidUnitMoveMenuDownAction
                     , Z_AixsMidUnitMoveMenuUpAction
                     , Z_AixsMoveMenuItemShow
                     , DEFAULT_HID_MASK
                     , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置Z轴最小单位移动菜单
const static struct MenuItem_t Z_MinUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_MIN_UNIT_NAME_ID, NULL
                     , Z_AixsMinUnitMoveMenuDownAction
                     , Z_AixsMinUnitMoveMenuUpAction
                     , Z_AixsMoveMenuItemShow
                     , DEFAULT_HID_MASK
                     , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置Z轴最小单位移动菜单
const static struct MenuItem_t Z_LeastUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_0_125_MM_NAME_ID, NULL
                     , Z_AixsLeastUnitMoveMenuDownAction
                     , Z_AixsLeastUnitMoveMenuUpAction
                     , Z_AixsLeastMoveMenuItemShow
                     , DEFAULT_HID_MASK
                     , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置挤出机最大单位移动菜单
const static struct MenuItem_t E_MaxUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_MAX_UNIT_NAME_ID, NULL
                     , ExtruderMaxUnitMoveMenuDownAction
                     , ExtruderMaxUnitMoveMenuUpAction
                     , ExtruderMoveMenuItemShow
                     , DEFAULT_HID_MASK
                     , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置挤出机中等单位移动菜单
const static struct MenuItem_t E_MidUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_MID_UNIT_NAME_ID, NULL
                     , ExtruderMidUnitMoveMenuDownAction
                     , ExtruderMidUnitMoveMenuUpAction
                     , ExtruderMoveMenuItemShow
                     , DEFAULT_HID_MASK
                     , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置挤出机最小单位移动菜单
const static struct MenuItem_t E_MinUnitMoveMenu[] =
{
    {MOVE_AXIS_ITEM_MOVE_MIN_UNIT_NAME_ID, NULL
                     , ExtruderMinUnitMoveMenuDownAction
                     , ExtruderMinUnitMoveMenuUpAction
                     , ExtruderMoveMenuItemShow
                     , DEFAULT_HID_MASK
                     , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//子菜单在下一级
const static struct MenuItem_t X_AxisPosMenu[] =
{
    {PREPARE_ITEM_MOVE_AXIS_NAME_ID,     NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {MOVE_AXIS_ITEM_MOVE_10_MM_NAME_ID,  DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)X_MaxUnitMoveMenu},
    {MOVE_AXIS_ITEM_MOVE_1_MM_NAME_ID,   DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)X_MidUnitMoveMenu},
    {MOVE_AXIS_ITEM_MOVE_0_1_MM_NAME_ID, DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)X_MinUnitMoveMenu},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//子菜单在下一级
const static struct MenuItem_t Y_AxisPosMenu[] =
{
    {PREPARE_ITEM_MOVE_AXIS_NAME_ID,      NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {MOVE_AXIS_ITEM_MOVE_10_MM_NAME_ID,   DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)Y_MaxUnitMoveMenu},
    {MOVE_AXIS_ITEM_MOVE_1_MM_NAME_ID,    DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)Y_MidUnitMoveMenu},
    {MOVE_AXIS_ITEM_MOVE_0_1_MM_NAME_ID,  DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)Y_MinUnitMoveMenu},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};
//移动
const static struct MenuItem_t Z_AxisPosMenu[] =
{
    {PREPARE_ITEM_MOVE_AXIS_NAME_ID,       NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {MOVE_AXIS_ITEM_MOVE_10_MM_NAME_ID,    DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)Z_MaxUnitMoveMenu},
    {MOVE_AXIS_ITEM_MOVE_1_MM_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)Z_MidUnitMoveMenu},
    {MOVE_AXIS_ITEM_MOVE_0_1_MM_NAME_ID,   DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)Z_MinUnitMoveMenu},
    {MOVE_AXIS_ITEM_MOVE_0_125_MM_NAME_ID, DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)Z_LeastUnitMoveMenu},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//移动挤出机
const static struct MenuItem_t ExtruderPosMenu[] =
{
    {PREPARE_ITEM_MOVE_AXIS_NAME_ID,     NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {MOVE_AXIS_ITEM_TOOCOLD_NAME_ID,     NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MOVEAXISEMNU_ITEM_AXISE_TOOCOLD_HID_MAKS, NULL},
    {MOVE_AXIS_ITEM_MOVE_10_MM_NAME_ID,  DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MOVEAXISEMNU_ITEM_AXISE_MAXUNIT_HID_MAKS, (struct MenuItem_t*)E_MaxUnitMoveMenu},
    {MOVE_AXIS_ITEM_MOVE_1_MM_NAME_ID,   DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MOVEAXISEMNU_ITEM_AXISE_MIDUNIT_HID_MAKS, (struct MenuItem_t*)E_MidUnitMoveMenu},
    {MOVE_AXIS_ITEM_MOVE_0_1_MM_NAME_ID, DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MOVEAXISEMNU_ITEM_AXISE_MINUNIT_HID_MAKS, (struct MenuItem_t*)E_MinUnitMoveMenu},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置热床预热温度
const static struct MenuItem_t BedTempMenu[] =
{
    {TUNE_ITEM_BED_NAME_ID, PreheatBedMenuOKAction
                          , PreheatBedMenuDownAction
                          , PreheatBedMenuUpAction
                          , PreheatBedMenuShow
                          , DEFAULT_HID_MASK
                          , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//设置喷头预热温度
const static struct MenuItem_t NozzleTempMenu[] =
{
    {TUNE_ITEM_NOZZLE_NAME_ID, PreheatNozzleMenuOKAction
                             , PreheatNozzleMenuDownAction
                             , PreheatNozzleMenuUpAction
                             , PreheatNozzleMenuShow
                             , DEFAULT_HID_MASK
                             , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

//风扇速度
const static struct MenuItem_t FansSpeedMenu[] =
{
    {TUNE_ITEM_FANSPEED_NAME_ID  , FansSpeedMenuOKAction
                                 , FansSpeedMenuDownAction
                                 , FansSpeedMenuUPAction
                                 , FansSpeedMenuShow
                                 , DEFAULT_HID_MASK
                                 , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

/*===============================================================================
 *                          Preheat PLA Conf start
 * ==============================================================================*/
const static struct MenuItem_t PLAFansSpeedMenu[] =
{
    {TUNE_ITEM_FANSPEED_NAME_ID  , PLAFansSpeedMenuOKAction
                                 , PLAFansSpeedMenuDownAction
                                 , PLAFansSpeedMenuUPAction
                                 , PLAFansSpeedMenuShow
                                 , DEFAULT_HID_MASK
                                 , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t PLANozzleTempMenu[] =
{
    {TUNE_ITEM_NOZZLE_NAME_ID  , PLAPreheatNozzleMenuOkAction
                               , PLAPreheatNozzleMenuDownAction
                               , PLAPreheatNozzleMenuUpAction
                               , PLAPreheatNozzleMenuShow
                               , DEFAULT_HID_MASK
                               , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t PLABedTempMenu[] =
{
    {TUNE_ITEM_BED_NAME_ID  , PLAPreheatBedMenuOkAction
                            , PLAPreheatBedMenuDownAction
                            , PLAPreheatBedMenuUPAction
                            , PLAPreheatBedMenuShow
                            , DEFAULT_HID_MASK
                            , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t PLAConfMenu[] =
{
    {CONSIGURATION_NAME_ID,       NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {TUNE_ITEM_FANSPEED_NAME_ID,  DefaultOKAction, DefaultUpAction, DefaultDownAction, PLAFanSpeedSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)PLAFansSpeedMenu},
    {TUNE_ITEM_NOZZLE_NAME_ID,    DefaultOKAction, DefaultUpAction, DefaultDownAction, PLANozzleTempSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)PLANozzleTempMenu},
    {TUNE_ITEM_BED_NAME_ID,       DefaultOKAction, DefaultUpAction, DefaultDownAction, PLABedTempSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)PLABedTempMenu},
    {CONTROL_ITEM_STORE_SETTING_NAME_ID, ParamPlaSaveOkAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};


/*===============================================================================
 *                          Preheat ABS Conf start
 * ==============================================================================*/
const static struct MenuItem_t ABSFanSpeedMenu[] =
{
    {TUNE_ITEM_FANSPEED_NAME_ID , ABSFansSpeedMenuOKAction
                          , ABSFansSpeedMenuDownAction
                          , ABSFansSpeedMenuUPAction
                          , ABSFansSpeedMenuShow
                          , DEFAULT_HID_MASK
                          , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t ABSNozzleTempMenu[] =
{
    {TUNE_ITEM_NOZZLE_NAME_ID  , ABSPreheatNozzleMenuOkAction
                            , ABSPreheatNozzleMenuDownAction
                            , ABSPreheatNozzleMenuUpAction
                            , ABSPreheatNozzleMenuShow
                            , DEFAULT_HID_MASK
                            , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t ABSBedTempMenu[] =
{
    {TUNE_ITEM_BED_NAME_ID  , ABSPreheatBedMenuOkAction
                            , ABSPreheatBedMenuDownAction
                            , ABSPreheatBedMenuUPAction
                            , ABSPreheatBedMenuShow
                            , DEFAULT_HID_MASK
                            , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t ABSConfMenu[] =
{
    {CONSIGURATION_NAME_ID,     NULL,    DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {TUNE_ITEM_FANSPEED_NAME_ID,         DefaultOKAction, DefaultUpAction, DefaultDownAction, ABSFanSpeedSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)ABSFanSpeedMenu},
    {TUNE_ITEM_NOZZLE_NAME_ID,           DefaultOKAction, DefaultUpAction, DefaultDownAction, ABSNozzleTempSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)ABSNozzleTempMenu},
    {TUNE_ITEM_BED_NAME_ID,              DefaultOKAction, DefaultUpAction, DefaultDownAction, ABSBedTempSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)ABSBedTempMenu},
    {CONTROL_ITEM_STORE_SETTING_NAME_ID, ParamAbsSaveOkAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

/*===============================================================================
 *                              Temperature Contents start
 * ==============================================================================*/
const static struct MenuItem_t TempControl[] =
{
    {MAINMENU_ITEM_MAIN_NAME_ID,           NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL}, //next和okaction同时为空表示回退到上一级菜单
    {TUNE_ITEM_NOZZLE_NAME_ID,             DefaultOKAction, DefaultUpAction, DefaultDownAction, NozzleTempSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)NozzleTempMenu},
    {TUNE_ITEM_BED_NAME_ID,                DefaultOKAction, DefaultUpAction, DefaultDownAction, BedTempSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)BedTempMenu},
    {TUNE_ITEM_FANSPEED_NAME_ID,           DefaultOKAction, DefaultUpAction, DefaultDownAction, FanSpeedSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)FansSpeedMenu},
    {TEMPERATURE_PREHEAT_PLA_CONF_NAME_ID, DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)PLAConfMenu},
    {TEMPERATURE_PREHEAT_ABS_CONF_NAME_ID, DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)ABSConfMenu},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

/*===============================================================================
 *                              Velocity Contents start
 * ==============================================================================*/
const static struct MenuItem_t VmaxXMenu[] = {
    {VELOCITY_ITEM_VMAX_X_NAME_ID, VelocityVmaxXOKAction
                                 , VelocityVmaxXDownAction
                                 , VelocityVmaxXUpAction
                                 , VelocityVmaxXMenuShow
                                 , DEFAULT_HID_MASK
                                 , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t VmaxYMenu[] = {
    {VELOCITY_ITEM_VMAX_Y_NAME_ID, VelocityVmaxYOKAction
                                 , VelocityVmaxYDownAction
                                 , VelocityVmaxYUpAction
                                 , VelocityVmaxYMenuShow
                                 , DEFAULT_HID_MASK
                                 , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};


const static struct MenuItem_t VmaxZMenu[] = {
    {VELOCITY_ITEM_VMAX_Z_NAME_ID, VelocityVmaxZOKAction
                                 , VelocityVmaxZDownAction
                                 , VelocityVmaxZUpAction
                                 , VelocityVmaxZMenuShow
                                 , DEFAULT_HID_MASK
                                 , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t VmaxEMenu[] = {
    {VELOCITY_ITEM_VMAX_E_NAME_ID, VelocityVmaxEOKAction
                                 , VelocityVmaxEDownAction
                                 , VelocityVmaxEUpAction
                                 , VelocityVmaxEMenuShow
                                 , DEFAULT_HID_MASK
                                 , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};


const static struct MenuItem_t VminMenu[] = {
    {VELOCITY_ITEM_VMIN_NAME_ID, VelocityVminOKAction
                               , VelocityVminDownAction
                               , VelocityVminUpAction
                               , VelocityVminMenuShow
                               , DEFAULT_HID_MASK
                               , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};


const static struct MenuItem_t VTravMinMenu[] = {
    {VELOCITY_ITEM_VTRAV_MIN_NAME_ID, VelocityVTravMinOKAction
                                    , VelocityVTravMinDownAction
                                    , VelocityVTravMinUpAction
                                    , VelocityVTravMinMenuShow
                                    , DEFAULT_HID_MASK
                                    , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t VelocityMenu[] =
{
    {ADVANCED_SETTING_NAME_ID,         NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {VELOCITY_ITEM_VMAX_X_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction, VelocityVmaxXspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)VmaxXMenu},
    {VELOCITY_ITEM_VMAX_Y_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction, VelocityVmaxYspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)VmaxYMenu},
    {VELOCITY_ITEM_VMAX_Z_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction, VelocityVmaxZspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)VmaxZMenu},
    {VELOCITY_ITEM_VMAX_E_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction, VelocityVmaxEspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)VmaxEMenu},
    {VELOCITY_ITEM_VMIN_NAME_ID,       DefaultOKAction, DefaultUpAction, DefaultDownAction, VelocityVminspecialDisplay, DEFAULT_HID_MASK,  (struct MenuItem_t*)VminMenu },
    {VELOCITY_ITEM_VTRAV_MIN_NAME_ID,  DefaultOKAction, DefaultUpAction, DefaultDownAction, VelocityVTravMinspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)VTravMinMenu},
    {MENU_ITEM_FINISH,  NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

/*===============================================================================
 *                              Acceleration Contents start
 * ==============================================================================*/
const static struct MenuItem_t AccelMenu[] = {
    {ACCELERATION_ITEM_ACCEL_NAME_ID   , AcceleOKAction
                                       , AcceleDownAction
                                       , AcceleUpAction
                                       , AcceleMenuShow
                                       , DEFAULT_HID_MASK
                                       , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};
const static struct MenuItem_t ARetractMenu[] = {
    {ACCELERATION_ITEM_A_RETRACT_NAME_ID, AcceleARetractOKAction
                                        , AcceleARetractDownAction
                                        , AcceleARetractUpAction
                                        , AcceleARetractMenuShow
                                        , DEFAULT_HID_MASK
                                        , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};
const static struct MenuItem_t ATravelMenu[] = {
    {ACCELERATION_ITEM_A_TRAVEL_NAME_ID , AcceleATravelOKAction
                                        , AcceleATravelDownAction
                                        , AcceleATravelUpAction
                                        , AcceleATravelMenuShow
                                        , DEFAULT_HID_MASK
                                        , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};
const static struct MenuItem_t AmaxXMenu[] = {
    {ACCELERATION_ITEM_AMAX_X_NAME_ID   , AcceleAmaxXOKAction
                                        , AcceleAmaxXDownAction
                                        , AcceleAmaxXUpAction
                                        , AcceleAmaxXMenuShow
                                        , DEFAULT_HID_MASK
                                        , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};
const static struct MenuItem_t AmaxYMenu[] = {
    {ACCELERATION_ITEM_AMAX_Y_NAME_ID   , AcceleAmaxYOKAction
                                        , AcceleAmaxYDownAction
                                        , AcceleAmaxYUpAction
                                        , AcceleAmaxYMenuShow
                                        , DEFAULT_HID_MASK
                                        , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};
const static struct MenuItem_t AmaxZMenu[] = {
    {ACCELERATION_ITEM_AMAX_Z_NAME_ID   , AcceleAmaxZOKAction
                                        , AcceleAmaxZDownAction
                                        , AcceleAmaxZUpAction
                                        , AcceleAmaxZMenuShow
                                        , DEFAULT_HID_MASK
                                        , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};
const static struct MenuItem_t AmaxEMenu[] = {
    {ACCELERATION_ITEM_AMAX_E_NAME_ID   , AcceleAmaxEOKAction
                                        , AcceleAmaxEDownAction
                                        , AcceleAmaxEUpAction
                                        , AcceleAmaxEMenuShow
                                        , DEFAULT_HID_MASK
                                        , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t AccelerationMenu[] =
{
    {ADVANCED_SETTING_NAME_ID,            NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {ACCELERATION_ITEM_ACCEL_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction, AccelespecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)AccelMenu},
    {ACCELERATION_ITEM_A_RETRACT_NAME_ID, DefaultOKAction, DefaultUpAction, DefaultDownAction, AcceleARetractspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)ARetractMenu},
    {ACCELERATION_ITEM_A_TRAVEL_NAME_ID,  DefaultOKAction, DefaultUpAction, DefaultDownAction, AcceleATravelspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)ATravelMenu},
    {ACCELERATION_ITEM_AMAX_X_NAME_ID,    DefaultOKAction, DefaultUpAction, DefaultDownAction, AcceleAmaxXspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)AmaxXMenu},
    {ACCELERATION_ITEM_AMAX_Y_NAME_ID,    DefaultOKAction, DefaultUpAction, DefaultDownAction, AcceleAmaxYspecialDisplay, DEFAULT_HID_MASK,  (struct MenuItem_t*)AmaxYMenu },
    {ACCELERATION_ITEM_AMAX_Z_NAME_ID,    DefaultOKAction, DefaultUpAction, DefaultDownAction, AcceleAmaxZspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)AmaxZMenu},
    {ACCELERATION_ITEM_AMAX_E_NAME_ID,    DefaultOKAction, DefaultUpAction, DefaultDownAction, AcceleAmaxEspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)AmaxEMenu},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

/*===============================================================================
 *                              Jerk Contents start
 * ==============================================================================*/
const static struct MenuItem_t VXJerkMenu[] = {
    {JERK_ITEM_X_NAME_ID  , VxJerkOKAction
                          , VxJerkDownAction
                          , VxJerkUpAction
                          , VxJerkMenuShow
                          , DEFAULT_HID_MASK
                          , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t VYJerkMenu[] = {
    {JERK_ITEM_Y_NAME_ID  , VyJerkOKAction
                          , VyJerkDownAction
                          , VyJerkUpAction
                          , VyJerkMenuShow
                          , DEFAULT_HID_MASK
                          , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t VZJerkMenu[] = {
    {JERK_ITEM_Z_NAME_ID  , VzJerkOKAction
                          , VzJerkDownAction
                          , VzJerkUpAction
                          , VzJerkMenuShow
                          , DEFAULT_HID_MASK
                          , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t VEJerkMenu[] = {
    {JERK_ITEM_E_NAME_ID  , VeJerkOKAction
                          , VeJerkDownAction
                          , VeJerkUpAction
                          , VeJerkMenuShow
                          , DEFAULT_HID_MASK
                          , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t JerkMenu[] =
{
    {ADVANCED_SETTING_NAME_ID,  NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {JERK_ITEM_X_NAME_ID,       DefaultOKAction, DefaultUpAction, DefaultDownAction, VxJerkspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)VXJerkMenu},
    {JERK_ITEM_Y_NAME_ID,       DefaultOKAction, DefaultUpAction, DefaultDownAction, VyJerkspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)VYJerkMenu},
    {JERK_ITEM_Z_NAME_ID,       DefaultOKAction, DefaultUpAction, DefaultDownAction, VzJerkspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)VZJerkMenu},
    {JERK_ITEM_E_NAME_ID,       DefaultOKAction, DefaultUpAction, DefaultDownAction, VeJerkspecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)VEJerkMenu},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};
/*===============================================================================
 *                              Steps/mm Contents start
 * ==============================================================================*/
const static struct MenuItem_t XstepsMenu[] = {
    {STEPS_ITEM_X_NAME_ID,XstepsOKAction
                         ,XstepsDownAction
                         ,XstepsUpAction
                         ,XstepsMenuShow
                         , DEFAULT_HID_MASK
                         , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t YstepsMenu[] = {
    {STEPS_ITEM_Y_NAME_ID,YstepsOKAction
                         ,YstepsDownAction
                         ,YstepsUpAction
                         ,YstepsMenuShow
                         , DEFAULT_HID_MASK
                         , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};


const static struct MenuItem_t ZstepsMenu[] = {
    {STEPS_ITEM_Z_NAME_ID,ZstepsOKAction
                         ,ZstepsDownAction
                         ,ZstepsUpAction
                         ,ZstepsMenuShow
                         , DEFAULT_HID_MASK
                         , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t EstepsMenu[] = {
    {STEPS_ITEM_E_NAME_ID,EstepsOKAction
                         ,EstepsDownAction
                         ,EstepsUpAction
                         ,EstepsMenuShow
                         , DEFAULT_HID_MASK
                         , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t StepsMenu[] =
{
    {ADVANCED_SETTING_NAME_ID, NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {STEPS_ITEM_X_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction,XstepsSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)XstepsMenu},
    {STEPS_ITEM_Y_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction,YstepsSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)YstepsMenu},
    {STEPS_ITEM_Z_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction,ZstepsSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)ZstepsMenu},
    {STEPS_ITEM_E_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction,EstepsSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)EstepsMenu},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};
/*===============================================================================
 *                              Motion Contents start
 * ==============================================================================*/
const static struct MenuItem_t MotionMenu[] =
{
    {MAINMENU_ITEM_MAIN_NAME_ID,       NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {MOTION_ITEM_VELOCITY_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)VelocityMenu},
    {MOTION_ITEM_ACCELERATION_NAME_ID, DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)AccelerationMenu},
    {MOTION_ITEM_JERK_NAME_ID,         DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)JerkMenu},
    {MOTION_ITEM_STEPS_NAME_ID,        DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)StepsMenu},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

/*===============================================================================
 *                              Filament Contents start
 * ==============================================================================*/
const static struct MenuItem_t FilMenu[] =
{
    {FILAMENT_ITEM_FIL_DIA_NAME_ID,FilamentOKAction,
                                   FilamentDownAction,
                                   FilamentUpAction,
                                   FilamentMenuShow
                                   , DEFAULT_HID_MASK
                                   , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t FilamentMenu[] =
{
    {ADVANCED_SETTING_NAME_ID,        NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {FILAMENT_ITEM_E_IN_MMM_NAME_ID,  FilamentMenuOKAction, DefaultUpAction, DefaultDownAction, FilamentMenuDisplay, DEFAULT_HID_MASK, NULL},
    {FILAMENT_ITEM_FIL_DIA_NAME_ID,   DefaultOKAction, DefaultUpAction, DefaultDownAction, FilamentDisplay, FILAMENT_ITEM_FIL_DIA_HID_MASK, (struct MenuItem_t*)FilMenu},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

/*===============================================================================
 *                              Control Contents start
 * ==============================================================================*/
const static struct MenuItem_t ControlMenu[] =
{
    {MAINMENU_ITEM_MAIN_NAME_ID,           NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},//next和okaction同时为空表示回退到上一级菜单
    {CONTROL_ITEM_TEMPERATURE_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)TempControl},
    {CONTROL_ITEM_MOTION_NAME_ID,          DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)MotionMenu },
//  {(S8 *)"BL Touch",        DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, TempControl},
//  {(S8 *)"Bed Leveling",    DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, TempControl},
    {CONTROL_ITEM_FILAMENT_NAME_ID,        DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)FilamentMenu},
    {CONTROL_ITEM_STORE_SETTING_NAME_ID,   ParamSaveOkAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {CONTROL_ITEM_LOAD_SETTING_NAME_ID,    ParamLoadOkAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MAINMENU_ITEM_LOAD_PARAM_HID_MASK, NULL},
    {CONTROL_ITEM_RESTORE_FAILSAFE_NAME_ID,ParamRecoverOkAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MAINMENU_ITEM_RECOVER_PARAM_HID_MASK, NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t MoveControl[] =
{
    {CONTROL_ITEM_MOTION_NAME_ID,          NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL}, //next和okaction同时为空表示回退到上一级菜单
    {MOVE_AXIS_ITEM_MOVE_X_AXIS_NAME_ID,   DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)X_AxisPosMenu},
    {MOVE_AXIS_ITEM_MOVE_Y_AXIS_NAME_ID,   DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)Y_AxisPosMenu},
    {MOVE_AXIS_ITEM_MOVE_Z_AXIS_NAME_ID,   DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)Z_AxisPosMenu},
    {MOVE_AXIS_ITEM_EXTRUDER_NAME_ID,      DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)ExtruderPosMenu},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t FileList[] =
{
    {FILELIST_ITEM_NAME_ID, SdCardFileListOKAction,
                            SdCardFileListUpAction,
                            SdCardFileListDownAction,
                            SdCardFileListShowAction,DEFAULT_HID_MASK,  NULL},//next和okaction同时为空表示回退到上一级菜单
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t PreheatPLA[] =
{
    {CONTROL_ITEM_TEMPERATURE_NAME_ID, NULL,DefaultUpAction,DefaultDownAction,DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {PREHEATPLA_ITEM_PLA_NAME_ID,      PreheatPLAMenuItemOKAction,DefaultUpAction,DefaultDownAction,DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {PREHEATPLA_ITEM_PLA_END_NAME_ID,  PreheatPLANozMenuItemOKAction,DefaultUpAction,DefaultDownAction,DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {PREHEATPLA_ITEM_PLA_BED_NAME_ID,  PreheatPLABedMenuItemOKAction,DefaultUpAction,DefaultDownAction,DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t PreheatABS[] =
{
    {CONTROL_ITEM_TEMPERATURE_NAME_ID, NULL,DefaultUpAction,DefaultDownAction,DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {PREHEATABS_ITEM_ABS_NAME_ID,      PreheatABSMenuItemOKAction,DefaultUpAction,DefaultDownAction,DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {PREHEATABS_ITEM_ABS_END_NAME_ID,  PreheatABSNozMenuItemOKAction,DefaultUpAction,DefaultDownAction,DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {PREHEATABS_ITEM_ABS_BED_NAME_ID,  PreheatABSBedMenuItemOKAction,DefaultUpAction,DefaultDownAction,DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t PrepareMenu[] =
{
    {MAINMENU_ITEM_MAIN_NAME_ID,            NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL}, //next和okaction同时为空表示回退到上一级菜单
    {PREPARE_ITEM_MOVE_AXIS_NAME_ID,        DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)MoveControl},
    {PREPARE_ITEM_AUTO_HOME_NAME_ID,        GoHomeMenuItemOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {PREPARE_ITEM_SET_HOME_OFFSET_NAME_ID,  HomeOffsetMenuOkAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {PREPARE_ITEM_DISABLE_STEPPERS_NAME_ID, DisableStepperMenuItemOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {PREPARE_ITEM_PREHEAT_PLA_NAME_ID,      DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)PreheatPLA},
    {PREPARE_ITEM_PREHEAT_ABS_NAME_ID,      DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)PreheatABS},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t PrinterInfoMenu[] =
{
    {MAINMENU_ITEM_ABOUT_PRINTF_NAME_ID,    AboutPrinterOkAction, AboutPrinterUpAction, AboutPrinterDownAction, AboutPrinterMenuShow, DEFAULT_HID_MASK, NULL},//next和okaction同时为空表示回退到上一级菜单
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项 (测试字符串的开始值是结束符直接用==判断)
};

/*===============================================================================
 *                              Tune Contents start
 * ==============================================================================*/

const static struct MenuItem_t FrSpeedMenu[] =
{
    {TUNE_ITEM_SPEED_NAME_ID, PrintfSpeedMenuOKAction
                            , PrintfSpeedMenuDownAction
                            , PrintfSpeedMenuUPAction
                            , PrintfSpeedMenuShow
                            , DEFAULT_HID_MASK
                            , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t FlowMenu[] =
{
    {TUNE_ITEM_FLOW_NAME_ID, FlowMenuOKAction
                           , FlowMenuDownAction
                           , FlowMenuUPAction
                           , FlowMenuShow
                           , DEFAULT_HID_MASK
                           , NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t TuneControl[] =
{
    {MAINMENU_ITEM_MAIN_NAME_ID,     NULL, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL}, //next和okaction同时为空表示回退到上一级菜单
    {TUNE_ITEM_SPEED_NAME_ID,        DefaultOKAction, DefaultUpAction, DefaultDownAction, PrintfSpeedSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)FrSpeedMenu},
    {TUNE_ITEM_NOZZLE_NAME_ID,       DefaultOKAction, DefaultUpAction, DefaultDownAction, NozzleTempSpecialDisplay, DEFAULT_HID_MASK,(struct MenuItem_t*)NozzleTempMenu},
    {TUNE_ITEM_BED_NAME_ID,          DefaultOKAction, DefaultUpAction, DefaultDownAction, BedTempSpecialDisplay, DEFAULT_HID_MASK,(struct MenuItem_t*)BedTempMenu},
    {TUNE_ITEM_FANSPEED_NAME_ID,     DefaultOKAction, DefaultUpAction, DefaultDownAction, FanSpeedSpecialDisplay, DEFAULT_HID_MASK,(struct MenuItem_t*)FansSpeedMenu},
    {TUNE_ITEM_FLOW_NAME_ID,         DefaultOKAction, DefaultUpAction, DefaultDownAction, FlowSpecialDisplay, DEFAULT_HID_MASK, (struct MenuItem_t*)FlowMenu},
    {TUNE_ITEM_BABYSTEPS_Z_NAME_ID,  DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

const static struct MenuItem_t MainMenu[] =
{
    {MAINMENU_ITEM_INFO_NAME_ID,          NULL, NULL, DefaultDownAction ,DefaultShowMenuItem, DEFAULT_HID_MASK, NULL},  //next和okaction同时为空表示回退到上一级菜单
    {MAINMENU_ITEM_PREPARE_NAME_ID,       DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MAINMENU_ITEM_PREPARE_HID_MASK, (struct MenuItem_t*)PrepareMenu}, //无法确定那个item高亮
    {MAINMENU_ITEM_TUNE_NAME_ID,          DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MAINMENU_ITEM_TUNE_HID_MASK, (struct MenuItem_t*)TuneControl},
    {MAINMENU_ITEM_CONTROL_NAME_ID,       DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)ControlMenu},
    {MAINMENU_ITEM_NOTFCARD_NAME_ID,      DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MAINMENU_ITEM_NOTFCARD_HID_MASK, NULL},
    {MAINMENU_ITEM_PRINTFFROMTF_NAME_ID,  DefaultOKAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MAINMENU_ITEM_PRINTFFROMTF_HID_MASK, (struct MenuItem_t*)FileList},
    {MAINMENU_ITEM_PAUSE_PRINTF_NAME_ID,  PausePrintAction,DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK, NULL},
    {MAINMENU_ITEM_RESUME_PRINTF_NAME_ID, ContinuePrintAction,DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MAINMENU_ITEM_RESUME_PRINTF_HID_MASK, NULL},
    {MAINMENU_ITEM_STOP_PRINTF_NAME_ID,   StopPrintAction, DefaultUpAction, DefaultDownAction, DefaultShowMenuItem, MAINMENU_ITEM_STOP_PRINTF_HID_MASK, NULL},
    {MAINMENU_ITEM_ABOUT_PRINTF_NAME_ID,  DefaultOKAction, DefaultUpAction, NULL, DefaultShowMenuItem, DEFAULT_HID_MASK, (struct MenuItem_t*)PrinterInfoMenu},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};
#endif
//子菜单在下一级
const static struct MenuItem_t StartMenu[] =
{
    {ROOTMENU_ITEM_ROOT_NAME_ID, DefaultOKAction, NULL, NULL, ShowRoot, DEFAULT_HID_MASK, (struct MenuItem_t*)MainMenu},
//    {ROOTMENU_ITEM_ROOT_NAME_ID, DefaultOKAction, NULL, NULL, ShowRoot, DEFAULT_HID_MASK, NULL},
    {MENU_ITEM_FINISH, NULL, NULL, NULL, NULL, DEFAULT_HID_MASK, NULL}  //数组结束菜单子项
};

struct MenuItem_t *RootMenu = (struct MenuItem_t *)StartMenu; //根菜单数组的地址.
/***************************************配置菜单部分end*****************************************************/

VOID CrShowViewInit()
{
    (HideMask |= MAINMENU_ITEM_TUNE_HID_MASK);
    (HideMask |= MAINMENU_ITEM_PRINTFFROMTF_HID_MASK);
    (HideMask |= MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK);
    (HideMask |= MAINMENU_ITEM_RESUME_PRINTF_HID_MASK);
    (HideMask |= MAINMENU_ITEM_STOP_PRINTF_HID_MASK);
    (HideMask |= FILAMENT_ITEM_FIL_DIA_HID_MASK);
    (HideMask |= MOVEAXISEMNU_ITEM_AXISE_MAXUNIT_HID_MAKS);
    (HideMask |= MOVEAXISEMNU_ITEM_AXISE_MIDUNIT_HID_MAKS);
    (HideMask |= MOVEAXISEMNU_ITEM_AXISE_MINUNIT_HID_MAKS);

}
