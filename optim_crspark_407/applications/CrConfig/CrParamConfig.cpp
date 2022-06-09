/*
#include <MenuSetting.h-bak>
#include <SdCard/CrSd.h>
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-01     cx2470       the first version
 */
#include "CrParamConfig.h"
#include <string.h>
#include <stdio.h>
#include "Common/Include/CrDirentAndFile.h"
//#include "CrMsgType.h"

//加速度结构体
struct Accele_t
{
    U16 Accel;
    U16 ARetract;
    U16 ATravel;
    U16 AmaxX;
    U16 AmaxY;
    U16 AmaxZ;
    U16 AmaxE;
};

//Jerk加速度
struct Jerk_t
{
    FLOAT VxJerk;
    FLOAT VyJerk;
    FLOAT VzJerk;
    FLOAT VeJerk;
};

//速度结构体
struct Velocity_t
{
    U16 VmaxX;
    U16 VmaxY;
    U16 VmaxZ;
    U16 VmaxE;
    U16 Vmin;
    U16 VTravMin;
};

//步进
struct Steps_t
{
    float Xsteps;
    float Ysteps;
    float Zsteps;
    float Esteps;
};

//材料预热温度
struct PreHeatConfig_t
{
    U32 BedPreheatTemp;
    U32 NozzlePreheatTemp;
    /**========================================
     *          ABS材料预设目标温度
     * ========================================*/
    U32 ABSBedPreheatTemp;
    U32 ABSNozzlePreheatTemp;
    /**========================================
     *          PLA材料预设目标温度
     * ========================================*/
    U32 PLABedPreheatTemp;
    U32 PLANozzlePreheatTemp;
};

struct FansSpeed_t
{
    S32 FansSpeed;      //风扇速度
    S32 PLAFansSpeed;   //Pla 材料预设风扇速度
    S32 ABSFansSpeed;   //ABS 材料预设风扇速度
};


#define CONFIG_FILE_SIZE             (0x2FF + 1) // 2K
#define _ALIGN(x) __attribute__      ((aligned(x)))

#define CONFIG_FILENAME             "/eeprom.dat"
#define INDEX_OFFSET                0

struct ParamMaterialTemperSet_t  TemperSet = {0};
struct ParamFanSpeed_t FansSet = {0};
struct ParamMotionParam_t MotionParam  = {0};

S32   FilamentVolumePattern = 0;
float Fil = FIL_DIA;

static S16 PrintRatio = PRINTF_RATIO;       //打印速率
static S16 FlowRatio = FLOW_RATIO;        //流量比

//默认的加速度参数
static struct Accele_t AcceleConfig_t = {
    ACCELERATION_ACCEL,
    ACCELERATION_RETRACT,
    ACCELERATION_TRAVEL,
    ACCELERATION_MAX_X,
    ACCELERATION_MAX_Y,
    ACCELERATION_MAX_Z,
    ACCELERATION_MAX_E
};

static struct Velocity_t VelocityConfig_t = {
        VELOCITY_V_MAX_X,
        VELOCITY_V_MAX_Y,
        VELOCITY_V_MAX_Z,
        VELOCITY_V_MAX_E,
        VELOCITY_V_MIN,
        VELOCITY_V_TRAV_MIN
};

static struct Steps_t  StepsConfig_t = {
            X_STEPS_MM,
            Y_STEPS_MM,
            Z_STEPS_MM,
            E_STEPS_MM
};

static struct Jerk_t JerkConfig_t =
{
        VX_JERK,
        VY_JERK,
        VZ_JERK,
        VE_JERK,
};

static struct PreHeatConfig_t TempConfig_t =
{
    0,
    0,
    ABS_MATERIAL_PREHEAT_BED_TEMP,
    ABS_MATERIAL_PREHEAT_NOZ_TEMP,
    PLA_MATERIAL_PREHEAT_BED_TEMP,
    PLA_MATERIAL_PREHEAT_NOZ_TEMP
};

static struct FansSpeed_t FansSpeedConfig_t = {
        0,
        PLA_MATERIAL_PRE_FAN_SPEED,
        ABS_MATERIAL_PRE_FAN_SPEED
};

