#include "GcodeG425Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrModel/CrMotion.h"

#define CMD_G425            ("G425")
#define CALIBRATION_MEASUREMENT_UNCERTAIN (1.0) //单位：mm
#define CALIBRATION_OBJECT_CENTER { 264.0, -22.0, -2.0 } //单位：mm
#define CALIBRATION_OBJECT_DIMENSIONS { 10.0, 10.0, 10.0 } //单位：mm
#define CALIBRATION_FEEDRATE_TRAVEL (3000) //mm/min
#define CALIBRATION_NOZZLE_OUTER_DIAMETER (2.0) //mm
#define CALIBRATION_NOZZLE_TIP_HEIGHT (1.0) //mm
#define CALIBRATION_MEASUREMENT_UNKNOWN (5.0) //mm

enum Side_t
{
    SIDE_TOP = 0U,
    SIDE_RIGHT,
    SIDE_FRONT,
    SIDE_LEFT,
    SIDE_BACK,
    SIDE_SIZE
};

struct Measurement_t
{
    XYZFloat_t ObjectCenter;
    FLOAT ObjectSides[SIDE_SIZE];
    FLOAT Backlash[SIDE_SIZE];
    XYZFloat_t PositionErr;
    XYZFloat_t NozzleOuterDim;
};

static XYZFloat_t Dimensions = CALIBRATION_OBJECT_DIMENSIONS;
static S8 ActiveExtruder = 0;

/* 沿着轴测量，直到触发探头，返回原点，返回测量值 */
FLOAT MeasureAxis(enum MoveAxisType_t Axis, int Dir, FLOAT *Backlash, FLOAT Uncertainty)
{
    return 0.0f;
}

VOID MoveToObjectCenter(struct Measurement_t *Measurement, FLOAT Uncertainty)
{
    struct MotionGcode_t Target = {0};
    /* 移动到XY中校准对象的中心，Z轴移到安全距离 */
    Target.Coord.CoorX = Measurement->ObjectCenter.X;
    Target.Coord.CoorY = Measurement->ObjectCenter.Y;
    Target.Coord.CoorZ = Measurement->ObjectCenter.Z + Dimensions.Z / 2 + Uncertainty;
    Target.FeedRate = CALIBRATION_FEEDRATE_TRAVEL / 60.0f;
    MotionModel->PutMotionCoord(Target);  /* 需要等待移动完成 */
}

/* 探测单一方向 */
VOID ProbeSide(struct Measurement_t *Measurement, FLOAT Uncertainty, S8 Side)
{
    FLOAT MeasurementResult;
    enum MoveAxisType_t Axis;
    S32 Direction = 1;
    struct MotionGcode_t Target = {0};

    /* 移动到XY中校准对象的中心，Z轴移到安全距离 */
    MoveToObjectCenter(Measurement, Uncertainty);

    switch ( Side )
    {
        case SIDE_TOP:
        {
            MeasurementResult = MeasureAxis(MOVE_Z_AXIS, -1, &(Measurement->Backlash[SIDE_TOP]), Uncertainty);
            Measurement->ObjectCenter.Z = MeasurementResult - Dimensions.Z / 2;
            Measurement->ObjectSides[SIDE_TOP] = MeasurementResult;
            return;
        }
        case SIDE_RIGHT:
        {
            Axis = MOVE_X_AXIS;
            Direction = -1;
            break;
        }
        case SIDE_FRONT:
        {
            Axis = MOVE_Y_AXIS;
            break;
        }
        case SIDE_LEFT:
        {
            Axis = MOVE_X_AXIS;
            break;
        }
        case SIDE_BACK:
        {
            Axis = MOVE_Y_AXIS;
            Direction = -1;
            break;
        }
        default:
        {
            return;
        }
    }

    Target.Coord = MotionModel->GetLatestCoord();
    /* 剩下X轴或Y轴的探测 */
    if ( Axis == MOVE_X_AXIS )
    {
        Target.Coord.CoorX = Measurement->ObjectCenter.X + (-Direction) *
            (Dimensions.X / 2 + Measurement->NozzleOuterDim.X / 2 + Uncertainty);
    }
    else
    {
        Target.Coord.CoorX = Measurement->ObjectCenter.Y + (-Direction) *
            (Dimensions.Y / 2 + Measurement->NozzleOuterDim.Y / 2 + Uncertainty);
    }
    Target.Coord.CoorZ = Measurement->ObjectSides[SIDE_TOP] - CALIBRATION_NOZZLE_TIP_HEIGHT * 0.7f;
    MotionModel->PutMotionCoord(Target);  /* 需要等待移动完成 */
    MeasurementResult = MeasureAxis(Axis, Direction, &(Measurement->Backlash[Side]), Uncertainty);
    if ( Axis == MOVE_X_AXIS )
    {
        Measurement->ObjectCenter.X = MeasurementResult + Direction *
            (Dimensions.X / 2 + Measurement->NozzleOuterDim.X / 2);
    }
    else
    {
        Measurement->ObjectCenter.Y = MeasurementResult + Direction *
            (Dimensions.Y / 2 + Measurement->NozzleOuterDim.Y / 2);
    }
    Measurement->ObjectSides[Side] = MeasurementResult;
}

