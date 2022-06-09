#include "GcodeG26Handle.h"

#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "../../GcodeType.h"
#include "../../../../CrModel/CrMotion.h"
#include "../../../../CrModel/CrHeaterManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_G26 "G26"
#define BED_DEFAULT_TEMPERATURE    (60U)            //热床默认温度（60℃）
#define BED_MAX_TEMPERATURE        (BED_MAX_TEMP)   //热床最高温度
#define BED_MIN_TEMPERATURE        (40U)            //热床最低温度（40℃）

#define FILAMENT_DEFAULT_DIAMETER   (FIL_DIA)   //长丝默认直径（1.75mm）
#define FILAMENT_MAX_DIAMETER       (4.0f)      //长丝最大直径（4.0mm）
#define FILAMENT_MIN_DIAMETER       (1.0f)      //长丝最小直径（1.0mm）

#define HOTEND_DEFAULT_TEMPERATURE     (205U)  //热端默认温度（205℃）
#define HOTEND_MAX_TEMPERATURE         (275U)  //热端最大温度（275℃）
#define HOTEND_MIN_TEMPERATURE         (165U)  //热端最低温度（165℃）

#define LAYER_DEFAULT_HEIGHT    (0.2f) //层默认高度（0.2mm）
#define LAYER_MAX_HEIGHT        (2.0f) //层最大高度（2.0mm）
#define LAYER_MIN_HEIGHT        (0.0f) //层最小高度（0.0mm）

#define OOZE_AMOUNT 0.3 //挤出量

#define PRIME_DEFAULT_LENGTH    (10.0f) //灌注默认长度
#define PRIME_MAX_LENGTH        (25.0f) //灌注最大长度
#define PRIME_MIN_LENGTH        (0.0f)  //灌注最小长度

#define RETRACTION_DEFAULT_MULTIPLIER (1.0f)  // 回缩默认倍数
#define RETRACTION_MAX_MULTIPLIER     (15.0f) // 回缩最大倍数
#define RETRACTION_MIN_MULTIPLIER     (0.05f) // 回缩最小倍数

#define NOZZLE_DEFAULT_SIZE (0.4f) //喷嘴默认尺寸（0.4mm）
#define NOZZLE_MAX_SIZE     (2.0f) //喷嘴最大尺寸（2.0mm）
#define NOZZLE_MIN_SIZE     (0.1f) //喷嘴最小尺寸（0.1mm）

#define DEFAULT_RANDOM_DEVIATION (50.0f) //默认随机偏差（50.0）

#define GRID_MAX_POINTS (9U) // 网格最大点数

#define Z_CLEARANCE_BETWEEN_PROBES (5U)

enum HeaterType_t
{
    HEATER_BED = 0U,    /* 热床加热器 */
    HEATER_NOZZLE       /* 喷嘴加热器 */
};

static FLOAT RetractionMultiplier, LayerHeight;

static S32 CheckCoordReachable(struct MotionGcode_t *Coord)
{
    /* 检查坐标点 (x,y) 是否可到达 */
    return 1;
}

/* 设置调平使能 */
static void SetBedLeveling(BOOL Enable)
{

}

/* 开启加热器，等待达到温度 */
/* true: 达到温度 false: 达不到温度 */
static BOOL TurnOnHeater(enum HeaterType_t HeaterType, S16 Temperature)
{
    struct HeaterManagerAttr_t HeaterAttr = {0};
    CrHeaterManager *HeaterManager = (HeaterType == HEATER_BED ? BedHeater : ProbeHeater);
    
    /* 获取熟悉 */
    HeaterAttr = BedHeater->CrHeaterMagagerGetAttr();
    /* 设置温度 */
    HeaterAttr.TargetTemper = Temperature;
    BedHeater->CrHeaterMagagerSetAttr(HeaterAttr);
    /* 开启加热器 */
    BedHeater->CrHeaterManagerStart();
    /* 等待温度到达，返回是否达到温度值 */
    if ( BedHeater->ReachedTargetTemper() == false )
    {
        return false;
    }
    return true;
}