static VOID FansConfigRecover();
static VOID PreheatConfigRecover();
static VOID JerkConfigRecover();
static VOID StepsConfigRecover();
static VOID FlowConfigRecover();
static VOID FrConfigRecover();
static VOID JerkConfigRecover();
static VOID AcceleConfigRecover();
static VOID VelocityConfigRecover();
//FLOAT *GetFilementConfig();
static BOOL AccessFinish(S8 * Buf);
static BOOL AccessStart(S8 * Buf);
static BOOL WriteData(S32 *Pos, S8* Value, const S8 Size, S8 *Buf);
static BOOL ReadData(S32 *Pos, S8* Value, const S8 Size, S8 *Buf);

static S32  PrintRatioConfigToParam(S16 *Fr);
static S32  FlowRatioConfigToParam(S16 *Flow);
static S32 PrintRatioParamToMotionConfig(S16 Fr);
static S32 FlowRatioParamToMotionConfig(S16 Flow);

static S32  VelConfigToMotionParamVel(struct Velocity_t *Vel, struct ParamMotionParam_t *Param);
static S32  MotionParamVelToVelConfig(struct ParamMotionParam_t *Param, struct Velocity_t *Vel);
static S32  StepConfigToMotionParamStep(struct Steps_t *Step, struct ParamMotionParam_t *Param);
static S32  MotionParamStepToStepConfig(struct ParamMotionParam_t *Param, struct Steps_t *Step);
static S32  JerkConfigToMotionParamJerk(struct Jerk_t *Jerk, struct ParamMotionParam_t *Param);
static S32  MotionParamJerkToJerkConfig(struct ParamMotionParam_t *Param, struct Jerk_t *Jerk);
static S32  AccelConfigToMotionParamAccel(struct Accele_t *Accel, struct ParamMotionParam_t *Param);
static S32  MotionParamAccelToAccelConfig(struct ParamMotionParam_t *Param, struct Accele_t *Accel);

static S32  FansConfigToFansParam(struct FansSpeed_t *FanSpeed, struct ParamFanSpeed_t *Fans);
static S32  FansParamToFansConfig(struct ParamFanSpeed_t *Fans, struct FansSpeed_t *FanSpeed);
static S32  PreheatConfigToTemperParamPla(struct PreHeatConfig_t *Cfg, struct ParamMaterialTemper_t *Pla);
static S32  PreheatConfigToTemperParamAbs(struct PreHeatConfig_t *Cfg, struct ParamMaterialTemper_t *Abs);
static S32  TemperParamPlaToPreheatConfig(struct PreHeatConfig_t *Cfg, struct ParamMaterialTemper_t *Pla);
static S32  TemperParamAbsToPreheatConfig(struct PreHeatConfig_t *Cfg, struct ParamMaterialTemper_t *Abs);

static VOID CrConfigParamToConfig();   //将参数的内容转换为配置的内容
static VOID CrConfigConfigToParam();   //将配置的内容转换为参数

S32 SaveConfig()
{
    char _ALIGN(4) Buff[CONFIG_FILE_SIZE] = {0};
    S32 Offset = INDEX_OFFSET;

    if ( AccessStart(Buff) <= 0 )
        open(CONFIG_FILENAME,  O_RDONLY | O_CREAT);

    rt_enter_critical();
    WriteData(&Offset, (S8 *)&AcceleConfig_t, sizeof(struct Accele_t), Buff);
    WriteData(&Offset, (S8 *)&FansSpeedConfig_t, sizeof(struct FansSpeed_t), Buff);
    WriteData(&Offset, (S8 *)&Fil, sizeof(float), Buff);
    WriteData(&Offset, (S8 *)&FilamentVolumePattern, sizeof(int), Buff);
    WriteData(&Offset, (S8 *)&JerkConfig_t, sizeof(struct Jerk_t), Buff);
    WriteData(&Offset, (S8 *)&StepsConfig_t, sizeof(struct Steps_t), Buff);
    WriteData(&Offset, (S8 *)&TempConfig_t, sizeof(struct PreHeatConfig_t), Buff);
    WriteData(&Offset, (S8 *)&VelocityConfig_t, sizeof(struct Velocity_t), Buff);
    rt_exit_critical();

    return AccessFinish(Buff);
}



S32 CrConfigSave()
{
    CrConfigParamToConfig(); //将参数转换为配置

    return SaveConfig();
}

