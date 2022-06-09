/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-25     cx2470       the first version
 */
#ifdef LCD_LANGUAGE_CN
#ifndef _LANGUAGECN_H_
#define _LANGUAGECN_H_
#include "CrInc/CrConfig.h"

#define     ROOTMENU_ITEM_ROOT_NAME_STRING_CN               "开始"
#define     MAINMENU_ITEM_MAIN_NAME_STRING_CN               "主界面"
#define     MAINMENU_ITEM_INFO_NAME_STRING_CN               "信息屏"
#define     MAINMENU_ITEM_PREPARE_NAME_STRING_CN            "准备"
#define     MAINMENU_ITEM_TUNE_NAME_STRING_CN               "调整"
#define     MAINMENU_ITEM_CONTROL_NAME_STRING_CN            "控制"
#define     MAINMENU_ITEM_NOTFCARD_NAME_STRING_CN           "无存储卡"
#define     MAINMENU_ITEM_PRINTFFROMTF_NAME_STRING_CN       "从存储卡打印"
#define     MAINMENU_ITEM_PAUSE_PRINTF_NAME_STRING_CN       "暂停打印"
#define     MAINMENU_ITEM_RESUME_PRINTF_NAME_STRING_CN      "恢复打印"
#define     MAINMENU_ITEM_STOP_PRINTF_NAME_STRING_CN        "停止打印"
#define     MAINMENU_ITEM_ABOUT_PRINTF_NAME_STRING_CN       "关于打印机"

#define     PREPARE_ITEM_MOVE_AXIS_NAME_STRING_CN           "移动轴"
#define     PREPARE_ITEM_AUTO_HOME_NAME_STRING_CN           "回原点"
#define     PREPARE_ITEM_SET_HOME_OFFSET_NAME_STRING_CN     "设置原点偏移"
#define     PREPARE_ITEM_DISABLE_STEPPERS_NAME_STRING_CN    "关闭步进电机"
#define     PREPARE_ITEM_PREHEAT_PLA_NAME_STRING_CN         "预热 PLA"
#define     PREPARE_ITEM_PREHEAT_ABS_NAME_STRING_CN         "预热 ABS"

#define     TUNE_ITEM_SPEED_NAME_STRING_CN                  "速率:"
#define     TUNE_ITEM_NOZZLE_NAME_STRING_CN                 "喷嘴:"
#define     TUNE_ITEM_BED_NAME_STRING_CN                    "热床:"
#define     TUNE_ITEM_FANSPEED_NAME_STRING_CN               "风扇速率:"
#define     TUNE_ITEM_FLOW_NAME_STRING_CN                   "Flow"
#define     TUNE_ITEM_BABYSTEPS_Z_NAME_STRING_CN            "微量调整Z轴:"

#define     CONTROL_ITEM_TEMPERATURE_NAME_STRING_CN         "温度"
#define     CONTROL_ITEM_MOTION_NAME_STRING_CN              "运动"
#define     CONTROL_ITEM_FILAMENT_NAME_STRING_CN            "丝料测容"
#define     CONTROL_ITEM_STORE_SETTING_NAME_STRING_CN       "保存设置"
#define     CONTROL_ITEM_LOAD_SETTING_NAME_STRING_CN        "装载设置"
#define     CONTROL_ITEM_RESTORE_FAILSAFE_NAME_STRING_CN    "恢复安全值"

#define     MOVE_AXIS_ITEM_MOVE_X_AXIS_NAME_STRING_CN       "移动X"
#define     MOVE_AXIS_ITEM_MOVE_Y_AXIS_NAME_STRING_CN       "移动Y"
#define     MOVE_AXIS_ITEM_MOVE_Z_AXIS_NAME_STRING_CN       "移动Z"
#define     MOVE_AXIS_ITEM_EXTRUDER_NAME_STRING_CN          "挤出机"

#define     MOVE_AXIS_ITEM_X_AXIS_NAME_STRING_CN            "X 轴"
#define     MOVE_AXIS_ITEM_Y_AXIS_NAME_STRING_CN            "Y 轴"
#define     MOVE_AXIS_ITEM_Z_AXIS_NAME_STRING_CN            "Z 轴"
#define     MOVE_AXIS_ITEM_TOO_COLD_NAME_STRING_CN          "HotEnd Too Cold"
#define     MOVE_AXIS_ITEM_MOVE_10_MM_NAME_STRING_CN        "移动 10 mm "
#define     MOVE_AXIS_ITEM_MOVE_1_MM_NAME_STRING_CN         "移动 1 mm"
#define     MOVE_AXIS_ITEM_MOVE_0_1_MM_NAME_STRING_CN       "移动 0.1 mm"
#define     MOVE_AXIS_ITEM_MOVE_0_125_MM_NAME_STRING_CN     "移动 0.025 mm"