/* 探测各个方向 */
VOID ProbeSides(struct Measurement_t *Measurement, FLOAT Uncertainty)
{
    struct XYZFloat_t Center = CALIBRATION_OBJECT_CENTER;
    for ( S8 i = SIDE_TOP; i < SIDE_SIZE; i++ )
    {
        ProbeSide(Measurement, Uncertainty, i);
    }
    Measurement->ObjectCenter.X = (Measurement->ObjectSides[SIDE_LEFT] + Measurement->ObjectSides[SIDE_RIGHT]) / 2;
    Measurement->ObjectCenter.Y = (Measurement->ObjectSides[SIDE_FRONT] + Measurement->ObjectSides[SIDE_BACK]) / 2;
    Measurement->NozzleOuterDim.X = Measurement->ObjectSides[SIDE_RIGHT] - Measurement->ObjectSides[SIDE_LEFT] - Dimensions.X;
    Measurement->NozzleOuterDim.Y = Measurement->ObjectSides[SIDE_BACK] - Measurement->ObjectSides[SIDE_FRONT] - Dimensions.Y;
    MoveToObjectCenter(Measurement, Uncertainty);
    Measurement->PositionErr.X = Center.X - Measurement->ObjectCenter.X;
    Measurement->PositionErr.Y = Center.Y - Measurement->ObjectCenter.Y;
    Measurement->PositionErr.Z = Center.Z - Measurement->ObjectCenter.Z;
}

/* 回差校准 */
VOID CalibrationBacklash(struct Measurement_t *Measurement, FLOAT Uncertainty)
{
    struct MotionGcode_t Target = {0};
    /* 关闭回差补偿 */
    /* TEMPORARY_BACKLASH_CORRECTION(all_off);
       TEMPORARY_BACKLASH_SMOOTHING(0.0f); */
    /* 各个方向进行校准 */
    ProbeSides(Measurement, Uncertainty);
#if 0
    /* 将Measurement中的backlash保存到全局变量backlash中 */
    backlash.distance_mm.x = (Measurement->Backlash[SIDE_LEFT] + Measurement->Backlash[SIDE_RIGHT]) / 2;
    backlash.distance_mm.x = Measurement->Backlash[SIDE_LEFT];
    backlash.distance_mm.x = Measurement->Backlash[SIDE_RIGHT];

    backlash.distance_mm.y = (Measurement->Backlash[SIDE_FRONT] + Measurement->Backlash[SIDE_BACK]) / 2;
    backlash.distance_mm.y = Measurement->Backlash[SIDE_FRONT];
    backlash.distance_mm.y = Measurement->Backlash[SIDE_BACK];

    backlash.distance_mm.z = Measurement->Backlash[SIDE_TOP];
#endif
    /* 开启回差补偿 */
    /* TEMPORARY_BACKLASH_CORRECTION(all_on);
       TEMPORARY_BACKLASH_SMOOTHING(0.0f); */
#if 0
    /* 在允许的方向移动消除回差 */
    Target.Coord.CoorX = MotionModel->GetLatestCoord().CoorX + AXIS_CAN_CALIBRATE(X) * 3;
    Target.Coord.CoorY = MotionModel->GetLatestCoord().CoorY + AXIS_CAN_CALIBRATE(Y) * 3;
    Target.Coord.CoorZ = MotionModel->GetLatestCoord().CoorZ + AXIS_CAN_CALIBRATE(Z) * 3;
    Target.FeedRate = CALIBRATION_FEEDRATE_TRAVEL / 60.0f;
    MotionModel->PutMotionCoord(Target); /* 需要等待执行完成 */

    Target.Coord.CoorX = MotionModel->GetLatestCoord().CoorX - AXIS_CAN_CALIBRATE(X) * 3;
    Target.Coord.CoorY = MotionModel->GetLatestCoord().CoorY - AXIS_CAN_CALIBRATE(Y) * 3;
    Target.Coord.CoorZ = MotionModel->GetLatestCoord().CoorZ - AXIS_CAN_CALIBRATE(Z) * 3;
    MotionModel->PutMotionCoord(Target); /* 需要等待执行完成 */
#endif
}