S32 CrConfigLoad()
{
    char _ALIGN(4) Buff[CONFIG_FILE_SIZE] = {0};
    S32 Offset = INDEX_OFFSET;

    if ( AccessStart(Buff) > 0 )
    {
        rt_enter_critical();
        ReadData(&Offset, (S8 *)&AcceleConfig_t, sizeof(struct Accele_t), Buff);
        ReadData(&Offset, (S8 *)&FansSpeedConfig_t, sizeof(struct FansSpeed_t), Buff);
        ReadData(&Offset, (S8 *)&Fil, sizeof(float), Buff);
        ReadData(&Offset, (S8 *)&FilamentVolumePattern, sizeof(S32), Buff);
        ReadData(&Offset, (S8 *)&JerkConfig_t, sizeof(struct Jerk_t), Buff);
        ReadData(&Offset, (S8 *)&StepsConfig_t, sizeof(struct Steps_t), Buff);
        ReadData(&Offset, (S8 *)&TempConfig_t, sizeof(struct PreHeatConfig_t), Buff);
        ReadData(&Offset, (S8 *)&VelocityConfig_t, sizeof(struct Velocity_t), Buff);
        rt_exit_critical();
    }
    //将内容进行转换
    CrConfigConfigToParam();

    return 1;
}

VOID CrConfigParamToConfig()
{
    TemperParamAbsToPreheatConfig(&TempConfig_t, &TemperSet.Abs);
    TemperParamPlaToPreheatConfig(&TempConfig_t, &TemperSet.Pla);

    MotionParamVelToVelConfig(&MotionParam, &VelocityConfig_t);

    PrintRatioParamToMotionConfig(MotionParam.PrintRatio);
    FlowRatioParamToMotionConfig(MotionParam.FlowRatio);

    MotionParamStepToStepConfig(&MotionParam, &StepsConfig_t);
    MotionParamJerkToJerkConfig(&MotionParam, &JerkConfig_t);
    MotionParamAccelToAccelConfig(&MotionParam, &AcceleConfig_t);
    FansParamToFansConfig(&FansSet, &FansSpeedConfig_t);

    return ;
}

VOID CrConfigConfigToParam()
{
    PreheatConfigToTemperParamAbs(&TempConfig_t, &TemperSet.Abs);
    PreheatConfigToTemperParamPla(&TempConfig_t, &TemperSet.Pla);
    FansConfigToFansParam(&FansSpeedConfig_t, &FansSet);

    FlowRatioConfigToParam(&MotionParam.FlowRatio);
    PrintRatioConfigToParam(&MotionParam.PrintRatio);

    VelConfigToMotionParamVel(&VelocityConfig_t, &MotionParam);
    StepConfigToMotionParamStep(&StepsConfig_t, &MotionParam);
    JerkConfigToMotionParamJerk(&JerkConfig_t, &MotionParam);
    AccelConfigToMotionParamAccel(&AcceleConfig_t, &MotionParam);

    return ;
}

VOID CrConfigRecover()
{
    AcceleConfigRecover();
    FansConfigRecover();
    *GetFilementConfig() = FIL_DIA;
    JerkConfigRecover();
    StepsConfigRecover();
    PreheatConfigRecover();
    VelocityConfigRecover();
    FrConfigRecover();
    FlowConfigRecover();
    SetFilamentVolumePattern(2);

    return ;
}

VOID CrConfigAbsSave()
{
    TemperParamAbsToPreheatConfig(&TempConfig_t, &TemperSet.Abs);
    FansSpeedConfig_t.ABSFansSpeed = FansSet.Abs.Speed;

    SaveConfig();
}

VOID CrConfigAbsRecover()
{
    TempConfig_t.ABSNozzlePreheatTemp = ABS_MATERIAL_PREHEAT_NOZ_TEMP;
    TempConfig_t.ABSBedPreheatTemp = ABS_MATERIAL_PREHEAT_BED_TEMP;
    FansSpeedConfig_t.ABSFansSpeed = ABS_MATERIAL_PRE_FAN_SPEED;

    PreheatConfigToTemperParamAbs(&TempConfig_t, &TemperSet.Abs);
    FansSet.Abs.Speed = FansSpeedConfig_t.ABSFansSpeed;
}