/* 喷嘴注入，返回执行情况 */
static BOOL PrimeNozzle(S8 PrimeFlag, FLOAT PrimeLength)
{
    return true;
}

/* 关闭加热器 */
static void TurnOffHeater(enum HeaterType_t HeaterType)
{
    static CrHeaterManager *HeaterManager = (HeaterType == HEATER_BED ? BedHeater : ProbeHeater);
    /* 接口内部会将温度降为0 */
    HeaterManager->CrHeaterManagerStop();
}

/* 绘制单层图案 */
static void DrawSignalLayerPattern()
{

}

S32 GcodeG26Parse(const S8 *CmdBuff, VOID *Arg)
{
    /* G26 操作有 B C D F H I K L O P Q R S U X Y */
    S8 Cmd[CMD_BUFF_LEN] = {0};
    S8 Opt[20] = {0};
    struct ArgG26_t *CmdArg = (struct ArgG26_t *)Arg;
    S32 CmdBuffLen, Offset, OptLen, OptVal, RetVal = 1;
    if ( Arg == NULL )
    {
        return CrErr;
    }

    if ( (GetGcodeCmd(CmdBuff, Cmd) == 0) || (strcmp(Cmd, CMD_G26) != 0) )
    {
        return GCODE_CMD_ERROR;
    }

    CmdBuffLen = strlen(CmdBuff);
    Offset = strlen(CMD_G26) + sizeof(S8);
    while ( Offset < CmdBuffLen )
    {
        if ( GetCmdOpt(CmdBuff + Offset, Opt) == 0 )
        {
            break;
        }
        OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode G26 has no param opt = %s\n", CmdBuff);
        }
        switch ( Opt[0] )
        {
            case OPT_B:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasB = 0;
                }
                else
                {
                    CmdArg->HasB = 1;
                    CmdArg->B = atoi(Opt + 1);
                    if ( (CmdArg->B > BED_MAX_TEMP - 10) || (CmdArg->B < BED_MIN_TEMPERATURE) )
                    {
                        RetVal = CrErr;
                        printf("Bed temperature is invalid (%d - %d)\n",
                               BED_MIN_TEMPERATURE, (BED_MAX_TEMPERATURE - 10));
                    }
                }
                break;
            }
            case OPT_C:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasC = 0;
                }
                else
                {
                    CmdArg->HasC = 1;
                    OptVal = atoi(Opt + 1);
                    CmdArg->C = (BOOL)(OptVal < 0 ? 0 : (OptVal > 255 ? 255 : OptVal));
                }
                break;
            }
            case OPT_D:
            {
                CmdArg->HasD = 1;
                break;
            }
            case OPT_F:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasF = 0;
                }
                else
                {
                    CmdArg->HasF = 1;
                    CmdArg->F = atof(Opt + 1);
                    if ( (CmdArg->F > FILAMENT_MAX_DIAMETER) || (CmdArg->F < FILAMENT_MIN_DIAMETER) )
                    {
                        RetVal = CrErr;
                        printf("Filament diameter is invalid (%f - %f)\n", 
                               FILAMENT_MIN_DIAMETER, FILAMENT_MAX_DIAMETER);
                    }
                }
                break;
            }
            case OPT_H:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasH = 0;
                }
                else
                {
                    CmdArg->HasH = 1;
                    CmdArg->H = atoi(Opt + 1);
                    if ( (CmdArg->H > HOTEND_MAX_TEMPERATURE - 15) || (CmdArg->H < HOTEND_MIN_TEMPERATURE) )
                    {
                        RetVal = CrErr;
                        printf("Hotend temperature is invalid (%d - %d)\n",
                               HOTEND_MIN_TEMPERATURE, (HOTEND_MAX_TEMPERATURE - 15));
                    }
                }
                break;
            }
            case OPT_I:
            {
                break;
            }
            case OPT_K:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasK = 0;
                }
                else
                {
                    CmdArg->HasK = 1;
                    OptVal = atoi(Opt + 1);
                    CmdArg->K = (BOOL)(OptVal < 0 ? 0 : (OptVal > 255 ? 255 : OptVal));
                }
                break;
            }
            case OPT_L:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasL = 0;
                }
                else
                {
                    CmdArg->HasL = 1;
                    CmdArg->L = atof(Opt + 1);
                    if ( (CmdArg->L > LAYER_MAX_HEIGHT) || (CmdArg->L < LAYER_MIN_HEIGHT) )
                    {
                        RetVal = CrErr;
                        printf("Layer height is invalid (%f - %f)\n", LAYER_MIN_HEIGHT, LAYER_MAX_HEIGHT);
                    }
                }
                break;
            }
            case OPT_O:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasO = 0;
                }
                else
                {
                    CmdArg->HasO = 1;
                    CmdArg->O = atof(Opt + 1);
                }
                break;
            }
            case OPT_P:
            {
                CmdArg->HasP = 1;
                if ( OptLen <= 1 )
                {
#ifdef HAS_LCD_MENU
                    CmdArg->P = -1;
#else
                    RetVal = CrErr;
                    printf("Prime length must be spectified\n");
#endif /* HAS_LCD_MENU */
                }
                else
                {
                    CmdArg->P = atof(Opt + 1);
                    if ( (CmdArg->P > PRIME_MAX_LENGTH) || (CmdArg->P < PRIME_MIN_LENGTH) )
                    {
                        RetVal = CrErr;
                        printf("Prime length is invalid (%f - %f)\n", PRIME_MIN_LENGTH, PRIME_MAX_LENGTH);
                    }
                }
                break;
            }
            case OPT_Q:
            {
                CmdArg->HasQ = 1;
                if ( OptLen <= 1 )
                {
                    RetVal = CrErr;
                    printf("Retraction multiplite must be specified.\n");
                }
                else
                {
                    CmdArg->Q = atof(Opt + 1);
                    if ( (CmdArg->Q > RETRACTION_MAX_MULTIPLIER) || (CmdArg->Q < RETRACTION_MIN_MULTIPLIER) )
                    {
                        RetVal = CrErr;
                        printf("Retraction multiplite is invalid (%f - %f)\n",
                               RETRACTION_MIN_MULTIPLIER, RETRACTION_MAX_MULTIPLIER);
                    }
                }
                break;
            }
            case OPT_R:
            {
                /*  */
                CmdArg->HasR = 1;
                CmdArg->R = (OptLen <= 1 ? (GRID_MAX_POINTS + 1) : atoi(Opt + 1));
                break;
            }
            case OPT_S:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasS = 0;
                }
                else
                {
                    CmdArg->HasS = 1;
                    CmdArg->S = atof(Opt + 1);
                    if ( (CmdArg->S > NOZZLE_MAX_SIZE) || (CmdArg->S < NOZZLE_MIN_SIZE) )
                    {
                        RetVal = CrErr;
                        printf("Nozzle size is invalid (%f - %f)", NOZZLE_MIN_SIZE, NOZZLE_MAX_SIZE);
                    }
                }
                break;
            }
            case OPT_U:
            {
                CmdArg->HasU = 1;
                CmdArg->U = (OptLen <= 1 ? DEFAULT_RANDOM_DEVIATION : atof(Opt + 1));
                break;
            }
            case OPT_X:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasX = 0;
                }
                else
                {
                    CmdArg->HasX = 1;
                    CmdArg->X = atof(Opt + 1);
                }
                break;
            }
            case OPT_Y:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasY = 0;
                }
                else
                {
                    CmdArg->HasY = 1;
                    CmdArg->Y = atof(Opt + 1);
                }
                break;
            }
            default:
            {
                printf("Gcode G26 has no define opt = %s\n", CmdBuff);
                break;
            }
        }
        Offset += OptLen + sizeof(S8);
    }
    /* R的特殊处理，行613 */
    return RetVal;
}

