#ifndef _CEMOTIONMODEL_H
#define _CEMOTIONMODEL_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"
#include "CrModel.h"
#include "CrMotion/StepperApi.h"
#include "Common/Include/CrThread.h"
#include "CrMsgQueue/CommonQueue/CrMsgQueue.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"


#ifndef FIL_DIA
#define FIL_DIA (1.75f)     //单位毫米
#endif

/*
//运动坐标(上报的坐标的内容)
struct MotionCoord_t
{
    float CoordX;   //X轴坐标
    float CoordY;   //Y轴坐标
    float CoordZ;   //Z轴坐标
    float CoordE;   //E轴坐标
};
*/
#define HOME_OPT_O      (0x01 << 1)
#define HOME_OPT_R      (0x01 << 2)
#define HOME_OPT_X      (0x01 << 3)
#define HOME_OPT_Y      (0x01 << 4)
#define HOME_OPT_Z      (0x01 << 5)

#define PAUSE_PARK_RETRACT_FEEDRATE         60  // (mm/s) Initial retract feedrate.
#define PAUSE_PARK_RETRACT_LENGTH            2  // (mm) Initial retract.

#define FILAMENT_CHANGE_SLOW_LOAD_FEEDRATE   6  // (mm/s) Slow move when starting load.
#define FILAMENT_CHANGE_SLOW_LOAD_LENGTH     0  // (mm) Slow length, to allow time to insert material.

#define X_MIN_POS 0
#define Y_MIN_POS 0
#define Z_MIN_POS 0
#define BED_WIDTH  200    //Y轴方向大小
#define BED_LENGTH 200    //X轴方向大小
#define MAX_HIGH   300    //打印空间的最大高度
#define X_MAX_POS (X_MIN_POS + BED_LENGTH)
#define Y_MAX_POS (Y_MIN_POS + BED_WIDTH)
#define Z_MAX_POS MAX_HIGH

//#define NOZZLE_PARK_POINT { (X_MIN_POS + 10), (Y_MAX_POS - 10), 20, PAUSE_PARK_RETRACT_LENGTH }
#define NOZZLE_PARK_POINT { (X_MIN_POS + 10), (Y_MAX_POS - 10), 20 }
  //#define NOZZLE_PARK_X_ONLY          // X move only is required to park
  //#define NOZZLE_PARK_Y_ONLY          // Y move only is required to park
#define NOZZLE_PARK_Z_RAISE_MIN   2   // (mm) Always raise Z by at least this distance
#define NOZZLE_PARK_XY_FEEDRATE 100   // (mm/s) X and Y axes feedrate (also used for delta Z axis)
#define NOZZLE_PARK_Z_FEEDRATE    5   // (mm/s) Z axis feedrate (not used for delta printers)


#define STR_SOFT_ENDSTOPS                   "Soft endstops: "
#define STR_SOFT_MIN                        "  Min: "
#define STR_SOFT_MAX                        "  Max: "


//容积or长度模式
typedef BOOL CalcInVolume_t;

enum StepperType_t
{
     StepperX  = 1,
     StepperY,
     StepperZ,
     StepperE
};
/* 运动单位 */
enum MotionUnitType_t
{
    Millimeter, //毫米
    Inch        //英寸
};
//这个是进行电机初始化的接口
struct MotionDriver_t
{
    struct StepperPins_t StepperX_Pin;
    struct StepperPins_t StepperY_Pin;
    struct StepperPins_t StepperZ_Pin;
    struct StepperPins_t StepperE_Pin;
    S8 *EndStopX_PinName;   //限位开关名称
    S8 *EndStopY_PinName;
    S8 *EndStopZ_PinName;
    U8 EndStopX_TrigLevel;   //限位开关触发电平
    U8 EndStopY_TrigLevel;
    U8 EndStopZ_TrigLevel;
};

//初始化
struct MotionAttr_t
{
    struct StepperAttr_t       AttrX;
    struct StepperPos_t        PosX;
    struct StepperMotionAttr_t MotionAttrX;
    struct StepperAttr_t       AttrY;
    struct StepperPos_t        PosY;
    struct StepperMotionAttr_t MotionAttrY;
    struct StepperAttr_t       AttrZ;
    struct StepperPos_t        PosZ;
    struct StepperMotionAttr_t MotionAttrZ;
    struct StepperAttr_t       AttrE;
    struct StepperPos_t        PosE;
    struct StepperMotionAttr_t MotionAttrE;
};

struct FwRetract_t
{
    FLOAT FeedRate;             //回抽速率 - M207 F
    FLOAT RetractLength;        //回抽长度 - M207 S
    FLOAT SwapRetractLength;    //回抽交换长度 (多挤出机) - M207 W
    FLOAT RetractZ_Raise;       //Z轴抬升高度 - M207 Z

    FLOAT RecoverFeedRate;      //恢复速率 - M208 F
    FLOAT SwapRecoverFeedRate;  //交换恢复速率 - M208 R
    FLOAT RecoverLengthExtra;   //额外的恢复长度 - M208 S
    FLOAT SwapRecoverLength;    //交换恢复长度 - M208 W

    BOOL  FwRetractEnable;      //M209 S
};


//设置
class CrMotion : public CrModel
{
public:
    CrMotion(struct MotionDriver_t &Driver, struct MotionAttr_t &Attr);
    ~CrMotion();

    static FLOAT Deltamm[AxisNum];//211213 wangcong

    //运动相关
    S32  StopMotionUrgently();                //快速停止
    S32  StopMotionSafely();                  //运动完停止,然后归零
    VOID AutoMoveHome(U8 Mark);               //自动回零
    VOID DisableAllSteppers();                //
    S32 PutMotionCoord(struct MotionGcode_t& Coord);             //最新坐标放入坐标队列