S32 CrConfigAbsLoad()
{
    char _ALIGN(4) Buff[CONFIG_FILE_SIZE] = {0};
    S32 Offset = INDEX_OFFSET;

    if ( !AccessStart(Buff) )
    {
        printf("Load Fail\n");
        return -1;
    }

    Offset += sizeof(struct Accele_t) + sizeof(struct FansSpeed_t) + sizeof(float) +
              sizeof(S32) + sizeof(struct Jerk_t) + sizeof(struct Steps_t);

    struct PreHeatConfig_t Cfg;
    ReadData(&Offset, (S8 *)&Cfg, sizeof(struct PreHeatConfig_t), Buff);
    TempConfig_t.ABSNozzlePreheatTemp = Cfg.ABSNozzlePreheatTemp;
    TempConfig_t.ABSBedPreheatTemp = Cfg.ABSBedPreheatTemp;
    PreheatConfigToTemperParamAbs(&TempConfig_t, &TemperSet.Abs);

    Offset = INDEX_OFFSET;
    Offset += sizeof(struct Accele_t);
    struct FansSpeed_t FansSpeed = {0};
    ReadData(&Offset, (S8 *)&FansSpeed, sizeof(struct FansSpeed_t), Buff);
    FansSpeedConfig_t.ABSFansSpeed = FansSpeed.ABSFansSpeed;
    FansSet.Abs.Speed = FansSpeedConfig_t.ABSFansSpeed;

    return 1;
}

VOID CrConfigPlaSave()
{
    TemperParamPlaToPreheatConfig(&TempConfig_t, &TemperSet.Pla);
    FansSpeedConfig_t.PLAFansSpeed = FansSet.Pla.Speed;

    SaveConfig();
}

VOID CrConfigPlaRecover()
{
    TempConfig_t.PLANozzlePreheatTemp = PLA_MATERIAL_PREHEAT_NOZ_TEMP;
    TempConfig_t.PLABedPreheatTemp = PLA_MATERIAL_PREHEAT_BED_TEMP;
    FansSpeedConfig_t.PLAFansSpeed = PLA_MATERIAL_PRE_FAN_SPEED;

    PreheatConfigToTemperParamAbs(&TempConfig_t, &TemperSet.Pla);
    FansSet.Pla.Speed = FansSpeedConfig_t.PLAFansSpeed;
}

S32 CrConfigPlaLoad()
{
    char _ALIGN(4) Buff[CONFIG_FILE_SIZE] = {0};
    S32 Offset = INDEX_OFFSET;

    if ( !AccessStart(Buff) )
    {
        printf("Load Fail\n");
        return -1;
    }

    Offset += sizeof(struct Accele_t) + sizeof(struct FansSpeed_t) + sizeof(float) +
              sizeof(S32) + sizeof(struct Jerk_t) + sizeof(struct Steps_t);
    struct PreHeatConfig_t Cfg;
    ReadData(&Offset, (S8 *)&Cfg, sizeof(struct PreHeatConfig_t), Buff);
    TempConfig_t.PLANozzlePreheatTemp = Cfg.PLANozzlePreheatTemp;
    TempConfig_t.PLABedPreheatTemp = Cfg.PLABedPreheatTemp;
    PreheatConfigToTemperParamPla(&TempConfig_t, &TemperSet.Pla);

    Offset = INDEX_OFFSET;
    Offset += sizeof(struct Accele_t);
    struct FansSpeed_t FansSpeed = {0};
    ReadData(&Offset, (S8 *)&FansSpeed, sizeof(struct FansSpeed_t), Buff);
    FansSpeedConfig_t.PLAFansSpeed = FansSpeed.PLAFansSpeed;
    FansSet.Pla.Speed = FansSpeedConfig_t.PLAFansSpeed;

    return 1;
}

struct ParamFanSpeed_t *GetFansParam()
{
    return &FansSet;
}

struct ParamMaterialTemperSet_t *GetTemperSet()
{
    return &TemperSet;
}

S32 SetTemperSet(struct ParamMaterialTemperSet_t *Set)
{
    if ( !Set )
        return 0;

    TemperSet = *Set;

    return 1;
}

struct ParamMotionParam_t *GetMotionParam()
{
    return &MotionParam;
}

S32 SetMotionParam(struct ParamMotionParam_t *Param)
{
    if ( !Param )
        return 0;

    MotionParam = *Param;

    return 1;
}

S32 SetFansParam(struct ParamFanSpeed_t *Param)
{
    if ( !Param )
        return 0;

    FansSet = *Param;

    return 1;
}

BOOL GetFilamentVolumePattern()
{
    return FilamentVolumePattern;
}

S32  SetFilamentVolumePattern(BOOL Opened)
{
    FilamentVolumePattern = Opened;

    return 1;
}