#ifdef HAS_HOTEND_OFFSET
VOID NormalizeHotendOffsets()
{
    S8 i;
    for ( i = 1; i < HOTENDS; i++ )
    {
        HotendOffset[i] -= HotendOffset[0];
    }
    HotendOffset[0].X = 0;
    HotendOffset[0].Y = 0;
    HotendOffset[0].Z = 0;
}
/* 打印热端偏移 */
VOID ReportHotendOffset()
{
    S8 i;
    for ( i = 1; i < HOTENDS; i++ )
    {
        printf("T%d  Hotend Offset X: %f Y: %f Z: %f\n", i,
            HotendOffset[i].X, HotendOffset[i].Y, HotendOffset[i].Z);
    }
}
#endif /* HAS_HOTEND_OFFSET */

#ifdef HAS_MULTI_HOTEND
VOID SetNozzle(struct Measurement_t *Measurement, S8 Extruder)
    /* 选择喷嘴 */
    if ( Extruder !=  ActiveExtruder)
    {
        MoveToObjectCenter(Measurement, CALIBRATION_MEASUREMENT_UNKNOWN);
#if 0
        /* 选择工具 */
        tool_change(Extruder); /* 未实现 */
#endif
    }
#endif /* HAS_MULTI_HOTEND */

/* 更新坐标（替换错误坐标） */
VOID UpdateMeasurement(struct Measurement_t *Measurement)
{
#if 0
    /* 修改当前位置信息 */
    current_position[axis] += Measurement->pos_error[axis];
    Measurement->obj_center[axis] = true_center[axis];
    Measurement->pos_error[axis] = 0;
#endif
}

/* 喷嘴校准 */
VOID CalibrationToolHead(struct Measurement_t *Measurement, FLOAT Uncertainty, S8 Extruder)
{
    /* 开启回差补偿（未实现） */
    /* TEMPORARY_BACKLASH_CORRECTION(all_on);
       TEMPORARY_BACKLASH_SMOOTHING(0.0f);*/
#ifdef HAS_MULTI_HOTEND
    /* 设置移动喷嘴 */
    SetNozzle(Measurement, Extruder);
#endif /* HAS_MULTI_HOTEND */
    /* 各个方向进行校准 */
    ProbeSides(Measurement, Uncertainty);
#ifdef HAS_HOTEND_OFFSET
    /* 调整热端偏移 */
    HotendOffset[Extruder].X += Measurement->PositionErr.X;
    HotendOffset[Extruder].Y += Measurement->PositionErr.Y;
    HotendOffset[Extruder].Z += Measurement->PositionErr.Z;
    NormalizeHotendOffsets();
#endif /* HAS_HOTEND_OFFSET */

    /* 等待坐标队列排空 */
    /* planner.synchronize(); */
    /* 纠正错误位置 */
    UpdateMeasurement(Measurement);
#if 0
    /* 同步当前坐标 */
    sync_plan_position(); /* 未实现 */
#endif
}

