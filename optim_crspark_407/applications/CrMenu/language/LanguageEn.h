/*
 * Copyright (c) 2006-2020  RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-25     cx2470       the first version
 */
#ifdef LCD_LANGUAGE_EN
#ifndef _LANUAGEEN_H_
#define _LANUAGEEN_H_
#include "CrInc/CrConfig.h"

#define     ROOTMENU_ITEM_ROOT_NAME_STRING_EN               "Start"
#define     MAINMENU_ITEM_MAIN_NAME_STRING_EN               "Main"
#define     MAINMENU_ITEM_INFO_NAME_STRING_EN               "Info Screen"
#define     MAINMENU_ITEM_PREPARE_NAME_STRING_EN            "Prepare"
#define     MAINMENU_ITEM_TUNE_NAME_STRING_EN               "Tune"
#define     MAINMENU_ITEM_CONTROL_NAME_STRING_EN            "Control"
#define     MAINMENU_ITEM_NOTFCARD_NAME_STRING_EN           "No TF Card"
#define     MAINMENU_ITEM_PRINTFFROMTF_NAME_STRING_EN       "PrintFromTF"
#define     MAINMENU_ITEM_PAUSE_PRINTF_NAME_STRING_EN       "Pause Print"
#define     MAINMENU_ITEM_RESUME_PRINTF_NAME_STRING_EN      "Resume Print"
#define     MAINMENU_ITEM_STOP_PRINTF_NAME_STRING_EN        "Stop Print"
#define     MAINMENU_ITEM_ABOUT_PRINTF_NAME_STRING_EN       "About Printer"

#define     PREPARE_ITEM_MOVE_AXIS_NAME_STRING_EN           "Move Axis"
#define     PREPARE_ITEM_AUTO_HOME_NAME_STRING_EN           "Auto Home"
#define     PREPARE_ITEM_SET_HOME_OFFSET_NAME_STRING_EN     "Set Home Offsets"
#define     PREPARE_ITEM_DISABLE_STEPPERS_NAME_STRING_EN    "Disable Steppers"
#define     PREPARE_ITEM_PREHEAT_PLA_NAME_STRING_EN         "Preheat PLA"
#define     PREPARE_ITEM_PREHEAT_ABS_NAME_STRING_EN         "Preheat ABS"

#define     TUNE_ITEM_SPEED_NAME_STRING_EN                  "Speed:"
#define     TUNE_ITEM_NOZZLE_NAME_STRING_EN                 "Nozzle:"
#define     TUNE_ITEM_BED_NAME_STRING_EN                    "Bed:"
#define     TUNE_ITEM_FANSPEED_NAME_STRING_EN               "Fan Speed:"
#define     TUNE_ITEM_FLOW_NAME_STRING_EN                   "Flow:"
#define     TUNE_ITEM_BABYSTEPS_Z_NAME_STRING_EN            "Babysteps Z:"

#define     CONTROL_ITEM_TEMPERATURE_NAME_STRING_EN         "Temperature"
#define     CONTROL_ITEM_MOTION_NAME_STRING_EN              "Motion"
#define     CONTROL_ITEM_FILAMENT_NAME_STRING_EN            "Filament"
#define     CONTROL_ITEM_STORE_SETTING_NAME_STRING_EN       "Store Settings"
#define     CONTROL_ITEM_LOAD_SETTING_NAME_STRING_EN        "Load Settings"
#define     CONTROL_ITEM_RESTORE_FAILSAFE_NAME_STRING_EN    "Restore failsafe"

#define     MOVE_AXIS_ITEM_MOVE_X_AXIS_NAME_STRING_EN       "Move X Axis"
#define     MOVE_AXIS_ITEM_MOVE_Y_AXIS_NAME_STRING_EN       "Move Y Axis"
#define     MOVE_AXIS_ITEM_MOVE_Z_AXIS_NAME_STRING_EN       "Move Z Axis"
#define     MOVE_AXIS_ITEM_EXTRUDER_NAME_STRING_EN          "Extruder"
#define     MOVE_AXIS_ITEM_TOO_COLD_NAME_STRING_EN          "Hotend Too Cold"

#define     MOVE_AXIS_ITEM_X_AXIS_NAME_STRING_EN            "X Axis"
#define     MOVE_AXIS_ITEM_Y_AXIS_NAME_STRING_EN            "Y Axis"
#define     MOVE_AXIS_ITEM_Z_AXIS_NAME_STRING_EN            "Z Axis"

#define     MOVE_AXIS_ITEM_MOVE_10_MM_NAME_STRING_EN        "Move 10(mm) "
#define     MOVE_AXIS_ITEM_MOVE_1_MM_NAME_STRING_EN         "Move 1(mm)"
#define     MOVE_AXIS_ITEM_MOVE_0_1_MM_NAME_STRING_EN       "Move 0.1(mm)"
#define     MOVE_AXIS_ITEM_MOVE_0_125_MM_NAME_STRING_EN     "Move 0.025(mm)"