VOID FansConfigRecover()
{
    struct FansSpeed_t Recover = {
            0,
            PLA_MATERIAL_PRE_FAN_SPEED,
            ABS_MATERIAL_PRE_FAN_SPEED
    };

    memcpy(&FansSpeedConfig_t, &Recover, sizeof(struct FansSpeed_t));

    FansSet.Pla.Speed = FansSpeedConfig_t.PLAFansSpeed;
    FansSet.Abs.Speed = FansSpeedConfig_t.ABSFansSpeed;

    return ;
}

VOID PreheatConfigRecover()
{
    struct PreHeatConfig_t Recover =
    {
          0,
          0,
          ABS_MATERIAL_PREHEAT_BED_TEMP,
          ABS_MATERIAL_PREHEAT_NOZ_TEMP,
          PLA_MATERIAL_PREHEAT_BED_TEMP,
          PLA_MATERIAL_PREHEAT_NOZ_TEMP
    };

    static struct FansSpeed_t FansRecover = {
            0,
            PLA_MATERIAL_PRE_FAN_SPEED,
            ABS_MATERIAL_PRE_FAN_SPEED
    };

    memcpy(&FansSpeedConfig_t, &FansRecover, sizeof(FansSpeed_t));
    memcpy(&TempConfig_t, &Recover, sizeof(PreHeatConfig_t));

    PreheatConfigToTemperParamPla(&TempConfig_t, &TemperSet.Pla);
    PreheatConfigToTemperParamAbs(&TempConfig_t, &TemperSet.Abs);

    return ;
}

VOID FrConfigRecover()
{
    PrintRatio = PRINTF_RATIO;       //打印速率
}

VOID FlowConfigRecover()
{
    FlowRatio = FLOW_RATIO;        //流量比
}

VOID JerkConfigRecover()
{
    struct Jerk_t Recover =
    {
        VX_JERK,
        VY_JERK,
        VZ_JERK,
        VE_JERK,
    };

    memcpy(&JerkConfig_t, &Recover, sizeof(Jerk_t));
    JerkConfigToMotionParamJerk(&JerkConfig_t, &MotionParam);

    return ;
}

VOID StepsConfigRecover()
{
    struct Steps_t Recover = {
            X_STEPS_MM,
            Y_STEPS_MM,
            Z_STEPS_MM,
            E_STEPS_MM
    };

    memcpy(&StepsConfig_t, &Recover, sizeof(Steps_t));

    StepConfigToMotionParamStep(&StepsConfig_t, &MotionParam);

    return ;
}

VOID AcceleConfigRecover()
{
    struct Accele_t Recover = {
        ACCELERATION_ACCEL,
        ACCELERATION_RETRACT,
        ACCELERATION_TRAVEL,
        ACCELERATION_MAX_X,
        ACCELERATION_MAX_Y,
        ACCELERATION_MAX_Z,
        ACCELERATION_MAX_E
    };

    memcpy(&AcceleConfig_t, &Recover, sizeof(Accele_t));

    //将加速度恢复出厂设置
    AccelConfigToMotionParamAccel(&AcceleConfig_t, &MotionParam);

    return ;
}

VOID VelocityConfigRecover()
{
    struct Velocity_t Recover = {
            VELOCITY_V_MAX_X,
            VELOCITY_V_MAX_Y,
            VELOCITY_V_MAX_Z,
            VELOCITY_V_MAX_E,
            VELOCITY_V_MIN,
            VELOCITY_V_TRAV_MIN
    };

    memcpy(&VelocityConfig_t, &Recover, sizeof(Velocity_t));

    VelConfigToMotionParamVel(&VelocityConfig_t, &MotionParam);

    return ;
}

static S32  PrintRatioConfigToParam(S16 *Fr)
{
    if ( !Fr )
        return 0;

    *Fr = PrintRatio;

    return 1;
}

static S32  FlowRatioConfigToParam(S16 *Flow)
{
    if ( !Flow )
        return 0;

    *Flow = FlowRatio;

    return 1;
}

S32 PrintRatioParamToMotionConfig(S16 Fr)
{
    PrintRatio = Fr;

    return 1;
}

S32 FlowRatioParamToMotionConfig(S16 Flow)
{
    FlowRatio = Flow;

    return 1;
}

