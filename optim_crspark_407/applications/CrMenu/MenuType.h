
#ifndef _MENU_TYPE_H
#define _MENU_TYPE_H

#include <stdio.h>
#include "CrInc/CrType.h"


#define MENU_ITEM_NAME_LENGHTH  50                       //菜单名限长

#define BACK_PARENT  1      //返回父菜单 (在OK Action的输出中给出是否返回父菜单或根菜单)
#define BACK_ROOT    2      //返回根菜单 (在OK Action的输出中给出是否返回父菜单或根菜单)

enum MenuNameId_e
{
    ROOTMENU_ITEM_ROOT_NAME_ID = 0,
    MAINMENU_ITEM_MAIN_NAME_ID,
    MAINMENU_ITEM_INFO_NAME_ID,
    MAINMENU_ITEM_PREPARE_NAME_ID,
    MAINMENU_ITEM_TUNE_NAME_ID,
    MAINMENU_ITEM_CONTROL_NAME_ID,
    MAINMENU_ITEM_NOTFCARD_NAME_ID,
    MAINMENU_ITEM_PRINTFFROMTF_NAME_ID,
    MAINMENU_ITEM_PAUSE_PRINTF_NAME_ID,
    MAINMENU_ITEM_RESUME_PRINTF_NAME_ID,
    MAINMENU_ITEM_STOP_PRINTF_NAME_ID,
    MAINMENU_ITEM_ABOUT_PRINTF_NAME_ID,

    PREPARE_ITEM_MOVE_AXIS_NAME_ID,
    PREPARE_ITEM_AUTO_HOME_NAME_ID,
    PREPARE_ITEM_SET_HOME_OFFSET_NAME_ID,
    PREPARE_ITEM_DISABLE_STEPPERS_NAME_ID,
    PREPARE_ITEM_PREHEAT_PLA_NAME_ID,
    PREPARE_ITEM_PREHEAT_ABS_NAME_ID,

    TUNE_ITEM_SPEED_NAME_ID,
    TUNE_ITEM_NOZZLE_NAME_ID,
    TUNE_ITEM_BED_NAME_ID,
    TUNE_ITEM_FANSPEED_NAME_ID,
    TUNE_ITEM_FLOW_NAME_ID,
    TUNE_ITEM_BABYSTEPS_Z_NAME_ID,

    CONTROL_ITEM_TEMPERATURE_NAME_ID,
    CONTROL_ITEM_MOTION_NAME_ID,
    CONTROL_ITEM_FILAMENT_NAME_ID,
    CONTROL_ITEM_STORE_SETTING_NAME_ID,
    CONTROL_ITEM_LOAD_SETTING_NAME_ID,
    CONTROL_ITEM_RESTORE_FAILSAFE_NAME_ID,

    MOVE_AXIS_ITEM_MOVE_X_AXIS_NAME_ID,
    MOVE_AXIS_ITEM_MOVE_Y_AXIS_NAME_ID,
    MOVE_AXIS_ITEM_MOVE_Z_AXIS_NAME_ID,
    MOVE_AXIS_ITEM_EXTRUDER_NAME_ID,
    MOVE_AXIS_ITEM_TOOCOLD_NAME_ID,

    MOVE_AXIS_ITEM_X_AXIS_NAME_ID,
    MOVE_AXIS_ITEM_Y_AXIS_NAME_ID,
    MOVE_AXIS_ITEM_Z_AXIS_NAME_ID,

    MOVE_AXIS_ITEM_MOVE_10_MM_NAME_ID,
    MOVE_AXIS_ITEM_MOVE_1_MM_NAME_ID,
    MOVE_AXIS_ITEM_MOVE_0_1_MM_NAME_ID,
    MOVE_AXIS_ITEM_MOVE_0_125_MM_NAME_ID,

    MOVE_AXIS_ITEM_MOVE_MAX_UNIT_NAME_ID,
    MOVE_AXIS_ITEM_MOVE_MIN_UNIT_NAME_ID,
    MOVE_AXIS_ITEM_MOVE_MID_UNIT_NAME_ID,

    STEPS_ITEM_X_NAME_ID,
    STEPS_ITEM_Y_NAME_ID,
    STEPS_ITEM_Z_NAME_ID,
    STEPS_ITEM_E_NAME_ID,