S32 GcodeG26Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( (BedHeater == NULL) || (ProbeHeater == NULL) || (ArgPtr == NULL) || (ReplyResult == NULL) )
    {
        return 0;
    }
    struct ArgG26_t *ParamG26 = (struct ArgG26_t *)ArgPtr;
    RetractionMultiplier = (ParamG26->HasQ ? ParamG26->Q : RETRACTION_DEFAULT_MULTIPLIER);
    FLOAT LayerHeight = (ParamG26->HasL ? ParamG26->L : LAYER_DEFAULT_HEIGHT),
          PrimeLength = (ParamG26->HasP ? ParamG26->P : PRIME_DEFAULT_LENGTH);
    S16 BedTemperature = (ParamG26->HasB ? ParamG26->B : BED_DEFAULT_TEMPERATURE),
        HotendTemperature = (ParamG26->HasH ? ParamG26->H : HOTEND_DEFAULT_TEMPERATURE);
    S8 PrimeFlag = 0;
    FLOAT NozzleSize = (ParamG26->HasS ? ParamG26->S : NOZZLE_DEFAULT_SIZE),
          FilamentDiameter = (ParamG26->HasF ? ParamG26->F : FILAMENT_DEFAULT_DIAMETER),
          OozeAmout = (ParamG26->HasO ? ParamG26->O : OOZE_AMOUNT);
    BOOL ContinueWithClosest = (ParamG26->HasC ? ParamG26->C : false),
         KeepHeatersOn = (ParamG26->HasK ? ParamG26->K : false),
         BedLevelingEnable = !(ParamG26->HasD ? true : false);
    struct MotionGcode_t Target = {0};
    struct MotionCoord_t CurCoord = MotionModel->GetLatestCoord();

    Target.Coord.CoorX = (ParamG26->HasX ? ParamG26->X : CurCoord.CoorX);
    Target.Coord.CoorY = (ParamG26->HasY ? ParamG26->Y : CurCoord.CoorY);

