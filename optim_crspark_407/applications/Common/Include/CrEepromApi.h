#ifndef _CREEPROMAPI_H
#define _CREEPROMAPI_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"
#include "CrModel/CrHeaterManager.h"
#include "CrMotion/StepperApi.h"

#define MSG_EEPROM_RESET  "Settings restored to default"
#define MSG_EEPROM_RESTORED "Settings restored from backup"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_COORDINATE_SYSTEMS 10
#define UBL_VERSION "1.01"

#ifndef XY_SKEW_FACTOR
  #define XY_SKEW_FACTOR 0.0
#endif
#ifndef XZ_SKEW_FACTOR
  #define XZ_SKEW_FACTOR 0.0
#endif
#ifndef YZ_SKEW_FACTOR
  #define YZ_SKEW_FACTOR 0.0
#endif

#ifndef MESH_GRID_MAX_POINTS_X
#define MESH_GRID_MAX_POINTS_X 3
#endif

#ifndef MESH_GRID_MAX_POINTS_Y
#define MESH_GRID_MAX_POINTS_Y 3
#endif

#ifndef UBL_GRID_MAX_POINTS_X
#define UBL_GRID_MAX_POINTS_X 3
#endif

#ifndef UBL_GRID_MAX_POINTS_Y
#define UBL_GRID_MAX_POINTS_Y 3
#endif

#ifndef GRID_MAX_POINTS_X
#define GRID_MAX_POINTS_X 3
#endif

#ifndef GRID_MAX_POINTS_Y
#define GRID_MAX_POINTS_Y 3
#endif

#ifndef EEPROM_NUM_SERVOS
#define EEPROM_NUM_SERVOS 9
#endif

#define FWRETRACT      //回抽的默认参数
#ifdef FWRETRACT
  #define FWRETRACT_AUTORETRACT           // Override slicer retractions
  #ifdef FWRETRACT_AUTORETRACT
    #define MIN_AUTORETRACT 0.1           // (mm) Don't convert E moves under this length
    #define MAX_AUTORETRACT 10.0          // (mm) Don't convert E moves over this length
  #endif
  #define RETRACT_LENGTH 3                // (mm) Default retract length (positive value)
  #define RETRACT_LENGTH_SWAP 13          // (mm) Default swap retract length (positive value)
  #define RETRACT_FEEDRATE 45             // (mm/s) Default feedrate for retracting
  #define RETRACT_ZRAISE 0                // (mm) Default retract Z-raise
  #define RETRACT_RECOVER_LENGTH 0        // (mm) Default additional recover length (added to retract length on recover)
  #define RETRACT_RECOVER_LENGTH_SWAP 0   // (mm) Default additional swap recover length (added to retract length on recover from toolchange)
  #define RETRACT_RECOVER_FEEDRATE 8      // (mm/s) Default feedrate for recovering from retraction
  #define RETRACT_RECOVER_FEEDRATE_SWAP 8 // (mm/s) Default feedrate for recovering from swap retraction
  #ifdef MIXING_EXTRUDER
    //#define RETRACT_SYNC_MIXING         // Retract and restore all mixing steppers simultaneously
  #endif
#endif


//电机相关参数的默认参数宏的定义
#ifndef DEFAULT_MAX_ACCELERATION
#define DEFAULT_MAX_ACCELERATION  { 3000, 3000, 100, 10000 }
#endif

#ifndef DEFAULT_AXIS_STEPS_PER_UNIT
#define DEFAULT_AXIS_STEPS_PER_UNIT   { 80, 80, 4000, 500 }
#endif

#ifndef DEFAULT_MAX_FEEDRATE
#define DEFAULT_MAX_FEEDRATE          { 300, 300, 5, 25 }
#endif

#define DEFAULT_MINSEGMENTTIME        20000   // (µs)
#define DEFAULT_ACCELERATION          3000    // X, Y, Z and E acceleration for printing moves
#define DEFAULT_RETRACT_ACCELERATION  3000    // E acceleration for retracts
#define DEFAULT_TRAVEL_ACCELERATION   3000    // X, Y, Z acceleration for travel (non printing) moves
#define DEFAULT_MINIMUMFEEDRATE       0.0     // minimum feedrate
#define DEFAULT_MINTRAVELFEEDRATE     0.0