    JERK_ITEM_X_NAME_ID,
    JERK_ITEM_Y_NAME_ID,
    JERK_ITEM_Z_NAME_ID,
    JERK_ITEM_E_NAME_ID,

    PREHEATPLA_ITEM_PLA_NAME_ID,
    PREHEATPLA_ITEM_PLA_END_NAME_ID,
    PREHEATPLA_ITEM_PLA_BED_NAME_ID,

    PREHEATABS_ITEM_ABS_NAME_ID,
    PREHEATABS_ITEM_ABS_END_NAME_ID,
    PREHEATABS_ITEM_ABS_BED_NAME_ID,

    TEMPERATURE_PREHEAT_PLA_CONF_NAME_ID,
    TEMPERATURE_PREHEAT_ABS_CONF_NAME_ID,

    MOTION_ITEM_VELOCITY_NAME_ID,
    MOTION_ITEM_ACCELERATION_NAME_ID,
    MOTION_ITEM_JERK_NAME_ID,
    MOTION_ITEM_STEPS_NAME_ID,

    ADVANCED_SETTING_NAME_ID,
    CONSIGURATION_NAME_ID,

    FILAMENT_ITEM_E_IN_MMM_NAME_ID,
    FILAMENT_ITEM_FIL_DIA_NAME_ID,

    FILELIST_ITEM_NAME_ID,

    ACCELERATION_ITEM_ACCEL_NAME_ID,
    ACCELERATION_ITEM_A_RETRACT_NAME_ID,
    ACCELERATION_ITEM_A_TRAVEL_NAME_ID,
    ACCELERATION_ITEM_AMAX_X_NAME_ID,
    ACCELERATION_ITEM_AMAX_Y_NAME_ID,
    ACCELERATION_ITEM_AMAX_Z_NAME_ID,
    ACCELERATION_ITEM_AMAX_E_NAME_ID,

    VELOCITY_ITEM_VMAX_X_NAME_ID,
    VELOCITY_ITEM_VMAX_Y_NAME_ID,
    VELOCITY_ITEM_VMAX_Z_NAME_ID,
    VELOCITY_ITEM_VMAX_E_NAME_ID,
    VELOCITY_ITEM_VMIN_NAME_ID,
    VELOCITY_ITEM_VTRAV_MIN_NAME_ID,

    ON_NAME_ID,
    OFF_NAME_ID,

    READY_NAME_ID,
    STOP_NAME_ID,
    PAUSE_NAME_ID,

    BED_HEARTING_NAME_ID,
    NOZ_HEARTING_NAME_ID,

    SD_CARD_INSERT_NAME_ID,
    SD_CARD_REMOVE_NAME_ID,

    MENU_ITEM_FINISH = 1000,
};

//在其他位置保存每一个数组的当前菜单项
struct MenuItemAttr_t
{
    enum MenuNameId_e Id;
    S8 *Name;
};
//typedef struct _MenuItem_t   //在不同的系统中typedef存在兼容性问题
struct MenuItem_t
{
    enum MenuNameId_e ItemNameId;
    VOID* (*OkAction)(VOID *ArgOut, VOID *ArgIn);         //大部分item跳转至Next,少数执行命令，如移动电机等
    VOID* (*UpAction)(VOID *ArgOut, VOID *ArgIn);
    VOID* (*DownAction)(VOID *ArgOut, VOID *ArgIn);
    S32 (*Show)(VOID *Arg);
    S32  HideMask;                   //隐藏属性
    struct MenuItem_t *Next;         //是child，(是father时此处为空，且OkAction也为空)
};


enum MenuType
{
    NormalTypeMenu = 0, //普通类型菜单是不含子菜单只显示自己名称
    RollBackTypeMenu,   //回退类型菜单
    FatherTypeMenu,     //含有子菜单的菜单
    SpecialTypeMenu     //特殊菜单
};

struct MenuShowInfo_t
{
    VOID    *ShowData;   //显示内容指针(主要指名字)
    S16     Index;      //索引(行号)
    S8      Highligh;   //1高亮， 0普通;被选中的高亮显示
    S8      MenuType;   //0是不含子菜单只显示自己名称， 1是回溯菜单， 2是含有子菜单，3 直接显示自己特有的内容
};

#endif