/* 打印测量面板 */
VOID ReportMeasuredFace(struct Measurement_t *Measurement)
{
    printf("Sides:");
    printf("  Top: %f", Measurement->ObjectSides[SIDE_TOP]);
    printf("  Left: %f", Measurement->ObjectSides[SIDE_LEFT]);
    printf("  Right: %f", Measurement->ObjectSides[SIDE_RIGHT]);
    printf("  Front: %f", Measurement->ObjectSides[SIDE_FRONT]);
    printf("  Back: %f", Measurement->ObjectSides[SIDE_BACK]);
    printf("\n");
}

/* 打印测量中心 */
VOID ReportMeasuredCenter(struct Measurement_t *Measurement)
{
    printf("Center:");
    printf("  X: %f", Measurement->ObjectCenter.X);
    printf("  Y: %f", Measurement->ObjectCenter.Y);
    printf("  Z: %f", Measurement->ObjectCenter.Z);
    printf("\n");
}

/* 打印测量回差值 */
VOID ReportMeasuredBacklash(struct Measurement_t *Measurement)
{
    printf("Backlash:");
    printf("  Left: %f", Measurement->Backlash[SIDE_LEFT]);
    printf("  Right: %F", Measurement->Backlash[SIDE_RIGHT]);
    printf("  Front: %F", Measurement->Backlash[SIDE_FRONT]);
    printf("  Back: %F", Measurement->Backlash[SIDE_BACK]);
    printf("  TOP: %F", Measurement->Backlash[SIDE_TOP]);
    printf("\n");
}

/* 打印测量喷嘴尺寸 */
VOID ReportMeasuredNozzleDimension(struct Measurement_t *Measurement)
{
    printf("Nozzle Tip Outer Dimensions:");
    printf("  X: %f", Measurement->NozzleOuterDim.X);
    printf("  Y: %f", Measurement->NozzleOuterDim.Y);
    printf("\n");
}

/* 打印测量坐标误差 */
VOID ReportMeasuredPosErr(struct Measurement_t *Measurement)
{
    printf("T%d", ActiveExtruder);
    printf(" Positional Error:");
    printf("  X: %f", Measurement->PositionErr.X);
    printf("  Y: %f", Measurement->PositionErr.Y);
    printf("  Z: %f", Measurement->PositionErr.Z);
    printf("\n");
}

/* 探头校准并打印位置、误差、回差和热端偏移 */
VOID ProbingAndReport(struct Measurement_t *Measurement, FLOAT Uncertainty)
{
    /* 各个方向进行校准 */
    ProbeSides(Measurement, Uncertainty);

    /* 开始打印信息 */
    printf("\n");
    /* 打印测量面板 */
    ReportMeasuredFace(Measurement);
    /* 打印测量中心 */
    ReportMeasuredCenter(Measurement);
    /* 打印测量回差值 */
    ReportMeasuredBacklash(Measurement);
    /* 打印测量喷嘴尺寸 */
    ReportMeasuredNozzleDimension(Measurement);
    /* 打印测量坐标误差 */
    ReportMeasuredPosErr(Measurement);
#ifdef HAS_HOTEND_OFFSET
    NormalizeHotendOffsets();
    /* 打印热端偏移 */
    ReportHotendOffset();
#endif
}