#ifndef DEFAULT_XJERK
#define DEFAULT_XJERK 10.0
#endif
#ifndef DEFAULT_YJERK
#define DEFAULT_YJERK 10.0
#endif
#ifndef DEFAULT_ZJERK
#define DEFAULT_ZJERK  0.3
#endif
#ifndef DEFAULT_EJERK
#define DEFAULT_EJERK    5.0
#endif

#define JUNCTION_DEVIATION_MM 0.013

//长丝切换参数的默认参数宏定义
#define TOOLCHANGE_FS_LENGTH              12  // (mm) Load / Unload length
#define TOOLCHANGE_FS_EXTRA_RESUME_LENGTH  0  // (mm) Extra length for better restart, fine tune by LCD/Gcode)
#define TOOLCHANGE_FS_RETRACT_SPEED   (50*60) // (mm/m) (Unloading)
#define TOOLCHANGE_FS_UNRETRACT_SPEED (25*60) // (mm/m) (On SINGLENOZZLE or Bowden loading must be slowed down)

// Longer prime to clean out a SINGLENOZZLE
#define TOOLCHANGE_FS_EXTRA_PRIME          0  // (mm) Extra priming length
#define TOOLCHANGE_FS_PRIME_SPEED    (4.6*60) // (mm/m) Extra priming feedrate
#define TOOLCHANGE_FS_WIPE_RETRACT         0  // (mm/m) Retract before cooling for less stringing, better wipe, etc.

// Cool after prime to reduce stringing
#define TOOLCHANGE_FS_FAN                 -1  // Fan index or -1 to skip
#define TOOLCHANGE_FS_FAN_SPEED          255  // 0-255
#define TOOLCHANGE_FS_FAN_TIME            10  // (seconds)

#ifndef X_MIN_POS
#define X_MIN_POS 0
#endif
#ifndef Y_MIN_POS
#define Y_MIN_POS 0
#endif
#define TOOLCHANGE_PARK_XY    { X_MIN_POS + 10, Y_MIN_POS + 10 }

#define TOOLCHANGE_ZRAISE                 2 // (mm)

#define FILAMENT_CHANGE_UNLOAD_LENGTH      100  // (mm) The length of filament for a complete unload.
#define FILAMENT_CHANGE_FAST_LOAD_LENGTH     0  // (mm) Load length of filament, from extruder gear to nozzle.

//backlash参数的默认参数宏
#define BACKLASH_DISTANCE_MM { 0, 0, 0 } // (mm)
#define BACKLASH_CORRECTION    0.0       // 0.0 = no correction; 1.0 = full correction

//喷头和touch的相对位置
#ifndef NOZZLE_TO_PROBE_OFFSET
  #define NOZZLE_TO_PROBE_OFFSET { 0, 0, 0 }
#endif

//Delta参数的默认参数宏
#ifndef DELTA_ENDSTOP_ADJ
  #define DELTA_ENDSTOP_ADJ { 0, 0, 0 }
#endif
#ifndef DELTA_TOWER_ANGLE_TRIM
  #define DELTA_TOWER_ANGLE_TRIM { 0, 0, 0 }
#endif
#ifndef DELTA_RADIUS_TRIM_TOWER
  #define DELTA_RADIUS_TRIM_TOWER { 0, 0, 0 }
#endif
#ifndef DELTA_DIAGONAL_ROD_TRIM_TOWER
  #define DELTA_DIAGONAL_ROD_TRIM_TOWER { 0, 0, 0 }
#endif


// 材料的目标温度和风扇速度
#define PREHEAT_1_LABEL       "PLA"
#define PREHEAT_1_TEMP_HOTEND 180
#define PREHEAT_1_TEMP_BED     70
#define PREHEAT_1_FAN_SPEED     0 // Value from 0 to 255

#define PREHEAT_2_LABEL       "ABS"
#define PREHEAT_2_TEMP_HOTEND 240
#define PREHEAT_2_TEMP_BED    110
#define PREHEAT_2_FAN_SPEED     0 // Value from 0 to 255


