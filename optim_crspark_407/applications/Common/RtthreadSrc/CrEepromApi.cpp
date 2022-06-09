#include "../Include/CrEepromApi.h"
#include "../Include/CrEeprom.h"
#include <stdio.h>
#include <string.h>
#include "CrModel/CrHeaterManager.h"
#include "CrInc/CrMsgType.h"
#include "Common/Include/CrSleep.h"

#define WITHIN(A,B,C)  ((A >= B) && (A <= C))

static U32 Offset = EEPROM_OFFSET;
BOOL EepromError = false;
struct SettingsData_t SettingData = {0};
static U16 UsedSize = sizeof(SettingData);
FLOAT UblValueZ[UBL_GRID_MAX_POINTS_X][UBL_GRID_MAX_POINTS_Y] = {0.0f};

static const U16 MeshesEnd = EEPROM_CAPACITY - 129;   // 128 (+1 because of the change to capacity rather than last valid address)
// is a placeholder for the size of the MAT; the MAT will always
// live at the very end of the eeprom


//运动参数
static const U32   _DMA[]  = DEFAULT_MAX_ACCELERATION;
static const FLOAT _DASU[]  = DEFAULT_AXIS_STEPS_PER_UNIT;
static const FLOAT _DMF[]  = DEFAULT_MAX_FEEDRATE;


static U16 MeshesStartIndex()
{
    return (UsedSize + EEPROM_OFFSET + 32) & 0xFFF8;  // Pad the end of configuration data so it can float up
    // or down a little bit without disrupting the mesh data
}

static U16 CalcNumMeshes()
{
    return (MeshesEnd - MeshesStartIndex()) / sizeof(UblValueZ);
}

static S32 MeshSlotOffset(const U8 Slot)
{
    return MeshesEnd - (Slot + 1) * sizeof(UblValueZ);
}

static VOID StoreMesh(const S8 Slot)
{
#ifdef AUTO_BED_LEVELING_UBL
    const S16 A = CalcNumMeshes();
    if ( !WITHIN(Slot, 0, A - 1) )
    {
        printf("?Invalid slot\n");
        printf("%d mesh slots available\n", Slot);
        printf("E2END=%d meshes_end=%d slot=%d\n", EEPROM_CAPACITY - 1, MeshesEnd, Slot);
        return;
    }

    S32 Pos = MeshSlotOffset(Slot);
    U16 Crc = 0;


    EepromStart();
    // Write crc to MAT along with other data, or just tack on to the beginning or end
    Offset = Pos;
    S16 Res = EepromWrite((S8 *)UblValueZ, sizeof(UblValueZ));
    BOOL Status = false;
    if ( Res > 0 )
    {
        Crc16(&Crc, UblValueZ, Res);
        if ( Res >= sizeof(UblValueZ) )
            Status = true;
    }

    EepromFinish();

    if (Status) printf("?Unable to save mesh data.\n");
    else        printf("Mesh saved in slot %d \n", Slot);

#else

    // Other mesh types

#endif
    return;
}

static VOID LoadMesh(const S8 Slot, VOID * const Into/*=nullptr*/)
{
#ifdef AUTO_BED_LEVELING_UBL
    const S16 A = CalcNumMeshes();

    if ( !WITHIN(Slot, 0, A - 1) )
    {
        printf("?Invalid slot\n");
        printf("%d mesh slots available\n", Slot);
        return;
    }

    S32 Pos = MeshSlotOffset(Slot);
    U16 Crc = 0;
    U8 * const Dest = Into ? (U8 *)Into : (U8 *)UblValueZ;

    EepromStart();
    // Write crc to MAT along with other data, or just tack on to the beginning or end
    Offset = Pos;
    const U16 Res = EepromRead(Dest, sizeof(UblValueZ));
    BOOL Status = false;
    if ( Res > 0 )
    {
        Crc16(&Crc, UblValueZ, Res);
        if ( Res >= sizeof(UblValueZ) )
            Status = true;
    }

    EepromFinish();

    if (Status) printf("?Unable to load mesh data.\n");
    else        printf("Mesh loaded from slot %d\n", Slot);

    EepromFinish();

    #else

    // Other mesh types

    #endif
}

static BOOL RestoreEeprom();

static S8 TempUnitsCode(TemperUnits_t TempUnits) {
  return TempUnits == CELSIUS ? 'C' : TempUnits == FAHRENHEIT ? 'F' : 'K';
}

static S8* TempUnitsName(TemperUnits_t TempUnits)
{
  return TempUnits == CELSIUS ? (S8 *)"Celsius" : TempUnits == FAHRENHEIT ? (S8 *)"Fahrenheit" : (S8 *)"Kelvin";
}

// Extruder offsets
#if HAS_HOTEND_OFFSET
static void ResetHotendOffsets()
{
    constexpr FLOAT tmp[3][HOTENDS] = { HOTEND_OFFSET_X, HOTEND_OFFSET_Y, HOTEND_OFFSET_Z };
    // Transpose from [XYZ][HOTENDS] to [HOTENDS][XYZ]
    HOTEND_LOOP() LOOP_XYZ(a) hotend_offset[e][a] = tmp[a][e];

    #if ENABLED_DUAL_X_CARRIAGE
      hotend_offset[1].x = _MAX(X2_HOME_POS, X2_MAX_POS);
    #endif
}
#endif


static void FwRetractReset()
{
#ifdef FWRETRACT
    SettingData.Retract.AutoRetractEnabled = false;
    SettingData.Retract.Length = RETRACT_LENGTH;
    SettingData.Retract.Feedrate = RETRACT_FEEDRATE;
    SettingData.Retract.RaiseZ = RETRACT_ZRAISE;
    SettingData.Retract.RecoverExtra = RETRACT_RECOVER_LENGTH;
    SettingData.Retract.RecoverFeedrate = RETRACT_RECOVER_FEEDRATE;
    SettingData.Retract.SwapLength = RETRACT_LENGTH_SWAP;
    SettingData.Retract.SwapRecoverExtra = RETRACT_RECOVER_LENGTH_SWAP;
    SettingData.Retract.SwapRecoverFeedrate = RETRACT_RECOVER_FEEDRATE_SWAP;
#if 0     //暂时没有此功能，不做实现
    current_hop = 0.0;

    LOOP_L_N(i, EXTRUDERS) {
      retracted[i] = false;
      #if EXTRUDERS > 1
        retracted_swap[i] = false;
      #endif
      current_retract[i] = 0.0;
    }
#endif

#endif
}

static VOID CtrlFanReset()
{
    SettingData.CtrlFanSettings.ActiveSpeed = CONTROLLERFAN_SPEED_ACTIVE;
    SettingData.CtrlFanSettings.IdleSpeed = CONTROLLERFAN_SPEED_IDLE;
    SettingData.CtrlFanSettings.Duration = CONTROLLERFAN_IDLE_TIME;
    SettingData.CtrlFanSettings.AutoMode = true;
}

static VOID Crc16(U16 *Crc, const VOID * const Data, U16 Cnt)
{
    U8 *Ptr = (U8 *)Data;
    while (Cnt--)
    {
      *Crc = (U16)(*Crc ^ (U16)(((U16)*Ptr++) << 8));
      for (U8 i = 0; i < 8; i++)
        *Crc = (U16)((*Crc & 0x8000) ? ((U16)(*Crc << 1) ^ 0x1021) : (*Crc << 1));
    }
}

VOID EepromStart()
{
    CrEepromOpen();
    Offset = EEPROM_OFFSET;
}

VOID EepromSkip(U16 Skip)
{
    Offset += Skip;

    return;
}

VOID EepromFinish()
{

    Offset = EEPROM_OFFSET;
    CrEepromClose();

    return ;
}

S32  EepromWrite(S8 *Data, S32 Len)
{
    S32 Res = CrEepromWrite(Offset, Data, Len);

    return Res;
}

S32  EepromRead(S8 *Buff, S32 Len)
{
    S32 Res = CrEepromRead(Offset, Buff, Len);

    return Res;
}

S32 EepromUpdateSetting()
{


    return 1;
}


S32  EepromApplySetting()
{


    return 1;
}