    const struct MotionCoord_t& GetLatestCoord();                //取最后放入队列的坐标备份
    VOID  SetLatestCoord(struct MotionCoord_t& Coord);           //设置最后放入队列的坐标备份
    
    VOID Exec();
    /*************************** extened by TingFengXuan ********************************/
    VOID SetRelativeMotion();                   //设置相对坐标运动模式
    VOID SetAbsluteMotion();                    //设置绝对坐标运动模式
    VOID SetRelativeExtrude();                  //设置相对坐标挤出模式
    VOID SetAbsluteExtrude();                   //设置绝对坐标挤出模式
    BOOL IsRelativeMotionMode();                //是否相对运动模式
    BOOL IsRelativeExtrudeMode();               //是否相对挤出模式
    VOID MotionPause();
    VOID MotionResume();
    VOID MotionStop();
    
    /**
     * TO DO:
     */
    VOID EndStopGloballyMark(BOOL Mark);        //限位开关全局有效/归零有效
    BOOL EndStopStatus(enum Axis_t axis);      //限位开关状态
    VOID SetAxisHomeOffset(enum Axis_t axis, FLOAT offset);//设置轴原点偏移
    FLOAT GetAxisHomeOffset(enum Axis_t axis);              //获取轴原点偏移

    struct FeedrateAttr_t GetMotionFeedRateAttr();
    VOID SetMotionFeedRateAttr(struct FeedrateAttr_t &Feedrate);

    /****************************** extened by guojin·chen ******************************/
    VOID DisableStepper(enum StepperType_t Type);     //失能电机
    VOID EnableStepper(enum StepperType_t Type);      //使能电机
    VOID ClearPlanQueue();                            //清空规划队列

    BOOL IsMotionUnitTypeInch();           //运动单位是否为英寸
    VOID SetMotionUnitType(enum MotionUnitType_t Unit); //设置运动单位
    BOOL IsMotionMoveFinished();            //运动是否完成
    VOID SetStepperMotion(struct StepperMotionAttr_t MotionAttr, enum StepperType_t Axis);
    struct StepperMotionAttr_t GetSetpperMotion(enum StepperType_t Axis);
    S32 GetStepperFd(enum StepperType_t Axis);

    struct MotionCoord_t& GetPauseCoord() {return PauseCoord;}
    struct MotionPositionMsg_t GetCurPosition();
    
    struct FwRetract_t& GetFwRetractSetting() {return FwRetractSetting;}
    VOID SetFwRetractSetting(struct FwRetract_t &FwRetract){FwRetractSetting = FwRetract; }
    BOOL IsVolumePatternEnable() {return EnableVolumePattern;}
    VOID SetVolumePattern(BOOL Enable) { EnableVolumePattern = Enable;}
    FLOAT GetDiameter() {return Diameter;}
    VOID SetDiameter(FLOAT FilaDia) { Diameter = FilaDia; };
    VOID SetSoftEndStop(U8 Enabled);
    BOOL SoftEndStopEnabled();
    BOOL HasEndStop();
private:

    S32 HandleCurPosMsg(S8 *Buff, S32 Len);         //请求当前位置
    S32 HandleSetMotionAttrMsg(S8 *Buff, S32 Len, struct ViewMotionMsg_t *OutAttr);
    S32 HandleStepperMoveMsg(S8 *Buff, S32 Len);

    VOID CurPosMsgOpt();
    VOID SetMotionAttrMsgOpt();
    VOID StepperMoveMsgOpt();
    
    VOID UnitConversionOutput();
    VOID UnitConversionInput();

    BOOL AllSteppersEnabled();
    VOID EnableAllSteppers();

private:

    S32  StepperX_Fd;
    S32  StepperY_Fd;
    S32  StepperZ_Fd;
    S32  StepperE_Fd;
    S32  EndStopX_Fd;
    S32  EndStopY_Fd;
    S32  EndStopZ_Fd;
    struct MotionPositionMsg_t CurPos;      //当前位置
    struct MotionCoord_t       LatestCoord; //队列中，最新放入的坐标

    //todo：全部改为被动式
    CrMsg_t SendMotionAttrMsg;              //发送运动属性
    CrMsg_t SendPosMsg;                     //上报位置信息用共享队列

    CrMsgQueue_t RecvReqPosMsg;             //接收位置请求
    CrMsgQueue_t RecvStepperMoveMsg;        //接收电机移动
    CrMsgQueue_t RecvSetMotionAttrMsg;      //接收改变属性（用到的地方用相同的名字）
    
    /*************************** extened by TingFengXuan ********************************/
    U8 IsRelativeMotion;                    //相对运动模式
    U8 IsRelativeExtrude;                   //相对挤出(独立于运动模式)
    struct MotionCoord_t       PauseCoord;  //暂停位置的坐标
    BOOL EndStopGlobaleMark;               //限位开关全局标识
    /****************************** extened by guojin·chen ******************************/
    enum MotionUnitType_t  MotionUnit;  //运动的单位
    struct FwRetract_t FwRetractSetting;    //固件回抽设置
	
    FLOAT  Diameter;   //长丝直径
    BOOL   EnableVolumePattern;  //容积模式

    /**
     * TO DO: calculate ticks thread.
     */
    CrPthread_t CalcThread;
    S8 TicksThreadBuff[1024];
    static VOID *CalcTicksThread(VOID *Arg);
};

extern CrMotion *MotionModel;
extern struct XYZFloat_t HotendOffset[HOTENDS];

#endif