//喷头的PID默认参数宏
#ifndef DEFAULT_Kp
#define DEFAULT_Kp 22.2
#endif
#ifndef DEFAULT_Ki
#define DEFAULT_Ki 1.08
#endif
#ifndef DEFAULT_Kd
#define DEFAULT_Kd 114
#endif
#ifndef DEFAULT_Kc
#define DEFAULT_Kc (100) // heating power = Kc * e_speed
#endif
#ifndef DEFAULT_Kf
#define DEFAULT_Kf 10                              // A constant value added to the PID-tuner
#endif

//热床的PID默认参数宏
#ifndef DEFAULT_bedKp
#define DEFAULT_bedKp 10.00
#endif

#ifndef DEFAULT_bedKi
#define DEFAULT_bedKi 0.023
#endif

#ifndef DEFAULT_bedKd
#define DEFAULT_bedKd 305.4
#endif

//断电恢复的使能
#define PLR_ENABLED_DEFAULT   false // Power Loss Recovery enabled by default. (Set with 'M413 Sn' & M500)

//长丝参数
#define DEFAULT_NOMINAL_FILAMENT_DIA 1.75
#define DEFAULT_VOLUMETRIC_EXTRUDER_LIMIT 0.00      // (mm^3/sec)

#define LIN_ADVANCE_K 0.22    // Unit: mm compression per 1mm/s extruder speed


#define PLR_ENABLED_DEFAULT   false // Power Loss Recovery enabled by default. (Set with 'M413 Sn' & M500)

//可控风扇
#ifndef CONTROLLERFAN_SPEED_ACTIVE
  #define CONTROLLERFAN_SPEED_ACTIVE 255
#endif
#ifndef CONTROLLERFAN_SPEED_IDLE
  #define CONTROLLERFAN_SPEED_IDLE     0
#endif
#ifndef CONTROLLERFAN_IDLE_TIME
  #define CONTROLLERFAN_IDLE_TIME     60
#endif


// Macros for initializing arrays
#define LIST_16(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,...) A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P
#define LIST_15(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,...) A,B,C,D,E,F,G,H,I,J,K,L,M,N,O
#define LIST_14(A,B,C,D,E,F,G,H,I,J,K,L,M,N,...) A,B,C,D,E,F,G,H,I,J,K,L,M,N
#define LIST_13(A,B,C,D,E,F,G,H,I,J,K,L,M,...) A,B,C,D,E,F,G,H,I,J,K,L,M
#define LIST_12(A,B,C,D,E,F,G,H,I,J,K,L,...) A,B,C,D,E,F,G,H,I,J,K,L
#define LIST_11(A,B,C,D,E,F,G,H,I,J,K,...) A,B,C,D,E,F,G,H,I,J,K
#define LIST_10(A,B,C,D,E,F,G,H,I,J,...) A,B,C,D,E,F,G,H,I,J
#define LIST_9( A,B,C,D,E,F,G,H,I,...) A,B,C,D,E,F,G,H,I
#define LIST_8( A,B,C,D,E,F,G,H,...) A,B,C,D,E,F,G,H
#define LIST_7( A,B,C,D,E,F,G,...) A,B,C,D,E,F,G
#define LIST_6( A,B,C,D,E,F,...) A,B,C,D,E,F
#define LIST_5( A,B,C,D,E,...) A,B,C,D,E
#define LIST_4( A,B,C,D,...) A,B,C,D
#define LIST_3( A,B,C,...) A,B,C
#define LIST_2( A,B,...) A,B
#define LIST_1( A,...) A

#define _LIST_N(N,V...) LIST_##N(V)
#define LIST_N(N,V...) _LIST_N(N,V)
#define ARRAY_N(N,V...) { _LIST_N(N,V) }

//
// BACKLASH_COMPENSATION
//
struct BacklashCompenSation_t{

    struct XYZFloat_t DistanceMm;   // M425 X
    FLOAT SmoothingMm;   // M425 S
    U8    Correction;    // M425 F
};

//
// SKEW_CORRECTION
//
struct SkewFactor_t
{
    FLOAT XY = XY_SKEW_FACTOR;
    FLOAT XZ = XZ_SKEW_FACTOR;
    FLOAT YZ = YZ_SKEW_FACTOR;
};