/* 校准全部 */
VOID CalibrationAll(struct Measurement_t *Measurement)
{
    S8 i;
    struct MotionGcode_t Target = {0};
#ifdef HAS_HOTEND_OFFSET
    /* 重置热端偏移值（未实现） */
    /* reset_hotend_offsets(); */
#endif
    /* 开启回差补偿（未实现） */
    /* TEMPORARY_BACKLASH_CORRECTION(all_on);
       TEMPORARY_BACKLASH_SMOOTHING(0.0f); */
    /* 对所有喷嘴进行粗略的校准 */
    for ( i = 0; i < HOTENDS; i++ )
    {
        CalibrationToolHead(Measurement, CALIBRATION_MEASUREMENT_UNKNOWN, i);
    }
#ifdef HAS_HOTEND_OFFSET
    NormalizeHotendOffsets();
#endif /* HAS_HOTEND_OFFSET */
#ifdef HAS_MULTI_HOTEND
    SetNozzle(Measurement, 0);
#endif /* HAS_MULTI_HOTEND */
    /* 进行回差校准 */
    CalibrationBacklash(Measurement, CALIBRATION_MEASUREMENT_UNCERTAIN);

#ifdef HAS_MULTI_HOTEND
    for ( i = 0; i < HOTENDS; i++ )
    {
        SetNozzle(Measurement, i);
    }
#endif /* HAS_MULTI_HOTEND */

    /* 对所有喷嘴进行精确的校准 */
    for ( i = 0; i < HOTENDS; i++ )
    {
        CalibrationToolHead(Measurement, CALIBRATION_MEASUREMENT_UNCERTAIN, i);
    }

    /* 将喷嘴停在远离校准点的位置（此处用（0,0,?） */
    Target.Coord.CoorX = 0;
    Target.Coord.CoorY = 0;
    Target.Coord.CoorZ = MotionModel->GetLatestCoord().CoorZ;
    Target.FeedRate = CALIBRATION_FEEDRATE_TRAVEL / 60.0f;
    MotionModel->PutMotionCoord(Target); /* 需要等待执行完成 */
}

S32 GcodeG425Parse(const S8 *CmdBuff, VOID *Arg)
{
    /*G425 的操作有 B T U V */
    S8 Cmd[CMD_BUFF_LEN] = {0};
    S32 CmdBuffLen = strlen(CmdBuff), OptLen;
    S32 Offset = strlen(CMD_G425) + sizeof(S8); //加1是越过分隔符
    struct ArgG425_t *CmdArg = (struct ArgG425_t *)Arg;
    S8 Opt[20] = {0};

    if ( Arg == NULL )
    {
        return 0;
    }

    /* 获取命令失败，获取的命令不是G425 */
    if ( (GetGcodeCmd(CmdBuff, Cmd) <= 0) || (strcmp(Cmd, CMD_G425) != 0) ) 
    {
        return 0;
    }

    while ( Offset < CmdBuffLen )  //处理结束
    {
        if ( GetCmdOpt(CmdBuff + Offset, Opt) == 0 )
        {
            break;    //获取参数失败
        }

        OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode G425 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_B:
            {
                CmdArg->HasB = 1;
                break;
            }
            case OPT_T:
            {
                CmdArg->HasT = 1;
                CmdArg->T = (OptLen <= 1 ? ActiveExtruder : atoi(Opt + 1));
                break;
            }
            case OPT_U:
            {
                if ( OptLen <= 1 )
                {
                    CmdArg->HasU = 0;
                }
                else
                {
                    CmdArg->HasU = 1;
                    CmdArg->U = atof(Opt + 1);
                }
                break;
            }
            case OPT_V:
            {
                CmdArg->HasV = 1;
                break;
            }
            default:
            {
                printf("Gcode G425 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}

S32 GcodeG425Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( ArgPtr == NULL )
    {
        return 0;
    }
    struct ArgG425_t *ParamG425 = (struct ArgG425_t *)ArgPtr;
    FLOAT Uncertainty = (ParamG425->HasU ? ParamG425->U : CALIBRATION_MEASUREMENT_UNCERTAIN);
    struct Measurement_t Measurement = {
        .ObjectCenter = CALIBRATION_OBJECT_CENTER,
        .ObjectSides = {0},
        .Backlash = {0},
        .PositionErr = {0},
        .NozzleOuterDim = { CALIBRATION_NOZZLE_OUTER_DIAMETER, CALIBRATION_NOZZLE_OUTER_DIAMETER },
    };

    if ( ParamG425->HasB )
    {
        CalibrationBacklash(&Measurement, Uncertainty);
    }
    else if ( ParamG425->HasT )
    {
        CalibrationToolHead(&Measurement, Uncertainty, ParamG425->T);
    }
    else if ( ParamG425->HasV )
    {
        ProbingAndReport(&Measurement, Uncertainty);
    }
    else
    {
        CalibrationAll(&Measurement);
    }

    return 1;
}

S32 GcodeG425Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