S32  VelConfigToMotionParamVel(struct Velocity_t *Vel, struct ParamMotionParam_t *Param)
{
    if ( !Vel || !Param )
        return 0;

    Param->TravelVelMinXYZ = Vel->VTravMin;
    Param->StepperXAttr.MaxVel = Vel->VmaxX;
    Param->StepperYAttr.MaxVel = Vel->VmaxY;
    Param->StepperZAttr.MaxVel = Vel->VmaxZ;
    Param->StepperEAttr.Param.MaxVel = Vel->VmaxE;
    //缺少最小速度

    return 1;
}

S32  MotionParamVelToVelConfig(struct ParamMotionParam_t *Param, struct Velocity_t *Vel)
{

    if ( !Vel || !Param )
        return 0;

    Vel->VTravMin = Param->TravelVelMinXYZ;
    Vel->VmaxX = Param->StepperXAttr.MaxVel;
    Vel->VmaxY = Param->StepperYAttr.MaxVel;
    Vel->VmaxZ = Param->StepperZAttr.MaxVel;
    Vel->VmaxE = Param->StepperEAttr.Param.MaxVel;
    //缺少最小速度

    return 1;
}

S32  StepConfigToMotionParamStep(struct Steps_t *Step, struct ParamMotionParam_t *Param)
{
    if ( !Step || !Param )
        return 0;

    Param->StepperXAttr.StepsPerMm = Step->Xsteps;
    Param->StepperYAttr.StepsPerMm = Step->Ysteps;
    Param->StepperZAttr.StepsPerMm = Step->Zsteps;
    Param->StepperEAttr.Param.StepsPerMm = Step->Esteps;

    return 1;
}

S32  MotionParamStepToStepConfig(struct ParamMotionParam_t *Param, struct Steps_t *Step)
{
    if ( !Step || !Param )
        return 0;

    Step->Xsteps = Param->StepperXAttr.StepsPerMm;
    Step->Ysteps = Param->StepperYAttr.StepsPerMm;
    Step->Zsteps = Param->StepperZAttr.StepsPerMm;
    Step->Esteps = Param->StepperEAttr.Param.StepsPerMm;

    return 1;
}

S32  JerkConfigToMotionParamJerk(struct Jerk_t *Jerk, struct ParamMotionParam_t *Param)
{
    if ( !Param || !Jerk )
        return 0;

    Param->StepperXAttr.Jerk = Jerk->VxJerk;
    Param->StepperYAttr.Jerk = Jerk->VyJerk;
    Param->StepperZAttr.Jerk = Jerk->VzJerk;
    Param->StepperEAttr.Param.Jerk = Jerk->VeJerk;

    return 1;
}

S32  MotionParamJerkToJerkConfig(struct ParamMotionParam_t *Param, struct Jerk_t *Jerk)
{
    if ( !Param || !Jerk )
        return 0;

    Jerk->VxJerk = (U16)Param->StepperXAttr.Jerk;
    Jerk->VyJerk = Param->StepperYAttr.Jerk;
    Jerk->VzJerk = Param->StepperZAttr.Jerk;
    Jerk->VeJerk = Param->StepperEAttr.Param.Jerk;

    return 1;
}

S32  AccelConfigToMotionParamAccel(struct Accele_t *Accel, struct ParamMotionParam_t *Param)
{
    if ( !Param || !Accel )
        return 0;

    Param->AccelXYZ = Accel->Accel;
    Param->TravelAccelXYZ = Accel->ATravel;
    Param->StepperXAttr.MaxAccel = Accel->AmaxX;
    Param->StepperYAttr.MaxAccel = Accel->AmaxY;
    Param->StepperZAttr.MaxAccel = Accel->AmaxZ;
    Param->StepperEAttr.Param.MaxAccel = Accel->AmaxE;
    Param->StepperEAttr.RetractAccel = Accel->ARetract;

    return 1;
}

S32  MotionParamAccelToAccelConfig(struct ParamMotionParam_t *Param, struct Accele_t *Accel)
{
    if ( !Param || !Accel )
        return 0;

    Accel->Accel = Param->AccelXYZ ;
    Accel->ATravel = Param->TravelAccelXYZ;
    Accel->AmaxX = Param->StepperXAttr.MaxAccel;
    Accel->AmaxY = Param->StepperYAttr.MaxAccel;
    Accel->AmaxZ = Param->StepperZAttr.MaxAccel;
    Accel->AmaxE = Param->StepperEAttr.Param.MaxAccel;
    Accel->ARetract = Param->StepperEAttr.RetractAccel;

    return 1;
}