struct  PlannerSettings_t
{
    U32   Max_Accel[AxisNum],   // (mm/s^2) M201 XYZE
          MinSegmentTime;       // (µs) M205 B
    FLOAT AxisSteps[AxisNum];   // (steps) M92 XYZE - Steps per millimeter(每1mm的步数)
    FLOAT MaxFeedrate[AxisNum]; // (mm/s) M203 XYZE - Max speeds
    FLOAT Accel,                // (mm/s^2) M204 S - Normal acceleration. DEFAULT ACCELERATION for all printing moves.
          RetractAccel,         // (mm/s^2) M204 R - Retract acceleration. Filament pull-back and push-forward while standing still in the other axes
          TravelAccel;          // (mm/s^2) M204 T - Travel acceleration. DEFAULT ACCELERATION for all NON printing moves.
    FLOAT MinFeedrate,          // (mm/s) M205 S - Minimum linear feedrate
          MinTravelFeedrate;    // (mm/s) M205 T - Minimum travel feedrate
};

struct RunoutAttr_t
{
    FLOAT DistanceMm;                             // M412 D
    BOOL SensorEnabled;                           // M412 S
};

struct MeshLevelAttr_t
{
    FLOAT MblOffsetZ;                            // mbl.z_offset
    FLOAT MblValuesZ[MESH_GRID_MAX_POINTS_X]   // mbl.z_values
                      [MESH_GRID_MAX_POINTS_Y];
};

struct LinearAutoLevelAttr_t
{
    struct MotionCoord_t DilinearGridSpacing, DilinearStart;       // G29 L F
    FLOAT ValuesZ[GRID_MAX_POINTS_X][MESH_GRID_MAX_POINTS_Y];
};

struct UblLevelAttr_t
{
    BOOL PlannerLevelingActive;  // M420 S  planner.leveling_active
    U8   UblStorageSlot;         // ubl.storage_slot
};

struct FilaVolumeAttr_t
{
    FLOAT PlannerFilaSize[EXTRUDERS];                // M200 T D  planner.filament_size[]
    FLOAT PlannerVolumetricExtruderLimit[EXTRUDERS]; // M200 T L  planner.volumetric_extruder_limit[]
    BOOL  VolumetricEnabled;                         // M200 S  parser.volumetric_enabled
};

struct PidCF_t
{
    float Kp, Ki, Kd, Kc, Kf;
};

struct HotEndPidAttr_t
{
    struct PidCF_t Pid[HOTENDS];
    S16            LpqLen;
};

struct FwretractSettings_t
{
       FLOAT Length;              // M207 S - G10 Retract length
       FLOAT Feedrate;            // M207 F - G10 Retract feedrate
       FLOAT RaiseZ,              // M207 Z - G10 Retract hop size
             RecoverExtra;        // M208 S - G11 Recover length
       FLOAT RecoverFeedrate;     // M208 F - G11 Recover feedrate
       FLOAT SwapLength,          // M207 W - G10 Swap Retract length
             SwapRecoverExtra;    // M208 W - G11 Swap Recover length
       FLOAT SwapRecoverFeedrate; // M208 R - G11 Swap Recover feedrate
       BOOL  AutoRetractEnabled;                             // M209 S
} ;

struct PID_t
{
    FLOAT Kp, Ki, Kd;
};

struct CtrlFanSetting_t
{
  U8   ActiveSpeed, // 0-255 (fullspeed); Speed with enabled stepper motors
       IdleSpeed;   // 0-255 (fullspeed); Speed after idle period with all motors are disabled
  U16  Duration;    // Duration in seconds for the fan to run after all motors are disabled
  BOOL AutoMode;    // Default true
};

struct FilaChangeSettings_t
{
  FLOAT UnloadLength, LoadLength;
};

struct ToolChangeSettings_t
{
  #if ENABLED_TOOLCHANGE_FILAMENT_SWAP
    FLOAT SwapLength, ExtraPrime, ExtraResume;
    S16   PrimeSpeed, RetractSpeed, UnRetractSpeed, Fan, FanSpeed, FanTime;
  #endif
  #if ENABLED_TOOLCHANGE_PARK
    BOOL EnablePark;
    FLOAT ChangePointX;
    FLOAT ChangePointY;
  #endif
  FLOAT RaiseZ;
};