#ifdef HAS_LCD_MENU
    PrimeFlag = (ParamG26->HasP ? (ParamG26-P < 0 ? -1 : 0) : 0);
#else
    PrimeFlag = (ParamG26->HasP ? 1 : 0);
#endif /* HAS_LCD_MENU */


    /* 检查坐标是否可达 */
    if ( CheckCoordReachable(&Target) == 0 )
    {
        printf("error: The target position is unreachable. \n");
        return CrErr;
    }

    /* 调平使能设置 */
    SetBedLeveling(BedLevelingEnable);

    /* 开启热床和喷嘴的加热器 */
    if ( TurnOnHeater(HEATER_BED, BedTemperature) && TurnOnHeater(HEATER_NOZZLE, HotendTemperature) )
    {
        Target.Coord.CoorE = 0.0f;
        /* 需要注入，且注入成功 */
        if ( (PrimeFlag != 0) && (PrimeNozzle(PrimeFlag, PrimeLength) == true) )
        {
            /* 当前状态：喷嘴预热，热床预热，喷嘴已灌注 */
            Target.Coord.CoorZ = LayerHeight;
            Target.Coord.CoorE += OozeAmout;
            MotionModel->PutMotionCoord(Target);
            /* 绘制单层图案 */
            DrawSignalLayerPattern();
        }
    }

    Target.Coord.CoorE = -1.0f * RetractionMultiplier; /* 回缩 */
    Target.Coord.CoorZ = Z_CLEARANCE_BETWEEN_PROBES; /* 抬起喷嘴 */
    MotionModel->PutMotionCoord(Target);

    if ( KeepHeatersOn == false )
    {
        /* 关闭加热器 */
        TurnOffHeater(HEATER_BED);
        TurnOffHeater(HEATER_NOZZLE);
    }
    return 1;
}

S32 GcodeG26Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