#define     MOVE_AXIS_ITEM_MOVE_MAX_UNIT_NAME_STRING_EN     "Max Unit"
#define     MOVE_AXIS_ITEM_MOVE_MIN_UNIT_NAME_STRING_EN     "Min Unit"
#define     MOVE_AXIS_ITEM_MOVE_MID_UNIT_NAME_STRING_EN     "Mid Unit"

#define     STEPS_ITEM_X_NAME_STRING_EN                     "Xsteps/mm:"
#define     STEPS_ITEM_Y_NAME_STRING_EN                     "Ysteps/mm:"
#define     STEPS_ITEM_Z_NAME_STRING_EN                     "Zsteps/mm:"
#define     STEPS_ITEM_E_NAME_STRING_EN                     "Esteps/mm:"

#define     JERK_ITEM_X_NAME_STRING_EN                      "VX-Jerk:"
#define     JERK_ITEM_Y_NAME_STRING_EN                      "VY-Jerk:"
#define     JERK_ITEM_Z_NAME_STRING_EN                      "VZ-Jerk:"
#define     JERK_ITEM_E_NAME_STRING_EN                      "VE-Jerk:"

#define     PREHEATPLA_ITEM_PLA_NAME_STRING_EN              "Preheat PLA"
#define     PREHEATPLA_ITEM_PLA_END_NAME_STRING_EN          "Preheat PLA End"
#define     PREHEATPLA_ITEM_PLA_BED_NAME_STRING_EN          "Preheat PLA Bed"

#define     PREHEATABS_ITEM_ABS_NAME_STRING_EN              "Preheat ABS"
#define     PREHEATABS_ITEM_ABS_END_NAME_STRING_EN          "Preheat ABS End"
#define     PREHEATABS_ITEM_ABS_BED_NAME_STRING_EN          "Preheat ABS Bed"

#define     TEMPERATURE_PREHEAT_PLA_CONF_NAME_STRING_EN     "Preheat PLA Conf"
#define     TEMPERATURE_PREHEAT_ABS_CONF_NAME_STRING_EN     "Preheat ABS Conf"

#define     MOTION_ITEM_VELOCITY_NAME_STRING_EN             "Velocity"
#define     MOTION_ITEM_ACCELERATION_NAME_STRING_EN         "Acceleration"
#define     MOTION_ITEM_JERK_NAME_STRING_EN                 "Jerk"
#define     MOTION_ITEM_STEPS_NAME_STRING_EN                "Steps/mm"

#define     ADVANCED_SETTING_NAME_STRING_EN                 "Advanced Settings"
#define     CONSIGURATION_NAME_STRING_EN                    "Configuration"

#define     FILAMENT_ITEM_E_IN_MMM_NAME_STRING_EN           "E in mm³:"
#define     FILAMENT_ITEM_FIL_DIA_NAME_STRING_EN            "Fil. Dia.:"

#define     FILELIST_ITEM_NAME_STRING_EN                    "Content"

#define     ACCELERATION_ITEM_ACCEL_NAME_STRING_EN          "Accel:"
#define     ACCELERATION_ITEM_A_RETRACT_NAME_STRING_EN      "A-Retract:"
#define     ACCELERATION_ITEM_A_TRAVEL_NAME_STRING_EN       "A-Travel:"
#define     ACCELERATION_ITEM_AMAX_X_NAME_STRING_EN         "Amax X:"
#define     ACCELERATION_ITEM_AMAX_Y_NAME_STRING_EN         "Amax Y:"
#define     ACCELERATION_ITEM_AMAX_Z_NAME_STRING_EN         "Amax Z:"
#define     ACCELERATION_ITEM_AMAX_E_NAME_STRING_EN         "Amax E:"

#define     VELOCITY_ITEM_VMAX_X_NAME_STRING_EN             "Vmax X:"
#define     VELOCITY_ITEM_VMAX_Y_NAME_STRING_EN             "Vmax Y:"
#define     VELOCITY_ITEM_VMAX_Z_NAME_STRING_EN             "Vmax Z:"
#define     VELOCITY_ITEM_VMAX_E_NAME_STRING_EN             "Vmax E:"
#define     VELOCITY_ITEM_VMIN_NAME_STRING_EN               "Vmin:"
#define     VELOCITY_ITEM_VTRAV_MIN_NAME_STRING_EN          "VTrav Min:"
#define     ON_NAME_STRING_EN                               "On"
#define     OFF_NAME_STRING_EN                              "Off"
#define     READY_STRING_EN                                 "Ready."
#define     STOP_STRING_EN                                  "Stop."
#define     PAUSE_STRING_EN                                 "Print Pause."

#define     BED_HEATING_EN                                  "Hot bed heating... "
#define     NOZ_HEATING_EN                                  "Nozzle heating ..."

#define     SD_CARD_INSERTED_EN                             "SD card inserted !"
#define     SD_CARD_PULL_OUT_EN                             "SD card removed  !"

extern const struct MenuItemAttr_t NameIdMap[];
#endif /* _LANUAGEEN_H_ */
#endif