struct Preheat_t
{
  U16 HotendTemp;
  U16 BedTemp;
  U16 FanSpeed;
};

struct SettingsData_t    //对应marlin的SettingsDataStruct
{
  S8    Version[4];                   // Vnn\0
  U16   Crc;                          // Data Checksum
  S16   LcdContrast;                  // M250 C   HAS_LCD_CONTRAST
  U32   ServoAngles[EEPROM_NUM_SERVOS][2];  // M281 P L U    // SERVO_ANGLES
  U32   MotorCurrentSetting[3];       // M907 X Z E   HAS_MOTOR_CURRENT_PWM
  FLOAT PlannerExtruderAdvanceK[EXTRUDERS]; // M900 K  planner.extruder_advance_K    LIN_ADVANCE
  FLOAT PlannerMaxJerk[AxisNum];      // M205 XYZE  planner.max_jerk
  FLOAT PlannerJunctionDeviation;     // M205 J     planner.junction_deviation_mm
  FLOAT PlannerFadeHeightZ;           // M420 Zn  planner.z_fade_height  ENABLE_LEVELING_FADE_HEIGHT (可用的床平淡出高度)
  BOOL  BltouchLastWrittenMode;       // BLTOUCH
  BOOL  RecoveryEnabled;              // M413 S   // POWER_LOSS_RECOVERY
  U8    PowerMonitorFlags;            // M430 I V W   Power monitor
  U8    Esteppers;                     // E轴电机个数  DISTINCT_E_FACTORS

  struct PlannerSettings_t      PlanSetting;
  struct XYZFloat_t             HomeOffset;   // 原点偏移
  struct RunoutAttr_t           FiaRunOutAttr;    //FILAMENT_RUNOUT_SENSOR
  struct XYZFloat_t             ProbeOffset; // HAS_BED_PROBE
  struct LinearAutoLevelAttr_t  AutoLevelAttr;  // AUTO_BED_LEVELING_BILINEAR
  struct UblLevelAttr_t         UblLevelAttr;  // AUTO_BED_LEVELING_UBL
  struct HotEndPidAttr_t        Hotend; // PIDTEMP
  struct PID_t                  BedPid;                                         // M304 PID / M303 E-1 U   PIDTEMPBED
  struct CtrlFanSetting_t       CtrlFanSettings;      // M710   Controller fan settings
  struct FwretractSettings_t    Retract;  // M207 S F Z W, M208 S F W R , M209 S   FWRETRACT
  struct FilaVolumeAttr_t       FilaVolAttr;   //  !NO_VOLUMETRIC
  struct XYZFloat_t             CoordinateSystem[MAX_COORDINATE_SYSTEMS];  // G54-G59.3   CNC_COORDINATE_SYSTEMS
  struct SkewFactor_t           PlannerSkewF;                    // M852 I J K  planner.skew_factor   SKEW_CORRECTION
  struct BacklashCompenSation_t Backlash;  // BACKLASH_COMPENSATION
  struct MeshLevelAttr_t        MeshAttr;   // MESH_BED_LEVELING

#if 0
  //
  // ABL_PLANAR
  //
  matrix_3x3 planner_bed_level_matrix;                  // planner.bed_level_matrix

  //
  // HAS_TRINAMIC_CONFIG
  //
  tmc_stepper_current_t tmc_stepper_current;            // M906 X Y Z X2 Y2 Z2 Z3 Z4 E0 E1 E2 E3 E4 E5
  tmc_hybrid_threshold_t tmc_hybrid_threshold;          // M913 X Y Z X2 Y2 Z2 Z3 Z4 E0 E1 E2 E3 E4 E5
  tmc_sgt_t tmc_sgt;                                    // M914 X Y Z X2 Y2 Z2 Z3 Z4
  tmc_stealth_enabled_t tmc_stealth_enabled;            // M569 X Y Z X2 Y2 Z2 Z3 Z4 E0 E1 E2 E3 E4 E5
#endif

  #if HAS_HOTEND_OFFSET
  struct XYZFloat_t HotendOffset[HOTENDS - 1];               // M218 XYZ
  #endif