#define     MOVE_AXIS_ITEM_MOVE_MAX_UNIT_NAME_STRING_CN     "Max Unit"
#define     MOVE_AXIS_ITEM_MOVE_MIN_UNIT_NAME_STRING_CN     "Min Unit"
#define     MOVE_AXIS_ITEM_MOVE_MID_UNIT_NAME_STRING_CN     "Mid Unit"

#define     STEPS_ITEM_X_NAME_STRING_CN                     "X轴步数/mm:"
#define     STEPS_ITEM_Y_NAME_STRING_CN                     "Y轴步数/mm:"
#define     STEPS_ITEM_Z_NAME_STRING_CN                     "Z轴步数/mm:"
#define     STEPS_ITEM_E_NAME_STRING_CN                     "挤出机步数/mm:"

#define     JERK_ITEM_X_NAME_STRING_CN                      "轴抖动速率X:"
#define     JERK_ITEM_Y_NAME_STRING_CN                      "轴抖动速率Y:"
#define     JERK_ITEM_Z_NAME_STRING_CN                      "轴抖动速率Z:"
#define     JERK_ITEM_E_NAME_STRING_CN                      "挤出机抖动速率:"

#define     PREHEATPLA_ITEM_PLA_NAME_STRING_CN              "预热 PLA"
#define     PREHEATPLA_ITEM_PLA_END_NAME_STRING_CN          "预热 PLA 喷嘴"
#define     PREHEATPLA_ITEM_PLA_BED_NAME_STRING_CN          "预热 PLA 热床"

#define     PREHEATABS_ITEM_ABS_NAME_STRING_CN              "预热 ABS"
#define     PREHEATABS_ITEM_ABS_END_NAME_STRING_CN          "预热 ABS 喷嘴"
#define     PREHEATABS_ITEM_ABS_BED_NAME_STRING_CN          "预热 ABS 热床"

#define     TEMPERATURE_PREHEAT_PLA_CONF_NAME_STRING_CN     "预热 PLA 设置"
#define     TEMPERATURE_PREHEAT_ABS_CONF_NAME_STRING_CN     "预热 ABS 设置"

#define     MOTION_ITEM_VELOCITY_NAME_STRING_CN             "速度"
#define     MOTION_ITEM_ACCELERATION_NAME_STRING_CN         "加速度"
#define     MOTION_ITEM_JERK_NAME_STRING_CN                 "抖动速率"
#define     MOTION_ITEM_STEPS_NAME_STRING_CN                "轴步数/mm"

#define     ADVANCED_SETTING_NAME_STRING_CN                 "Advanced Settings"
#define     CONSIGURATION_NAME_STRING_CN                    "Configuration"

#define     FILAMENT_ITEM_E_IN_MMM_NAME_STRING_CN           "测容积mm³:"
#define     FILAMENT_ITEM_FIL_DIA_NAME_STRING_CN            "丝料直径:"

#define     FILELIST_ITEM_NAME_STRING_CN                    "Content"

#define     ACCELERATION_ITEM_ACCEL_NAME_STRING_CN          "加速度:"
#define     ACCELERATION_ITEM_A_RETRACT_NAME_STRING_CN      "收进加速度:"
#define     ACCELERATION_ITEM_A_TRAVEL_NAME_STRING_CN       "非打印移动加速度:"
#define     ACCELERATION_ITEM_AMAX_X_NAME_STRING_CN         "最大打印加速度X:"
#define     ACCELERATION_ITEM_AMAX_Y_NAME_STRING_CN         "最大打印加速度Y:"
#define     ACCELERATION_ITEM_AMAX_Z_NAME_STRING_CN         "最大打印加速度Z:"
#define     ACCELERATION_ITEM_AMAX_E_NAME_STRING_CN         "最大打印加速度E:"

#define     VELOCITY_ITEM_VMAX_X_NAME_STRING_CN             "最大速率 X:"
#define     VELOCITY_ITEM_VMAX_Y_NAME_STRING_CN             "最大速率 Y:"
#define     VELOCITY_ITEM_VMAX_Z_NAME_STRING_CN             "最大速率 Z:"
#define     VELOCITY_ITEM_VMAX_E_NAME_STRING_CN             "最大速率 E:"
#define     VELOCITY_ITEM_VMIN_NAME_STRING_CN               "最小速率:"
#define     VELOCITY_ITEM_VTRAV_MIN_NAME_STRING_CN          "最小移动速率:"

#define     ON_NAME_STRING_CN                               "开"
#define     OFF_NAME_STRING_CN                              "关"

#define     READY_STRING_CN                                 "已就绪."
#define     STOP_STRING_CN                                  "停止打印"
#define     PAUSE_STRING_CN                                 "暂停打印"

#define     BED_HEATING_CN                                  "热床加热中"
#define     NOZ_HEATING_CN                                  "喷嘴加热中"

#define     SD_CARD_INSERTED_CN                             "存储卡已插入"
#define     SD_CARD_PULL_OUT_CN                             "存储卡被拔出"

extern const struct MenuItemAttr_t NameIdMap[];
#endif /* APPLICATIONS_CRMENU_LANGUAGE_LANGUAGECN_H_ */
#endif