S32  SettingReport(BOOL ForReplay)
{
      /**
       * Announce current units, in case inches are being displayed
       */
    //BOOL ForReplay = false;
    #if INCH_MODE_SUPPORT
        //获取直线运动的单位
        FLOAT LinearUnitFactor = 1.0
        printf("  G2 %c ; %s\n", LinearUnitFactor == 1.0 ? '1' : '0', LinearUnitFactor != 1.0 ? " (in)" : " (mm)");
    #else
        printf("  G21    ; Units in mm %s \n", " (mm)");
    #endif


      #if HAS_LCD_MENU
        // Temperature units - for Ultipanel temperature options
        #if TEMPERATURE_UNITS_SUPPORT
          TemperUnits_t TempUnit = CELSIUS;
          printf("  M149 %c ; Units in %s\n", TempUnitsCode(TempUnit), TempUnitsName(TempUnit));
        #else
          printf("  M149 C ; Units in Celsius\n");
        #endif
      #endif


        #if ( EXTRUDERS && DISABLE_NO_VOLUMETRICS )    //      #if EXTRUDERS && DISABLED(NO_VOLUMETRICS)

        /**
         * Volumetric extrusion M200
         */
        BOOL VolumetricEnabled = false;
        if ( !ForReplay )
        {
          printf("; %s", "Filament settings:");
          if ( VolumetricEnabled )
            printf(" Enabled\n");
          else
            printf(" Disabled\n");
        }

        #if EXTRUDERS == 1

          FLOAT FilaSize = 0.0f;
          printf("  M200 S%d D%f", S32(VolumetricEnabled), FilaSize);
          #if VOLUMETRIC_EXTRUDER_LIMIT
          FLOAT VolumetricExtruderLimit = 0.0f;
          printf( " L%f", VolumetricExtruderLimit);
          #endif
          printf("\n");
        #else
          LOOP_L_N( i, EXTRUDERS )
          {
                FLOAT FilaSize = 0.0f;
                printf("  M200 T%d D%f", S32(i), FilaSize);
                #if VOLUMETRIC_EXTRUDER_LIMIT
                FLOAT VolumetricExtruderLimit = 0.0f;
                printf( " L%f", VolumetricExtruderLimit);
                #endif
                printf("\n");
          }
          //BOOL VolumetricEnabled = false;
          printf("  M200 S%d\n", (S32)VolumetricEnabled);
        #endif
      #endif // EXTRUDERS && !NO_VOLUMETRICS

      //打印电机的步进值
      printf("Steps per unit: ");
      FLOAT StepX, StepY, StepZ, StepE;
      printf(" M92 X%f Y%f Z%f E%f\n", StepX, StepY, StepZ, StepE);

      printf("Maximum feedrates (units/s):");
      FLOAT FrX, FrY, FrZ, FrE;
      printf("  M203 X%f Y%f Z%f E%f\n", FrX, FrY, FrZ, FrE);

      printf("Maximum Acceleration (units/s2):");
      FLOAT AccX, AccY, AccZ, AccE;
      printf("  M201 X%f Y%f Z%f E%f\n", AccX, AccY, AccZ, AccE);

      printf("Acceleration (units/s2): P<print_accel> R<retract_accel> T<travel_accel>");
      FLOAT Accel, AccelRetract, AccelTravel;
      printf("  M204 P%f R%f T%f\n", Accel, AccelRetract, AccelTravel);

      printf("Advanced: B<min_segment_time_us> S<min_feedrate> T<min_travel_feedrate> J<junc_dev> X<max_x_jerk> Y<max_y_jerk> Z<max_z_jerk> E<max_e_jerk>");
      FLOAT MinSegmentTime, MinFr, MinTravelFr, JuncDev, MaxJerkX, MaxJerkY, MaxJerkZ, MaxJerkE;
      printf("  M205 B%f S%f T%f J%f X%f Y%f Z%f E%f\n", MinSegmentTime, MinFr, MinTravelFr, JuncDev, MaxJerkX, MaxJerkY, MaxJerkZ, MaxJerkE);

      #if HAS_M206_COMMAND
        printf("Home offset:");
        FLOAT HomeOffsetX, HomeOffsetY, HomeOffsetZ;
        #if IS_CARTESIAN
            printf("  M206 X%f Y%f Z%f\n", HomeOffsetX, HomeOffsetY, HomeOffsetZ)
        #else
            printf("  M206 Z%f\n", HomeOffsetZ);
        #endif
      #endif

      #if HAS_HOTEND_OFFSET
        printf("Hotend offsets:");
        LOOP_S_L_N( e, 1, HOTENDS )
        {
            struct MotionCoord_t HotendOffset;
            printf("  M218 T%d X%f Y%f Z%f\n", (S32)e, HotendOffset.CoorX, HotendOffset.CoorY, HotendOffset.CoorZ);
        }
      #endif

      /**
       * Bed Leveling
       */
      #if HAS_LEVELING

        #if MESH_BED_LEVELING

          printf("Mesh Bed Leveling:");

        #elif (AUTO_BED_LEVELING_UBL)
          //获取ForReplay和Name
          //BOOL ForReplay = false;
          S8 *Name = ""
          if ( !ForReplay )
          {
              printf("; %s : ", Name);
          }

        #elif HAS_ABL_OR_UBL

          printf("Auto Bed Leveling:");

        #endif

        BOOL LevelActive = 0;
        #if ENABLE_LEVELING_FADE_HEIGHT
          FLOAT FadeHeightZ = 0.0f;
          printf("  M420 S%d Z%\n", LevelActive ? 1 : 0, FadeHeightZ)
        #elif
          printf("  M420 S%d\n", LevelActive ? 1 : 0);
        #endif

        #if MESH_BED_LEVELING
          LOOP_L_N( py, GRID_MAX_POINTS_Y ) {
              LOOP_L_N( px, GRID_MAX_POINTS_X ) {
                  FLOAT ValueZ;
                  printf("  G29 S3 I%d J%d", (S32)px, (S32)py);
                  printf(" Z %f\n", ValueZ);
              }
          }
          FLOAT MblOffsetZ;
          printf("  G29 S4 Z%f\n", MblOffsetZ);

        #elif (AUTO_BED_LEVELING_UBL)

          if (!ForReplay)
          {
                printf("\n");
                BOOL LevelingActive = false;   //获取调平状态
                printf("Unified Bed Leveling  System v%s ", UBL_VERSION);                     //ubl.report_state();
                if ( LevelingActive )
                {
                    printf(" active");
                }
                else
                {
                    printf(" in active");
                }
                CrM_Sleep(50);
                printf("\n");
                S32 StorageSlot = 0;   //ubl.storage_slot
                printf("; %s%d", "Active Mesh Slot: ", StorageSlot);
                printf("; %s%d meshs.\n", "EEPROM can hold ", CalcNumMeshes());
          }

         //ubl.report_current_mesh();   // This is too verbose for large meshes. A better (more terse)
                                                    // solution needs to be found.
        #elif (AUTO_BED_LEVELING_BILINEAR)
          BOOL LevelingIsValid = false;  //获取调平是否合法
          if ( LevelingIsValid )
          {
            LOOP_L_N( py, GRID_MAX_POINTS_Y )
              LOOP_L_N( px, GRID_MAX_POINTS_X )
              {
                FLOAT ValueZ = 0.0f;
                printf("  G29 W I%d J%d Z%f\n", (S32)px, " J", (S32)py, ValueZ);    //LINEAR_UNIT(z_values[px][py])
              }
          }
        #endif

      #endif // HAS_LEVELING

      #if EDITABLE_SERVO_ANGLES
        printf("Servo Angles:");
        LOOP_L_N( i, NUM_SERVOS )
        {
           switch ( i )
           {
                #if SWITCHING_EXTRUDER
                  case SWITCHING_EXTRUDER_SERVO_NR:
                  #if EXTRUDERS > 3
                    case SWITCHING_EXTRUDER_E23_SERVO_NR:
                  #endif
                #elif SWITCHING_NOZZLE
                  case SWITCHING_NOZZLE_SERVO_NR:
                #elif ENABLE_BLTOUCH || (HAS_Z_SERVO_PROBE && defined(Z_SERVO_ANGLES))
                  case Z_PROBE_SERVO_NR:
                #endif
                  U16  ServoAnglesL, ServoAnglesU;   //ServoAnglesL对应servo_angles[i][0]  ServoAnglesU对应servo_angles[i][1]
                  printf("  M281 P%d L%d U%d\n", (S32)i, ServoAnglesL, ServoAnglesU);
                default: break;
           }
        }

      #endif // EDITABLE_SERVO_ANGLES

      #if HAS_SCARA_OFFSET

        printf("SCARA settings: S<seg-per-sec> P<theta-psi-offset> T<theta-offset>");
        FLOAT DeltaSegmentsPerSecond;
        struct MotionCoord_t ScaraHomeOffset;
        printf("  M665 S%f P%f T%f Z%f\n", DeltaSegmentsPerSecond, ScaraHomeOffset.CoorX, ScaraHomeOffset.CoorY, ScaraHomeOffset.CoorZ);

      #elif ENABLE_DELTA

        printf("Endstop adjustment:");
        struct MotionCoord_t DeltaEndstopAdj;
        printf("  M666 X%f Y%f Z%f\n", DeltaEndstopAdj.CoorX, DeltaEndstopAdj.CoorY, DeltaEndstopAdj.CoorZ);
        printf("Delta settings: L<diagonal rod> R<radius> H<height> S<segments per sec> XYZ<tower angle trim> ABC<rod trim>");
        struct MotionCoord_t DeltaTowerAngleTrim, DeltaDiagonalRodTrim;
        FLOAT DeltaRadius, DeltaHeight, DeltaDiagonalRod, DeltaSegmentsPerSecond;
        printf("  M665 L%f R%f H%f S%f X%f Y%f Z%f A%f B%f C%f\n", DeltaDiagonalRod
                , DeltaRadius, DeltaHeight, DeltaSegmentsPerSecond,
                , DeltaDiagonalRodTrim.CoorX, DeltaDiagonalRodTrim.CoorY, DeltaDiagonalRodTrim.CoorZ
                , DeltaTowerAngleTrim.CoorX, DeltaTowerAngleTrim.CoorY, DeltaTowerAngleTrim.CoorZ);

      #elif HAS_EXTRA_ENDSTOPS

        printf("Endstop adjustment:");
        printf("  M666");
        //FLOAT X2_endstop_adj;
        #if ENABLED_X_DUAL_ENDSTOPS
            FLOAT X2EndstopAdj;   //endstops.x2_endstop_adj,
            printf(" X%f\n", X2EndstopAdj);
        #endif
        #if ENABLED_Y_DUAL_ENDSTOPS
            FLOAT Y2EndstopAdj;  //endstops.y2_endstop_adj
            printf(" Y%f\n", Y2EndstopAdj);
        #endif
        #if ENABLED_Z_MULTI_ENDSTOPS
          #if NUM_Z_STEPPER_DRIVERS >= 3
            FLOAT Z3EndstopAdj;    //endstops.z3_endstop_adj
            printf(" S2 Z", Z3EndstopAdj);
            printf("  M666 S3 Z%f", Z3EndstopAdj);
            #if NUM_Z_STEPPER_DRIVERS >= 4
              FLOAT Z4EndstopAdj;   //endstops.z4_endstop_adj
              printf("  M666 S4 Z%f", Z4EndstopAdj);
            #endif
            printf("\n");
          #else
            FLOAT Z2EndstopAdj;   //endstops.z2_endstop_adj
            printf(" Z%f\n", Z2EndstopAdj);
          #endif
        #endif

      #endif // [XYZ]_DUAL_ENDSTOPS

      #if PREHEAT_COUNT
         printf("Material heatup parameters:");
         LOOP_L_N( i, PREHEAT_COUNT )
         {
            printf("  M145 S%d", (S32)i
            #if HAS_HOTEND
                S32 HotendTemper = 0;    //material_preset[i].hotend_temp
                printf(" H%d", HotendTemper);
            #endif
            #if HAS_HEATED_BED
                S32 BedTemper = 0;   //material_preset[i].bed_temp
                printf(" B%d", BedTemper);
            #endif
            #if HAS_FAN
                S32 FanSpeed = 0;   //material_preset[i].fan_speed
                printf(" F%d", FanSpeed);
            #endif

            printf("\n");
        }

      #endif

      #if HAS_PID_HEATING

        printf("PID settings:");
        #if ENABLED_PIDTEMP
        HOTEND_LOOP()    //多个挤出机
        {
              FLOAT Kp, Ki, Kd;
              S32 Index;
              #if ENABLED_PID_PARAMS_PER_HOTEND
                  printf("  M301 E%d P%f", Index, Kp);
              #else
                  printf("  M301 P%f", Kp);
              #endif
                  printf(" I%f D%f", Ki, Kd);

            #if ENABLED_PID_EXTRUSION_SCALING
              FLOAT  Kc;
              S16 LpqLen = 0;    //thermalManager.lpq_len
              printf(" C%f", Kc);
              if ( Index == 0 ) printf(" L%d", LpqLen);
            #endif
            #if ENABLED_PID_FAN_SCALING
              FLOAT  Kf;
              printf(" F%f", Kf);
            #endif
            printf("\n");
        }
        #endif // PIDTEMP

        #if ENABLED_PIDTEMPBED
          FLOAT BedKp, BedKi, BedKd;
          printf("  M304 P%f I%f D%f\n", BedKp, BedKi, BedKd);
        #endif

      #endif // PIDTEMP || PIDTEMPBED

      #if HAS_USER_THERMISTORS
          printf("User thermistors:");
          //LOOP_L_N(i, USER_THERMISTORS)    //暂时不做
          //    thermalManager.log_user_thermistor(i, true);
      #endif

      #if HAS_LCD_CONTRAST
        printf("LCD Contrast:");
        S16 Contrast = 0;    //ui.contrast
        printf("  M250 C%f\n", Contrast);
      #endif

      #if CONTROLLER_FAN_EDITABLE
        if ( !ForReplay )
        {
            printf("; Controller Fan\n");
        }

        U8   ActiveSpeed     //controllerFan.settings.active_speed
            , IdleSpeed;     //controllerFan.settings.idle_speed
        U16  Duration;       //controllerFan.settings.duration
        BOOL AutoMode;       //controllerFan.settings.auto_mode
        printf("  M710 S%d I%d A%d D%d ; ( %d% %d%)", (S32)ActiveSpeed, (S32)IdleSpeed, (S32)AutoMode, Duration,
                ((S32)(ActiveSpeed) * 100) / 255, ((S32)(IdleSpeed) * 100) / 255);
      #endif

      #if POWER_LOSS_RECOVERY
          printf("Power-Loss Recovery:");
          BOOL RecoveryEnable = false;
          printf("  M413 S%d\n", int(RecoveryEnable));
      #endif

      #ifdef FWRETRACT

        printf("Retract: S<length> F<units/m> Z<lift>");
        FLOAT RetractLength, SwapRetractLength, RetractFr, RetractRaiseZ;
        printf("  M207 S%f W%f F%f Z%f\n", RetractLength, SwapRetractLength, RetractFr, RetractRaiseZ);

        printf("Recover: S<length> F<units/m>");
        FLOAT RetractRecoverExtra, SwapRetractRecoverExtra, RetractRecoverFr;
        printf("  M208 S%f W%f F%f\n", RetractRecoverExtra, SwapRetractRecoverExtra, RetractRecoverFr);

        #ifdef FWRETRACT_AUTORETRACT

        printf("Auto-Retract: S=0 to disable, 1 to interpret E-only moves as retract/recover");
        BOOL AutoRetractEnable = false;  //fwretract.autoretract_enabled
        printf("  M209 S%d\n", AutoRetractEnable ? 1 : 0);

        #endif // FWRETRACT_AUTORETRACT

      #endif // FWRETRACT

      /**
       * Probe Offset
       */
      #if HAS_BED_PROBE
          if ( !ForReplay )
              printf("; %s", "Z-Probe Offset");
          struct MotionCoord_t ProbeOffset;
          if ( !ForReplay )
          {
              enum MotionUnitType_t Unit;
              if ( Unit == Millimeter )
              {
                  printf(" (in):\n");
              }
              else
              {
                  printf(" (mm):\n");
              }
          }

          #if HAS_PROBE_XY_OFFSET
            printf("  M851 X%f Y%f Z%f", ProbeOffset.CoorX, ProbeOffset.CoorY, ProbeOffset.CoorZ);
          #else
            printf("  M851 X0 Y0 Z%f", ProbeOffset.CoorZ);
          #endif
      #endif

      /**
       * Bed Skew Correction
       */
      #if SKEW_CORRECTION_GCODE
        printf("Skew Factor: ");
        struct SkewFactor_t SkF;
        #if SKEW_CORRECTION_FOR_Z
            printf("  M852 I%f J%f K%f\n", SkF.xy, SkF.xz, SkF.yz);
        #else
            printf("  M852 S%f\n", SkF.xy);
        #endif
      #endif

      #if HAS_TRINAMIC_CONFIG    //TMC电机相关内容暂时不做
#if 0
        /**
         * TMC stepper driver current
         */
          printf("Stepper driver current:");

        #if AXIS_IS_TMC(X) || AXIS_IS_TMC(Y) || AXIS_IS_TMC(Z)
          say_M906(forReplay);
          #if AXIS_IS_TMC(X)
            SERIAL_ECHOPAIR_P(SP_X_STR, stepperX.getMilliamps());
          #endif
          #if AXIS_IS_TMC(Y)
            SERIAL_ECHOPAIR_P(SP_Y_STR, stepperY.getMilliamps());
          #endif
          #if AXIS_IS_TMC(Z)
            SERIAL_ECHOPAIR_P(SP_Z_STR, stepperZ.getMilliamps());
          #endif
          SERIAL_EOL();
        #endif

        #if AXIS_IS_TMC(X2) || AXIS_IS_TMC(Y2) || AXIS_IS_TMC(Z2)
          say_M906(forReplay);
          SERIAL_ECHOPGM(" I1");
          #if AXIS_IS_TMC(X2)
            SERIAL_ECHOPAIR_P(SP_X_STR, stepperX2.getMilliamps());
          #endif
          #if AXIS_IS_TMC(Y2)
            SERIAL_ECHOPAIR_P(SP_Y_STR, stepperY2.getMilliamps());
          #endif
          #if AXIS_IS_TMC(Z2)
            SERIAL_ECHOPAIR_P(SP_Z_STR, stepperZ2.getMilliamps());
          #endif
          SERIAL_EOL();
        #endif

        #if AXIS_IS_TMC(Z3)
          say_M906(forReplay);
          SERIAL_ECHOLNPAIR(" I2 Z", stepperZ3.getMilliamps());
        #endif

        #if AXIS_IS_TMC(Z4)
          say_M906(forReplay);
          SERIAL_ECHOLNPAIR(" I3 Z", stepperZ4.getMilliamps());
        #endif

        #if AXIS_IS_TMC(E0)
          say_M906(forReplay);
          SERIAL_ECHOLNPAIR(" T0 E", stepperE0.getMilliamps());
        #endif
        #if AXIS_IS_TMC(E1)
          say_M906(forReplay);
          SERIAL_ECHOLNPAIR(" T1 E", stepperE1.getMilliamps());
        #endif
        #if AXIS_IS_TMC(E2)
          say_M906(forReplay);
          SERIAL_ECHOLNPAIR(" T2 E", stepperE2.getMilliamps());
        #endif
        #if AXIS_IS_TMC(E3)
          say_M906(forReplay);
          SERIAL_ECHOLNPAIR(" T3 E", stepperE3.getMilliamps());
        #endif
        #if AXIS_IS_TMC(E4)
          say_M906(forReplay);
          SERIAL_ECHOLNPAIR(" T4 E", stepperE4.getMilliamps());
        #endif
        #if AXIS_IS_TMC(E5)
          say_M906(forReplay);
          SERIAL_ECHOLNPAIR(" T5 E", stepperE5.getMilliamps());
        #endif
        #if AXIS_IS_TMC(E6)
          say_M906(forReplay);
          SERIAL_ECHOLNPAIR(" T6 E", stepperE6.getMilliamps());
        #endif
        #if AXIS_IS_TMC(E7)
          say_M906(forReplay);
          SERIAL_ECHOLNPAIR(" T7 E", stepperE7.getMilliamps());
        #endif
        SERIAL_EOL();

        //暂时不做
        /**
         * TMC Hybrid Threshold
         */
        #if ENABLED(HYBRID_THRESHOLD)
        printf("Hybrid Threshold:");
          #if AXIS_HAS_STEALTHCHOP(X) || AXIS_HAS_STEALTHCHOP(Y) || AXIS_HAS_STEALTHCHOP(Z)
            say_M913(forReplay);
            #if AXIS_HAS_STEALTHCHOP(X)
              SERIAL_ECHOPAIR_P(SP_X_STR, stepperX.get_pwm_thrs());
            #endif
            #if AXIS_HAS_STEALTHCHOP(Y)
              SERIAL_ECHOPAIR_P(SP_Y_STR, stepperY.get_pwm_thrs());
            #endif
            #if AXIS_HAS_STEALTHCHOP(Z)
              SERIAL_ECHOPAIR_P(SP_Z_STR, stepperZ.get_pwm_thrs());
            #endif
            SERIAL_EOL();
          #endif

          #if AXIS_HAS_STEALTHCHOP(X2) || AXIS_HAS_STEALTHCHOP(Y2) || AXIS_HAS_STEALTHCHOP(Z2)
            say_M913(forReplay);
            SERIAL_ECHOPGM(" I1");
            #if AXIS_HAS_STEALTHCHOP(X2)
              SERIAL_ECHOPAIR_P(SP_X_STR, stepperX2.get_pwm_thrs());
            #endif
            #if AXIS_HAS_STEALTHCHOP(Y2)
              SERIAL_ECHOPAIR_P(SP_Y_STR, stepperY2.get_pwm_thrs());
            #endif
            #if AXIS_HAS_STEALTHCHOP(Z2)
              SERIAL_ECHOPAIR_P(SP_Z_STR, stepperZ2.get_pwm_thrs());
            #endif
            SERIAL_EOL();
          #endif

          #if AXIS_HAS_STEALTHCHOP(Z3)
            say_M913(forReplay);
            SERIAL_ECHOLNPAIR(" I2 Z", stepperZ3.get_pwm_thrs());
          #endif

          #if AXIS_HAS_STEALTHCHOP(Z4)
            say_M913(forReplay);
            SERIAL_ECHOLNPAIR(" I3 Z", stepperZ4.get_pwm_thrs());
          #endif

          #if AXIS_HAS_STEALTHCHOP(E0)
            say_M913(forReplay);
            SERIAL_ECHOLNPAIR(" T0 E", stepperE0.get_pwm_thrs());
          #endif
          #if AXIS_HAS_STEALTHCHOP(E1)
            say_M913(forReplay);
            SERIAL_ECHOLNPAIR(" T1 E", stepperE1.get_pwm_thrs());
          #endif
          #if AXIS_HAS_STEALTHCHOP(E2)
            say_M913(forReplay);
            SERIAL_ECHOLNPAIR(" T2 E", stepperE2.get_pwm_thrs());
          #endif
          #if AXIS_HAS_STEALTHCHOP(E3)
            say_M913(forReplay);
            SERIAL_ECHOLNPAIR(" T3 E", stepperE3.get_pwm_thrs());
          #endif
          #if AXIS_HAS_STEALTHCHOP(E4)
            say_M913(forReplay);
            SERIAL_ECHOLNPAIR(" T4 E", stepperE4.get_pwm_thrs());
          #endif
          #if AXIS_HAS_STEALTHCHOP(E5)
            say_M913(forReplay);
            SERIAL_ECHOLNPAIR(" T5 E", stepperE5.get_pwm_thrs());
          #endif
          #if AXIS_HAS_STEALTHCHOP(E6)
            say_M913(forReplay);
            SERIAL_ECHOLNPAIR(" T6 E", stepperE6.get_pwm_thrs());
          #endif
          #if AXIS_HAS_STEALTHCHOP(E7)
            say_M913(forReplay);
            SERIAL_ECHOLNPAIR(" T7 E", stepperE7.get_pwm_thrs());
          #endif
          SERIAL_EOL();
        #endif // HYBRID_THRESHOLD

        //暂时不做
        /**
         * TMC Sensorless homing thresholds
         */
        #if USE_SENSORLESS
          printf("StallGuard threshold:");
          #if X_SENSORLESS || Y_SENSORLESS || Z_SENSORLESS
            CONFIG_ECHO_START();
            say_M914();
            #if X_SENSORLESS
              SERIAL_ECHOPAIR_P(SP_X_STR, stepperX.homing_threshold());
            #endif
            #if Y_SENSORLESS
              SERIAL_ECHOPAIR_P(SP_Y_STR, stepperY.homing_threshold());
            #endif
            #if Z_SENSORLESS
              SERIAL_ECHOPAIR_P(SP_Z_STR, stepperZ.homing_threshold());
            #endif
            SERIAL_EOL();
          #endif

          #if X2_SENSORLESS || Y2_SENSORLESS || Z2_SENSORLESS
            CONFIG_ECHO_START();
            say_M914();
            SERIAL_ECHOPGM(" I1");
            #if X2_SENSORLESS
              SERIAL_ECHOPAIR_P(SP_X_STR, stepperX2.homing_threshold());
            #endif
            #if Y2_SENSORLESS
              SERIAL_ECHOPAIR_P(SP_Y_STR, stepperY2.homing_threshold());
            #endif
            #if Z2_SENSORLESS
              SERIAL_ECHOPAIR_P(SP_Z_STR, stepperZ2.homing_threshold());
            #endif
            SERIAL_EOL();
          #endif

          #if Z3_SENSORLESS
            CONFIG_ECHO_START();
            say_M914();
            SERIAL_ECHOLNPAIR(" I2 Z", stepperZ3.homing_threshold());
          #endif

          #if Z4_SENSORLESS
            CONFIG_ECHO_START();
            say_M914();
            SERIAL_ECHOLNPAIR(" I3 Z", stepperZ4.homing_threshold());
          #endif

        #endif // USE_SENSORLESS

        /**
         * TMC stepping mode
         */
        #if HAS_STEALTHCHOP      //暂时不做
            printf("Driver stepping mode:");
          #if AXIS_HAS_STEALTHCHOP(X)
            const bool chop_x = stepperX.get_stealthChop_status();
          #else
            constexpr bool chop_x = false;
          #endif
          #if AXIS_HAS_STEALTHCHOP(Y)
            const bool chop_y = stepperY.get_stealthChop_status();
          #else
            constexpr bool chop_y = false;
          #endif
          #if AXIS_HAS_STEALTHCHOP(Z)
            const bool chop_z = stepperZ.get_stealthChop_status();
          #else
            constexpr bool chop_z = false;
          #endif

          if (chop_x || chop_y || chop_z) {
            say_M569(forReplay);
            if (chop_x) SERIAL_ECHOPGM_P(SP_X_STR);
            if (chop_y) SERIAL_ECHOPGM_P(SP_Y_STR);
            if (chop_z) SERIAL_ECHOPGM_P(SP_Z_STR);
            SERIAL_EOL();
          }

          #if AXIS_HAS_STEALTHCHOP(X2)
            const bool chop_x2 = stepperX2.get_stealthChop_status();
          #else
            constexpr bool chop_x2 = false;
          #endif
          #if AXIS_HAS_STEALTHCHOP(Y2)
            const bool chop_y2 = stepperY2.get_stealthChop_status();
          #else
            constexpr bool chop_y2 = false;
          #endif
          #if AXIS_HAS_STEALTHCHOP(Z2)
            const bool chop_z2 = stepperZ2.get_stealthChop_status();
          #else
            constexpr bool chop_z2 = false;
          #endif

          if (chop_x2 || chop_y2 || chop_z2) {
            say_M569(forReplay, PSTR("I1"));
            if (chop_x2) SERIAL_ECHOPGM_P(SP_X_STR);
            if (chop_y2) SERIAL_ECHOPGM_P(SP_Y_STR);
            if (chop_z2) SERIAL_ECHOPGM_P(SP_Z_STR);
            SERIAL_EOL();
          }

          #if AXIS_HAS_STEALTHCHOP(Z3)
            if (stepperZ3.get_stealthChop_status()) { say_M569(forReplay, PSTR("I2 Z"), true); }
          #endif

          #if AXIS_HAS_STEALTHCHOP(Z4)
            if (stepperZ4.get_stealthChop_status()) { say_M569(forReplay, PSTR("I3 Z"), true); }
          #endif

          #if AXIS_HAS_STEALTHCHOP(E0)
            if (stepperE0.get_stealthChop_status()) { say_M569(forReplay, PSTR("T0 E"), true); }
          #endif
          #if AXIS_HAS_STEALTHCHOP(E1)
            if (stepperE1.get_stealthChop_status()) { say_M569(forReplay, PSTR("T1 E"), true); }
          #endif
          #if AXIS_HAS_STEALTHCHOP(E2)
            if (stepperE2.get_stealthChop_status()) { say_M569(forReplay, PSTR("T2 E"), true); }
          #endif
          #if AXIS_HAS_STEALTHCHOP(E3)
            if (stepperE3.get_stealthChop_status()) { say_M569(forReplay, PSTR("T3 E"), true); }
          #endif
          #if AXIS_HAS_STEALTHCHOP(E4)
            if (stepperE4.get_stealthChop_status()) { say_M569(forReplay, PSTR("T4 E"), true); }
          #endif
          #if AXIS_HAS_STEALTHCHOP(E5)
            if (stepperE5.get_stealthChop_status()) { say_M569(forReplay, PSTR("T5 E"), true); }
          #endif
          #if AXIS_HAS_STEALTHCHOP(E6)
            if (stepperE6.get_stealthChop_status()) { say_M569(forReplay, PSTR("T6 E"), true); }
          #endif
          #if AXIS_HAS_STEALTHCHOP(E7)
            if (stepperE7.get_stealthChop_status()) { say_M569(forReplay, PSTR("T7 E"), true); }
          #endif

        #endif // HAS_STEALTHCHOP
#endif

      #endif // HAS_TRINAMIC_CONFIG

      /**
       * Linear Advance
       */
      #if ENABLED_LIN_ADVANCE
            printf("Linear Advance:");
        #if EXTRUDERS < 2
            FLOAT ExtruderAdvanceK;
            printf("  M900 K%f\n", ExtruderAdvanceK);
        #else
          LOOP_L_N( i, EXTRUDERS )
          {
              FLOAT ExtruderAdvanceK;
              printf("  M900 T%d K%f\n", (S32)i, ExtruderAdvanceK);
          }
        #endif
      #endif

      #if HAS_MOTOR_CURRENT_PWM
          printf("Stepper motor currents:");
          S32 MotorCurSettingX, MotorCurSettingZ, MotorCurSettingE;
          printf("  M907 X%d Z%d E%d\n", MotorCurSettingX, MotorCurSettingZ, MotorCurSettingE);
      #endif

      /**
       * Advanced Pause filament load & unload lengths
       */
      #if ENABLED_ADVANCED_PAUSE_FEATURE
        printf("Filament load/unload lengths:");
        FLOAT UnloadLength, LoadLength;
        #if EXTRUDERS == 1
          printf("  M603 ");    //say_M603(forReplay);
          printf("L%f U%f\n", LoadLength, UnloadLength);
        #else
          S8 i;
          for ( i = 0; i < EXTRUDERS; i++ )
          {
              printf("  M603 ");
              printf("T%d L%f U%f\n", i, LoadLength, UnloadLength);
          }
        #endif
      #endif

      #if EXTRUDERS > 1
          printf("Tool-changing:");
          //M217_report(true);  //暂时不做
      #endif

      #if ENABLED_BACKLASH_GCODE
          printf("Backlash compensation:");
          FLOAT BackLashCorrection, BackLashDisX, BackLashDisY, BackLashDisZ, BackLashSmooth;
          printf("  M425 F%f X%f Y%f Z%f", BackLashCorrection, BackLashDisX, BackLashDisY, BackLashDisZ);
          #ifdef BACKLASH_SMOOTHING_MM
          printf(" S%f", BackLashSmooth);
          #endif
          printf("\n");
      #endif

      #if HAS_FILAMENT_SENSOR
        printf("Filament runout sensor:");
        BOOL RunoutEnable = false;
        printf("  M412 S%d" , RunoutEnable);
          #if HAS_FILAMENT_RUNOUT_DISTANCE
            FLOAT RunoutDis = 0.0f;
            printf(" D%f", RunoutDis);
          #endif
        printf("\n");
      #endif

    return 1;
}

VOID EepromResetSetting()
{
    S8 i, Count = sizeof(_DMA)/sizeof(_DMA[0]);
    for ( i = 0; i < AxisNum; ++i )
    {
        if ( i>= Count )
            break;

      SettingData.PlanSetting.Max_Accel[i] = _DMA[i];
      SettingData.PlanSetting.AxisSteps[i] = _DASU[i];
      SettingData.PlanSetting.MaxFeedrate[i] = _DMF[i];
    }

    SettingData.PlanSetting.MinSegmentTime = DEFAULT_MINSEGMENTTIME;
    SettingData.PlanSetting.Accel = DEFAULT_ACCELERATION;
    SettingData.PlanSetting.RetractAccel = DEFAULT_RETRACT_ACCELERATION;
    SettingData.PlanSetting.TravelAccel = DEFAULT_TRAVEL_ACCELERATION;
    SettingData.PlanSetting.MinFeedrate = FLOAT(DEFAULT_MINIMUMFEEDRATE);
    SettingData.PlanSetting.MinTravelFeedrate = FLOAT(DEFAULT_MINTRAVELFEEDRATE);

    #if HAS_CLASSIC_JERK
      #ifndef DEFAULT_XJERK
        #define DEFAULT_XJERK 0
      #endif
      #ifndef DEFAULT_YJERK
        #define DEFAULT_YJERK 0
      #endif
      #ifndef DEFAULT_ZJERK
        #define DEFAULT_ZJERK 0
      #endif

      PlannerMaxJerk[0] = DEFAULT_XJERK;
      PlannerMaxJerk[1] = DEFAULT_YJERK;
      PlannerMaxJerk[2] = DEFAULT_ZJERK;

    #if HAS_CLASSIC_E_JERK
      PlannerMaxJerk[0] = DEFAULT_EJERK;
    #endif

    #endif

    #if HAS_JUNCTION_DEVIATION
      SettingData.PlannerJunctionDeviation = float(JUNCTION_DEVIATION_MM);
    #endif

    #if HAS_SCARA_OFFSET
      SettingData.HomeOffset.X = SettingData.HomeOffset.Y = SettingData.HomeOffset.Z = 0;
    #elif HAS_HOME_OFFSET
      SettingData.HomeOffset.X = SettingData.HomeOffset.Y = SettingData.HomeOffset.Z = 0;
    #endif


    #if HAS_HOTEND_OFFSET      //TERN_(HAS_HOTEND_OFFSET, reset_hotend_offsets());
      constexpr FLOAT Tmp[3][HOTENDS] = { HOTEND_OFFSET_X, HOTEND_OFFSET_Y, HOTEND_OFFSET_Z };
      // Transpose from [XYZ][HOTENDS] to [HOTENDS][XYZ]
      S8 i, j, OffSet = 0;
      for ( i = 0; i < HOTENDS - 1 ; i++ )    //settingdata中的hotendoffset 的个数是HOTENDS - 1
      {
          for ( j = 0; j < 3; j++ )
          {
              SettingData.HotendOffset[i].X = Tmp[Offset + j++];
              SettingData.HotendOffset[i].Y = Tmp[Offset + j++];
              SettingData.HotendOffset[i].Z = Tmp[Offset + j++];
          }

          Offset += j;
      }

      #if ENABLED_DUAL_X_CARRIAGE
          SettingData.HotendOffset[i].X = X2_HOME_POS;                            //hotend_offset[1].x = _MAX(X2_HOME_POS, X2_MAX_POS);
          if ( X2_HOME_POS < X2_MAX_POS )
              SettingData.HotendOffset[i].X = X2_MAX_POS;
      #endif

    #endif

    //
    // Filament Runout Sensor
    //

    #if HAS_FILAMENT_SENSOR
      SettingData.FiaRunOutAttr.SensorEnabled = true;
      SettingData.FiaRunOutAttr.DistanceMm = FILAMENT_RUNOUT_DISTANCE_MM;
      //  runout.reset();    //此功能暂时没有留到以后进行补充
    #endif

    //
    // Tool-change Settings
    //
    #if EXTRUDERS > 1
      #if ENABLED_TOOLCHANGE_FILAMENT_SWAP
      SettingData.ToolChangeSettings.SwapLength     = TOOLCHANGE_FS_LENGTH;
      SettingData.ToolChangeSettings.ExtraResume    = TOOLCHANGE_FS_EXTRA_RESUME_LENGTH;
      SettingData.ToolChangeSettings.RetractSpeed   = TOOLCHANGE_FS_RETRACT_SPEED;
      SettingData.ToolChangeSettings.UnRetractSpeed = TOOLCHANGE_FS_UNRETRACT_SPEED;
      SettingData.ToolChangeSettings.ExtraPrime     = TOOLCHANGE_FS_EXTRA_PRIME;
      SettingData.ToolChangeSettings.PrimeSpeed     = TOOLCHANGE_FS_PRIME_SPEED;
      SettingData.ToolChangeSettings.FanSpeed       = TOOLCHANGE_FS_FAN_SPEED;
      SettingData.ToolChangeSettings.FanTime        = TOOLCHANGE_FS_FAN_TIME;
      #endif

      #if ENABLED_TOOLCHANGE_FS_PRIME_FIRST_USED
        enable_first_prime = false;
      #endif

      #if ENABLED_TOOLCHANGE_PARK
        constexpr struct XYZFloat_t Tpxy = TOOLCHANGE_PARK_XY;
        SettingData.ToolChangeSettings.EnablePark = true;
        SettingData.ToolChangeSettings.ChangePointX = Tpxy.X;
        SettingData.ToolChangeSettings.ChangePointY = Tpxy.Y;
      #endif

        SettingData.ToolChangeSettings.RaiseZ = TOOLCHANGE_ZRAISE;

      #if ENABLED_TOOLCHANGE_MIGRATION_FEATURE
        migration = migration_defaults;
      #endif

    #endif

    #if ENABLED_BACKLASH_GCODE
        SettingData.Backlash.Correction = (BACKLASH_CORRECTION) * 255;
      SettingData.Backlash.Distance = BACKLASH_DISTANCE_MM;
      #ifdef BACKLASH_SMOOTHING_MM
      SettingData.Backlash.SmoothingMm = BACKLASH_SMOOTHING_MM;
      #endif
    #endif

#if 0        //暂时不做
    //TERN_(EXTENSIBLE_UI, ExtUI::onFactoryReset());   //暂时不做UI相关的内容

    //
    // Case Light Brightness
    //
    TERN_(HAS_CASE_LIGHT_BRIGHTNESS, case_light_brightness = CASE_LIGHT_DEFAULT_BRIGHTNESS);

    //
    // Magnetic Parking Extruder
    //
    TERN_(MAGNETIC_PARKING_EXTRUDER, mpe_settings_init());


    //
    // Global Leveling
    //
    TERN_(ENABLE_LEVELING_FADE_HEIGHT, new_z_fade_height = 0.0);
    TERN_(HAS_LEVELING, reset_bed_level());
#endif

    #if HAS_BED_PROBE
      constexpr float Dpo[] = NOZZLE_TO_PROBE_OFFSET;
      if ( (sizeof(Dpo) / sizeof(Dpo[0])) >= 3 )
      {
            #if HAS_PROBE_XY_OFFSET
               SettingData.ProbeOffset.X = Dpo[0];
               SettingData.ProbeOffset.Y = Dpo[1];
               SettingData.ProbeOffset.Z = Dpo[2];
            #else
               SettingData.ProbeOffset.X = SettingData.ProbeOffset.Y = 0;
               SettingData.ProbeOffset.Z = dpo[2];
            #endif
      }
    #endif

#if 0
    //
    // Z Stepper Auto-alignment points, 暂时不做实现
    //
    TERN_(Z_STEPPER_AUTO_ALIGN, z_stepper_align.reset_to_default());

    //找不到base_servo_angles的内容无法做实现
    //
    // Servo Angles
    //
    #ifdef  EDITABLE_SERVO_ANGLES                  //TERN_(EDITABLE_SERVO_ANGLES, COPY(servo_angles, base_servo_angles)); // When not editable only one copy of servo angles exists
    S8 MinSize =

    #endif
#endif

    //
    // BLTOUCH
    //
    //#if ENABLED(BLTOUCH)
    //  bltouch.last_written_mode;
    //#endif

    //
    // Endstop Adjustments
    //
    #if ENABLED_DELTA
      SettingData.DeltaEndstopAdj = DELTA_ENDSTOP_ADJ;
      SettingData.DeltaTowerAngleTrim = DELTA_TOWER_ANGLE_TRIM;
      SettingData.DeltaDiagonalRodTrim = DELTA_DIAGONAL_ROD_TRIM_TOWER;
      SettingData.DeltaHeight = DELTA_HEIGHT;
      SettingData.DeltaRadius = DELTA_RADIUS;
      SettingData.DeltaDiagonalRod = DELTA_DIAGONAL_ROD;
      SettingData.DeltaSegmentsPerSecond = DELTA_SEGMENTS_PER_SECOND;
    #endif

    #if ENABLED_X_DUAL_ENDSTOPS
      #ifndef X2_ENDSTOP_ADJUSTMENT
        #define X2_ENDSTOP_ADJUSTMENT 0
      #endif
      SettingData.X2EndstopAdj = X2_ENDSTOP_ADJUSTMENT;
    #endif

    #if ENABLED_Y_DUAL_ENDSTOPS
      #ifndef Y2_ENDSTOP_ADJUSTMENT
        #define Y2_ENDSTOP_ADJUSTMENT 0
      #endif
      SettingData.Y2EndstopAdj = Y2_ENDSTOP_ADJUSTMENT;
    #endif

    #if ENABLED_Z_MULTI_ENDSTOPS
      #ifndef Z2_ENDSTOP_ADJUSTMENT
        #define Z2_ENDSTOP_ADJUSTMENT 0
      #endif
      SettingData.Z2EndstopAdj = Z2_ENDSTOP_ADJUSTMENT;
      #if NUM_Z_STEPPER_DRIVERS >= 3
        #ifndef Z3_ENDSTOP_ADJUSTMENT
          #define Z3_ENDSTOP_ADJUSTMENT 0
        #endif
      SettingData.Z3EndstopAdj = Z3_ENDSTOP_ADJUSTMENT;
      #endif
      #if NUM_Z_STEPPER_DRIVERS >= 4
        #ifndef Z4_ENDSTOP_ADJUSTMENT
          #define Z4_ENDSTOP_ADJUSTMENT 0
        #endif
      SettingData.Z4EndstopAdj = Z4_ENDSTOP_ADJUSTMENT;
      #endif
    #endif

    //
    // Preheat parameters
    //
    #if PREHEAT_COUNT
      #if HAS_HOTEND
        constexpr uint16_t Hpre[] = ARRAY_N(PREHEAT_COUNT, PREHEAT_1_TEMP_HOTEND, PREHEAT_2_TEMP_HOTEND, PREHEAT_3_TEMP_HOTEND, PREHEAT_4_TEMP_HOTEND, PREHEAT_5_TEMP_HOTEND);
      #endif
      #if HAS_HEATED_BED
        constexpr uint16_t Bpre[] = ARRAY_N(PREHEAT_COUNT, PREHEAT_1_TEMP_BED, PREHEAT_2_TEMP_BED, PREHEAT_3_TEMP_BED, PREHEAT_4_TEMP_BED, PREHEAT_5_TEMP_BED);
      #endif
      #if HAS_FAN
        constexpr uint8_t Fpre[] = ARRAY_N(PREHEAT_COUNT, PREHEAT_1_FAN_SPEED, PREHEAT_2_FAN_SPEED, PREHEAT_3_FAN_SPEED, PREHEAT_4_FAN_SPEED, PREHEAT_5_FAN_SPEED);
      #endif
      LOOP_L_N(i, PREHEAT_COUNT) {
        #if HAS_HOTEND
          SettingData.MaterialPreset[i].HotendTemp = Hpre[i];
        #endif
        #if HAS_HEATED_BED
          SettingData.MaterialPreset[i].BedTemp = Bpre[i];
        #endif
        #if HAS_FAN
          SettingData.MaterialPreset[i].FanSpeed = Fpre[i];
        #endif
      }
    #endif

    //
    // Hotend PID
    //

    #if ENABLED_PIDTEMP
#if 0
      #if ENABLED_PID_PARAMS_PER_HOTEND
        constexpr float defKp[] =
          #ifdef DEFAULT_Kp_LIST
            DEFAULT_Kp_LIST
          #else
            ARRAY_BY_HOTENDS1(DEFAULT_Kp)
          #endif
        , defKi[] =
          #ifdef DEFAULT_Ki_LIST
            DEFAULT_Ki_LIST
          #else
            ARRAY_BY_HOTENDS1(DEFAULT_Ki)
          #endif
        , defKd[] =
          #ifdef DEFAULT_Kd_LIST
            DEFAULT_Kd_LIST
          #else
            ARRAY_BY_HOTENDS1(DEFAULT_Kd)
          #endif
        ;
        static_assert(WITHIN(COUNT(defKp), 1, HOTENDS), "DEFAULT_Kp_LIST must have between 1 and HOTENDS items.");
        static_assert(WITHIN(COUNT(defKi), 1, HOTENDS), "DEFAULT_Ki_LIST must have between 1 and HOTENDS items.");
        static_assert(WITHIN(COUNT(defKd), 1, HOTENDS), "DEFAULT_Kd_LIST must have between 1 and HOTENDS items.");
        #if ENABLED(PID_EXTRUSION_SCALING)
          constexpr float defKc[] =
            #ifdef DEFAULT_Kc_LIST
              DEFAULT_Kc_LIST
            #else
              ARRAY_BY_HOTENDS1(DEFAULT_Kc)
            #endif
          ;
          static_assert(WITHIN(COUNT(defKc), 1, HOTENDS), "DEFAULT_Kc_LIST must have between 1 and HOTENDS items.");
        #endif
        #if ENABLED(PID_FAN_SCALING)
          constexpr float defKf[] =
            #ifdef DEFAULT_Kf_LIST
              DEFAULT_Kf_LIST
            #else
              ARRAY_BY_HOTENDS1(DEFAULT_Kf)
            #endif
          ;
          static_assert(WITHIN(COUNT(defKf), 1, HOTENDS), "DEFAULT_Kf_LIST must have between 1 and HOTENDS items.");
        #endif
        #define PID_DEFAULT(N,E) def##N[E]
      #else
        #define PID_DEFAULT(N,E) DEFAULT_##N
      #endif
      HOTEND_LOOP() {
        PID_PARAM(Kp, e) = float(PID_DEFAULT(Kp, ALIM(e, defKp)));
        PID_PARAM(Ki, e) = scalePID_i(PID_DEFAULT(Ki, ALIM(e, defKi)));
        PID_PARAM(Kd, e) = scalePID_d(PID_DEFAULT(Kd, ALIM(e, defKd)));
        TERN_(PID_EXTRUSION_SCALING, PID_PARAM(Kc, e) = float(PID_DEFAULT(Kc, ALIM(e, defKc))));
        TERN_(PID_FAN_SCALING, PID_PARAM(Kf, e) = float(PID_DEFAULT(Kf, ALIM(e, defKf))));
      }
#endif
    S8 i;
    for ( i = 0; i < HOTENDS; i++ )
    {
        SettingData.Hotend.Pid[i].Kp = DEFAULT_Kp;
        SettingData.Hotend.Pid[i].Ki = DEFAULT_Ki;
        SettingData.Hotend.Pid[i].Kd = DEFAULT_Kd;

        #if ENABLED_PID_EXTRUSION_SCALING
        SettingData.Hotend.Pid[i].Kc = DEFAULT_Kc;
        #endif

        #if ENABLED_PID_FAN_SCALING
        SettingData.Hotend.Pid[i].Kf = DEFAULT_Kf;
        #endif
    }
    #endif

    //
    // PID Extrusion Scaling
    //
    SettingData.Hotend.LpqLen =  20;                        //TERN_(PID_EXTRUSION_SCALING, thermalManager.lpq_len = 20); // Default last-position-queue size

    //
    // Heated Bed PID
    //
    #if ENABLED_PIDTEMPBED
      SettingData.BedPid.Kp = DEFAULT_bedKp;
      SettingData.BedPid.Ki = DEFAULT_bedKi;
      SettingData.BedPid.Kd = DEFAULT_bedKd;
    #endif

#if 0    //没有此功能，暂时不实现
    //
    // User-Defined Thermistors
    //
    TERN_(HAS_USER_THERMISTORS, thermalManager.reset_user_thermistors());



    //
    // Power Monitor
    //
    TERN_(POWER_MONITOR, power_monitor.reset());


    //
    // LCD Contrast
    //
    SettingData.LcdContrast = DEFAULT_LCD_CONTRAST;   //TERN_(HAS_LCD_CONTRAST, ui.set_contrast(DEFAULT_LCD_CONTRAST));
#endif

    //
    // Controller Fan
    //
    CtrlFanReset();     //TERN_(USE_CONTROLLER_FAN, controllerFan.reset());

    //
    // Power-Loss Recovery
    //
    SettingData.RecoveryEnabled = PLR_ENABLED_DEFAULT;            //TERN_(POWER_LOSS_RECOVERY, recovery.enable(ENABLED(PLR_ENABLED_DEFAULT)));


    //
    // Firmware Retraction
    //
    FwRetractReset();

    //
    // Volumetric & Filament Size
    //

    #if DISABLED_NO_VOLUMETRICS
      SettingData.FilaVolAttr.VolumetricEnabled = VOLUMETRIC_DEFAULT_ON;     //ENABLED(VOLUMETRIC_DEFAULT_ON);
      LOOP_L_N(q, EXTRUDERS)
          SettingData.FilaVolAttr.PlannerFilaSize[q] = DEFAULT_NOMINAL_FILAMENT_DIA;
      #if ENABLED_VOLUMETRIC_EXTRUDER_LIMIT
        LOOP_L_N(q, EXTRUDERS)
      SettingData.FilaVolAttr.PlannerVolumetricExtruderLimit[q] = DEFAULT_VOLUMETRIC_EXTRUDER_LIMIT;
      #endif
    #endif

#if 0
    endstops.enable_globally(ENABLED(ENDSTOPS_ALWAYS_ON_DEFAULT));

    reset_stepper_drivers();
#endif

    //
    // Linear Advance
    //

    #if ENABLED_LIN_ADVANCE
      LOOP_L_N(i, EXTRUDERS) {
        SettingData.PlannerExtruderAdvanceK[i] = LIN_ADVANCE_K;
        //TERN_(EXTRA_LIN_ADVANCE_K, other_extruder_advance_K[i] = LIN_ADVANCE_K);   //暂时不做
      }
    #endif

    //
    // Motor Current PWM   设置电机的电流，从而影响功率
    //

    #if HAS_MOTOR_CURRENT_PWM
      SettingData.MotorCurrentSetting[0] = PWM_MOTOR_CURRENT;
      SettingData.MotorCurrentSetting[1] = PWM_MOTOR_CURRENT;
      SettingData.MotorCurrentSetting[2] = PWM_MOTOR_CURRENT;
      LOOP_L_N(q, 3)
        stepper.digipot_current(q, (stepper.motor_current_setting[q] = tmp_motor_current_setting[q]));
    #endif

#if 0     //暂时不实现
    //
    // CNC Coordinate System
    //
    TERN_(CNC_COORDINATE_SYSTEMS, (void)gcode.select_coordinate_system(-1)); // Go back to machine space   选择机器空间为工作空间
#endif

    //
    // Skew Correction
    //
    #if ENABLED_SKEW_CORRECTION_GCODE
      SettingData.PlannerSkewF.XY = XY_SKEW_FACTOR;
      #if ENABLED_SKEW_CORRECTION_FOR_Z
      SettingData.PlannerSkewF.XZ = XZ_SKEW_FACTOR;
      SettingData.PlannerSkewF.YZ = YZ_SKEW_FACTOR;
      #endif
    #endif

    //
    // Advanced Pause filament load & unload lengths
    //
    #if ENABLED_ADVANCED_PAUSE_FEATURE
      LOOP_L_N(e, EXTRUDERS)
      {
        SettingData.FcSettings[e].UnLoadLength = FILAMENT_CHANGE_UNLOAD_LENGTH;
        SettingData.FcSettings[e].LoadLength = FILAMENT_CHANGE_FAST_LOAD_LENGTH;
      }
    #endif


    printf("Hardcoded Default Settings Loaded\n");

    EepromApplySetting(); //将内容应用到具体的变量中

    return;
}

S32 EepromSaveSetting()
{
    S32 Capacity = 0;
    CrEepromIoctl(CMD_GET_EEPROM_CAPACITY, &Capacity);
    if ( sizeof(SettingData) > Capacity )   //存的数据的空间超过了EEPROM的大小
        return 0;

    FLOAT Dummyf = 0;
    S8 Ver[4] = "ERR";
    U16 WorkingCrc = 0;
    memcpy(SettingData.Version, Ver, sizeof(SettingData.Version));
    SettingData.Crc = WorkingCrc;

    EepromStart();
    EepromError = false;

//    //写入版本的初始值
//    EepromWrite(Ver, sizeof(Ver));
//    EepromSkip(sizeof(Ver));
//
//    //跳过校验值的存放位置
//    EepromSkip(sizeof(U16));

    //开始保存参数
    EepromUpdateSetting();

    //保存数据
    S32 Res = EepromWrite((S8*)&SettingData, sizeof(SettingData));
    if ( Res >= sizeof(SettingData) )
    {
        UsedSize = sizeof(SettingData);
        Crc16(&WorkingCrc, (U8 *)&SettingData + sizeof(Ver) + sizeof(WorkingCrc), sizeof(SettingData)- sizeof(Ver) - sizeof(WorkingCrc));
        Offset = EEPROM_OFFSET;
        EepromWrite((S8 *)Version, sizeof(Version));
        EepromSkip(sizeof(Version));
        EepromWrite((S8 *)&WorkingCrc, sizeof(WorkingCrc));
    }
    else
    {
        if ( Res > (sizeof(Ver) + sizeof(WorkingCrc)) )
            Crc16(&WorkingCrc, (U8 *)&SettingData + sizeof(Ver) + sizeof(WorkingCrc), Res - sizeof(Ver) - sizeof(WorkingCrc) );

        EepromError = true;
    }

    printf("Settings Stored ( %d bytes; crc %d )", Res, WorkingCrc);

    EepromFinish();


    //与界面交互提示的内容展示不实现
#ifdef UBL_SAVE_ACTIVE_ON_M500
    if ( SettingData.UblLevelAttr.UblStorageSlot >= 0 )
      StoreMesh(SettingData.UblLevelAttr.UblStorageSlot);
#endif

    return Res;
}

S32 EepromLoadSetting()
{
    S32 Capacity = 0;
    CrEepromIoctl(CMD_GET_EEPROM_CAPACITY, &Capacity);
    if ( sizeof(SettingData) > Capacity )   //存的数据的空间超过了EEPROM的大小
        return 0;

    EepromStart();
    EepromError = false;
    U16 WorkingCrc = 0;

    //读取数据
    struct SettingsData_t Setting;
    S32 Res = EepromRead((S8*)&Setting, sizeof(Setting));
    if ( Res >= sizeof(Setting) )
    {
        UsedSize = sizeof(Setting);
        Crc16(&WorkingCrc, (U8 *)&Setting + sizeof(Setting.Version) + sizeof(WorkingCrc), Res - sizeof(Setting.Version) - sizeof(WorkingCrc) );
        Offset = EEPROM_OFFSET;

        //判断版本是否正确
        if ( strncmp(Setting.Version, Version, sizeof(Version) - 1) != 0 )
        {
            EepromError = true;
            if (Setting.Version[3] != '\0')
            {
                Setting.Version[0] = '?';
                Setting.Version[1] = '\0';
            }

            printf("EEPROM version mismatch (EEPROM=%s , Marlin= %s)\n", Setting.Version, EEPROM_VERSION);
        }
        else
        {
            if ( WorkingCrc == Setting.Crc )
            {
                memcpy(&SettingData, &Setting, sizeof(Setting));

                //获取ubl level attr
                if ( SettingData.UblLevelAttr.UblStorageSlot >= 0 )
                {
                    LoadMesh(SettingData.UblLevelAttr.UblStorageSlot, UblValueZ);
                    printf("Mesh %d loaded from storage.", SettingData.UblLevelAttr.UblStorageSlot);
                }
            }
            else
            {
                EepromError = true;
            }
        }
        SettingReport(false);
    }
    else
    {
        memset((S8 *)&Setting, 0, sizeof(Setting));
        Res = EepromRead((S8*)&Setting, sizeof(Setting));
        if ( Res >= sizeof(Setting) )
        {
            UsedSize = sizeof(Setting);
            Crc16(&WorkingCrc, (U8 *)&Setting + sizeof(Setting.Version) + sizeof(WorkingCrc), Res - sizeof(Setting.Version) - sizeof(WorkingCrc) );
            Offset = EEPROM_OFFSET;

            //判断版本是否正确
            if ( strncmp(Setting.Version, Version, sizeof(Version) - 1) != 0 )
            {
                EepromError = true;
                if (Setting.Version[3] != '\0')
                {
                    Setting.Version[0] = '?';
                    Setting.Version[1] = '\0';
                 }

                 printf("EEPROM version mismatch (EEPROM=%s , Marlin= %s)\n", Setting.Version, EEPROM_VERSION);
            }
            else
            {
               if ( WorkingCrc == Setting.Crc )
               {
                  memcpy(&SettingData, &Setting, sizeof(Setting));

                  //获取ubl level attr
                  if ( SettingData.UblLevelAttr.UblStorageSlot >= 0 )
                  {
                       LoadMesh(SettingData.UblLevelAttr.UblStorageSlot, UblValueZ);
                       printf("Mesh %d loaded from storage.", SettingData.UblLevelAttr.UblStorageSlot);
                   }
                }
                else
                {
                    EepromError = true;
                }
            }
            SettingReport(false);
        }
        else
        {
            if ( Res > (sizeof(Setting.Version) + sizeof(WorkingCrc)) )
                 Crc16(&WorkingCrc, (U8 *)&SettingData + sizeof(Setting.Version) + sizeof(WorkingCrc), Res - sizeof(Setting.Version) - sizeof(WorkingCrc) );

            EepromError = true;

            EepromResetSetting();
        }
    }

    EepromFinish();

    return !EepromError;
}

BOOL EepromValidate()
{
//    validating = true;
    #ifdef ARCHIM2_SPI_FLASH_EEPROM_BACKUP_SIZE
      BOOL Success = EepromLoadSetting();
      if ( !Success && RestoreEeprom() )
      {
          printf("Recovered backup EEPROM settings from SPI Flash\n");
          Success = EepromLoadSetting();
      }
    #else
      const BOOL Success = EepromLoadSetting();
    #endif
//    validating = false;
    return Success;

}

BOOL RestoreEeprom()
{
      U8 Data[sizeof(struct SettingsData_t)];//uint8_t data[ARCHIM2_SPI_FLASH_EEPROM_BACKUP_SIZE];

      bool Success = false;
#if ARCHIM2_SPI_FLASH_EEPROM_BACKUP_SIZE
      Success =  UIFlashStorage::read_config_data(Data, sizeof(struct SettingsData_t));    //此功能暂时不处理
#endif
      if ( Success )
      {
          //将备份的数据保存到eeprom中
          EepromStart();
          Success = EepromWrite((S8 *)Data, sizeof(Data));
          EepromFinish();
      }

      if (Success)
          printf("%s\n", MSG_EEPROM_RESTORED);
      else
          printf("%s\n", MSG_EEPROM_RESET);

      return Success;
}