S32  FansConfigToFansParam(struct FansSpeed_t *FanSpeed, struct ParamFanSpeed_t *Fans)
{
    if ( !FanSpeed || !Fans )
        return 0;

    Fans->Pla.Speed = FanSpeed->PLAFansSpeed;
    Fans->Abs.Speed = FanSpeed->ABSFansSpeed;

    return 1;
}

S32  FansParamToFansConfig(struct ParamFanSpeed_t *Fans, struct FansSpeed_t *FanSpeed)
{
    if ( !FanSpeed || !Fans )
        return 0;

    FanSpeed->PLAFansSpeed = Fans->Pla.Speed;
    FanSpeed->ABSFansSpeed = Fans->Abs.Speed;

    return 1;
}

S32  PreheatConfigToTemperParamPla(struct PreHeatConfig_t *Cfg, struct ParamMaterialTemper_t *Pla)
{
    if ( !Cfg || !Pla )
        return 0;

    Pla->Nozzle = Cfg->PLANozzlePreheatTemp;
    Pla->Bed = Cfg->PLABedPreheatTemp;

    return 1;
}

S32  PreheatConfigToTemperParamAbs(struct PreHeatConfig_t *Cfg, struct ParamMaterialTemper_t *Abs)
{
    if ( !Cfg || !Abs )
        return 0;

    Abs->Nozzle = Cfg->ABSNozzlePreheatTemp;
    Abs->Bed = Cfg->ABSBedPreheatTemp;

    return 1;
}

S32  TemperParamPlaToPreheatConfig(struct PreHeatConfig_t *Cfg, struct ParamMaterialTemper_t *Pla)
{
    if ( !Cfg || !Pla )
        return 0;

    Cfg->PLANozzlePreheatTemp = Pla->Nozzle;
    Cfg->PLABedPreheatTemp = Pla->Bed;

    return 1;
}

S32  TemperParamAbsToPreheatConfig(struct PreHeatConfig_t *Cfg, struct ParamMaterialTemper_t *Abs)
{
    if ( !Cfg || !Abs )
        return 0;

    Cfg->ABSNozzlePreheatTemp = Abs->Nozzle;
    Cfg->ABSBedPreheatTemp = Abs->Bed;

    return 1;
}

FLOAT *GetFilementConfig()
{
    return &Fil;
}

VOID SetFilementConfig(FLOAT Filement)
{
    Fil = Filement;
}

BOOL AccessFinish(S8 * Buf)
{
    if ( !Buf ) return 0;

    S32 FileFd = -1;
    if ( (FileFd = open(CONFIG_FILENAME, O_CREAT | O_RDWR | O_TRUNC)) < 0 )
      return 0;

    S32 BytesWritten = write(FileFd, Buf, CONFIG_FILE_SIZE);

    close(FileFd);

    return (BytesWritten == CONFIG_FILE_SIZE);
}

BOOL AccessStart(S8 * Buf)
{
    if ( !Buf ) return 0;

    S32 FileFd = -1;
    if ( (FileFd = open(CONFIG_FILENAME,  O_RDONLY)) < 0 )
    {
        rt_kprintf("Open Config file %s fail, use default config\n", CONFIG_FILENAME);
        return 0;
    }

    S32 BytesRead = read(FileFd, Buf, CONFIG_FILE_SIZE);
    rt_kprintf("%s:Read config file %s %d Byte\n", __FUNCTION__, CONFIG_FILENAME, BytesRead);

    if (BytesRead < 0)
        return 0;

    close(FileFd);

    return 1;
}

BOOL WriteData(S32 *Pos, S8* Value, const S8 Size, S8 *Buf)
{
    if ( Size >= CONFIG_FILE_SIZE || *Pos > CONFIG_FILE_SIZE || !Pos || !Value || !Buf )
        return false;

    memcpy(Buf + *Pos, Value, Size);
    (*Pos) += Size;

    return true;
}

BOOL ReadData(S32 *Pos, S8* Value, const S8 Size, S8 *Buf)
{
    if ( Size >= CONFIG_FILE_SIZE || *Pos > CONFIG_FILE_SIZE || !Pos || !Value || !Buf )
        return false;

    memcpy(Value, Buf + *Pos, Size);
    (*Pos) += Size;

    return true;
}