 /*
  //
  // Temperature first layer compensation values
  //
  #if PROBE_TEMP_COMPENSATION
    int16_t z_offsets_probe[COUNT(temp_comp.z_offsets_probe)], // M871 P I V
            z_offsets_bed[COUNT(temp_comp.z_offsets_bed)]      // M871 B I V
            #if ENABLED(USE_TEMP_EXT_COMPENSATION)
              , z_offsets_ext[COUNT(temp_comp.z_offsets_ext)]  // M871 E I V
            #endif
            ;
  #endif


  //
  // Z_STEPPER_AUTO_ALIGN, Z_STEPPER_ALIGN_KNOWN_STEPPER_POSITIONS
  //
  #if ENABLED(Z_STEPPER_AUTO_ALIGN)
    xy_pos_t z_stepper_align_xy[NUM_Z_STEPPER_DRIVERS];             // M422 S X Y
    #if ENABLED(Z_STEPPER_ALIGN_KNOWN_STEPPER_POSITIONS)
      xy_pos_t z_stepper_align_stepper_xy[NUM_Z_STEPPER_DRIVERS];   // M422 W X Y
    #endif
  #endif

  //
  // User-defined Thermistors
  //
  #if HAS_USER_THERMISTORS
    user_thermistor_t user_thermistor[USER_THERMISTORS]; // M305 P0 R4700 T100000 B3950
  #endif
*/

  //
  // Material Presets
  //
  #if PREHEAT_COUNT
  struct Preheat_t MaterialPreset[PREHEAT_COUNT];        // M145 S0 H B F
  #endif

    //
    // DELTA / [XYZ]_DUAL_ENDSTOPS
    //
    #if ENABLED_DELTA
      FLOAT      DeltaHeight;                                 // M666 H
      XYZFloat_t DeltaEndstopAdj;                      // M666 X Y Z
      FLOAT      DeltaRadius,                                 // M665 R
                 DeltaDiagonalRod,                           // M665 L
                 DeltaSegmentsPerSecond;                    // M665 S
      XYZFloat_t DeltaTowerAngleTrim,                 // M665 X Y Z
                 DeltaDiagonalRodTrim;                // M665 A B C
    #elif HAS_EXTRA_ENDSTOPS
      FLOAT X2EndstopAdj,                               // M666 X
            Y2EndstopAdj,                               // M666 Y
            Z2EndstopAdj,                               // M666 (S2) Z
            Z3EndstopAdj,                               // M666 (S3) Z
            Z4EndstopAdj;                               // M666 (S4) Z
    #endif

  //
  // ADVANCED_PAUSE_FEATURE
  //
  #if EXTRUDERS
    struct FilaChangeSettings_t FcSettings[EXTRUDERS];       // M603 T U L
  #endif


  //
  // Tool-change settings
  //
  #if EXTRUDERS > 1
    struct ToolChangeSettings_t ToolChangeSettings;          // M217 S P R
  #endif

/*
  //
  // EXTENSIBLE_UI
  //
  #if ENABLED(EXTENSIBLE_UI)
    // This is a significant hardware change; don't reserve space when not present
    uint8_t extui_data[ExtUI::eeprom_data_size];
  #endif

  //
  // HAS_CASE_LIGHT_BRIGHTNESS
  //
  #if HAS_CASE_LIGHT_BRIGHTNESS
    uint8_t case_light_brightness;
  #endif
*/
};

extern struct SettingsData_t SettingData;
extern FLOAT UblValueZ[UBL_GRID_MAX_POINTS_X][UBL_GRID_MAX_POINTS_Y];

VOID EepromStart();
VOID EepromSkip(U16 Skip);
VOID EepromFinish();
S32  EepromWrite(S8 *Data, S32 Len);
S32  EepromRead(S8 *Buff, S32 Len);


S32  EepromSaveSetting();
S32  EepromLoadSetting();
S32  SettingReport(BOOL ForReplay);
VOID EepromResetSetting();
S32  EepromUpdateSetting();
S32  EepromApplySetting();
BOOL EepromValidate();



#ifdef __cplusplus
}
#endif

#endif
