#ifndef _CRMSGTYPE_H
#define _CRMSGTYPE_H

#include "CrType.h"

#define MAX_PATH_LENGTH       128
#define FILE_NAME_LENGTH      32
#define PAGE_FILE_COUNT       5
#define SUFFIX_FILE_SIZE      10
#define MAX_MSG_LENGTH        60

#define MOVEAXISMSG_PRI 1

/*********************Model MsgType************************/
struct FileInfo_t
{
    S8 Name[FILE_NAME_LENGTH];
    BOOL IsDirectory;
};

enum DevStatus_t
{
    DEV_PLUCKED,
    DEV_INSERTED
};

//SD卡插入状态
struct SdStatusMsg_t
{
    BOOL Inserted;  //插入sd卡为TURE，未插入为FALSE
};

//文件列表/*model to view*/
struct SdCardPageFileListMsg_t
{
    S8  Path[MAX_PATH_LENGTH];   //当前文件路径
    S32 MaxCount;   //当前文件路径下的文件总个数
    S32 PageNo;     //当前页的页号
    S32 Count;      //当前页的文件个数
    struct FileInfo_t FileList[PAGE_FILE_COUNT];   //包含文件信息的文件列表
};

//打印文件名 /*model to view 打印主页*/
struct SdCardPrintFileMsg_t
{
    S8 Name[FILE_NAME_LENGTH];
};

enum PrintStatus_t
{
    STOP_PRINT = 0, //停止打印
    PRINTING,       //开始打印
    PAUSE_PRINT,    //暂停打印
    RESUME_PRINT,    //恢复打印
//    PRINTF_IDLE    //恢复打印
};

//打印状态
//struct PrintStatus_t
//{
//    PrintStatus_t Status;
//};

//打印进度
struct PrintInfoMsg_t
{
//    struct PrintStatus_t  Status;
    enum PrintStatus_t Status;
    S32 Progress;       //打印进度
    S32 TimeElapsed;   //打印当前消耗的时间
};

struct ParamMaterialTemper_t
{
    S32 Nozzle;
    S32 Bed;
//    S32 FansSpeed;
};

struct ParamMaterialTemperSet_t
{
    struct ParamMaterialTemper_t Pla;
    struct ParamMaterialTemper_t Abs;
};

//电机的运动参数
struct MotionParamXYZ_t
{
    S32 MaxVel;
    S32 MaxAccel;    //最大加速度
//    S32 Jerk;    //加速度的导出
    FLOAT Jerk;    //加速度的导出
    FLOAT StepsPerMm;   //每mm的步数
};

struct MotionParamE_t
{
    struct MotionParamXYZ_t Param;
    S32 RetractAccel;   //回抽加速度
};

struct ParamMotionParam_t
{
    struct MotionParamXYZ_t StepperXAttr;
    struct MotionParamXYZ_t StepperYAttr;
    struct MotionParamXYZ_t StepperZAttr;
    struct MotionParamE_t StepperEAttr;
    S32 TravelAccelXYZ;   //不挤料时的合加速度
    S32 AccelXYZ;         //XY轴的合加速度
    S32 TravelVelMinXYZ;  //不挤料时的最小速度
    S32 VelMinXYZ;        //挤料时的最小速度
    S16 PrintRatio;       //打印速率
    S16 FlowRatio;        //流量比
};

//PID参数
struct PidAttr_t
{
    float   Kp;         //
    float   Ki;         //
    float   Kd;         //

};
struct PidParam_t
{
    float  DeadZone;               //死区(输出在此范围内的值，则不做任何操作)
    float  IntegralSaturation;     //积分饱和值
    float  SystemSaturation;       //系统饱和值
};

struct PwmControllerAttr_t
{
    S32 Freq;    //频率
    S32 Duty;    //占空
    S32 Times;   //if Times=0, the pwm will run forever
    S32 DefaultLevel;    //默认高低电平
};

//上报的内容
struct HeaterManagerAttr_t
{
    struct PidAttr_t  PidAttr;
    struct PidParam_t PidParam;
    struct PwmControllerAttr_t HeaterAttr;

    FLOAT TargetTemper;
    FLOAT CurTemper;
    BOOL  ReachedTargetTemper;
//    S32   FanSpeed;
};

//风扇的速度
struct FanSpeed_t
{
    S32 Speed;
};

//参数model的风扇速度
struct ParamFanSpeed_t
{
    struct FanSpeed_t Pla;
    struct FanSpeed_t Abs;
};

struct TemperInfo_t
{
    S32 Nozzle;
    S32 Bed;
//    S32 FanSpeed;
};

//上报的温度的内容
struct HeaterTemperInfo_t
{
    struct TemperInfo_t TargetTemper;
    struct TemperInfo_t CurTemper;
};

//上报位置信息的结构()
struct MotionPositionMsg_t
{
    FLOAT PosX;   //X轴位置
    FLOAT PosY;   //Y轴位置
    FLOAT PosZ;   //Z轴位置
    FLOAT PosE;   //E轴位置
    S16   PrintSpeed; //100为标准速度，小于100表示减慢速度，100表示加快打印速度
    U8    StepperX_Enable;   //电机X使能标识
    U8    StepperY_Enable;   //电机Y使能标识
    U8    StepperZ_Enable;   //电机Z使能标识
    U8    HomeX;     //归零标识
    U8    HomeY;     //归零标识
    U8    HomeZ;     //归零标识
};

/*********************Model MsgType************************/

/********************view MsgType*************************/

enum ParamOptMsgAction_t
{
    LOAD_ALL_FORM_FILE = 1,
    SAVE_ALL_TO_FILE,
    LOAD_ALL_DEFAULT_PARAM,
    LOAD_PLA_PARAM,
    SAVE_PLA_PARAM,
    LOAD_PLA_DEFAULT_PARAM,
    LOAD_ABS_PARAM,
    SAVE_ABS_PARAM,
    LOAD_ABS_DEFAULT_PARAM
};

//对param model 所有参数的操作
struct ViewParamOptActionMsg_t
{
    enum ParamOptMsgAction_t Action;   //参数动作：0加载默认文件参数，1保存参数至默认文件，2恢复默认参数。
};

enum MotionMsgAction_t
{
    SET_MOTION_MSG = 1,
    GET_MOTION_MSG
};

//对param model 运动参数操作
struct ViewMotionMsg_t
{
    struct ParamMotionParam_t MotionParam;
    BOOL  FilamentPatternAction;    //E轴挤出的计算模式：1打开容积模式，2关闭容积模式
    enum MotionMsgAction_t  Action; //1Set参数，2Get参数
    FLOAT Diameter;                 //直径

};

enum TargetTemperSetMsgAction_t
{
//    SET_MOTION_MSG = 0,
    SET_TEMPER_MSG = 1,
    GET_TEMPER_MSG,
//    SAVE_PLA_TEMPER_MSG,
//    LOAD_PLA_TEMPER_MSG,
//    LOAD_DEFAULT_PLA_TEMPER_MSG,
//    SAVE_ABS_TEMPER_MSG,
//    LOAD_ABS_TEMPER_MSG,
//    LOAD_DEFAULT_ABS_TEMPER_MSG,
};

//设置和操作温度参数的结构体
struct ViewTargetTemperSetMsg_t
{
    struct ParamMaterialTemperSet_t Set;
    enum TargetTemperSetMsgAction_t Action;//操作的类型：0无动作，1设置温度 2请求参数
};

enum PrintCmdAction
{
    CMD_STOP = 1,
    CMD_START,
    CMD_PAUSE,
    CMD_RESUME
};

//打印动作
struct ViewPrintCmdMsg_t
{
    enum PrintCmdAction Action;
};

//请求打印进度
struct ViewPrintInfoMsg_t
{
    S32 ReqProcess;
};

//View 请求获取页的内容  /*view to model*/
struct ViewPageInfoMsg_t
{
    S8 Path[MAX_PATH_LENGTH];     //文件路径
    S8 PageNo;
    S8 PageSize;
    S8 Suffix[SUFFIX_FILE_SIZE];  //筛选文件类型
};

//View 请求设置打印文件信息
struct ViewSetPrintFileMsg_t
{
    S8 Path[MAX_PATH_LENGTH];
    S32 Index;
    S8 Suffix[SUFFIX_FILE_SIZE];
//    S8 Name[FILE_NAME_LENGTH];
};

//请求打印文件
struct ViewReqPrintFileMsg_t
{
    S32 ReqFile;
};

enum HeaterMagnagerMsgAtcion
{
    GET_HEATER_MANAGER_ATTR = 1,
    SET_HEATER_MANAGER_ATTR
};

struct ViewHeaterMsg_t
{
    struct HeaterManagerAttr_t Attr;
    enum HeaterMagnagerMsgAtcion Action;
};

//View的温度操作
struct ViewHeaterTemperMsg_t
{
    struct HeaterTemperInfo_t  Info;
    enum HeaterMagnagerMsgAtcion Action;
};

// 请求Sd卡状态
struct ViewReqDevStatusMsg_t
{
    S32 ReqStatus;
};

enum MoveAxisType_t
{
    MOVE_X_AXIS = 0x01,
    MOVE_Y_AXIS = 0x02,
    MOVE_Z_AXIS = 0x04,
    MOVE_E_AXIS = 0x08,
    BABY_STEP_Z = 0x10
};

enum StepperEnableAction_t
{
    STEPPER_ACTION_NONE,
    STEPPER_ACTION_ENABLE,
    STEPPER_ACTION_DISABLE
};
//电机移动消息
struct ViewStepperMoveMsg_t
{
    FLOAT MoveDisX; //移动距离
    FLOAT MoveDisY; //移动距离
    FLOAT MoveDisZ; //移动距离
    FLOAT MoveDisE; //移动距离
    FLOAT BabyStepZ; //Z轴补偿值
    S8   AxisType; //移动轴（使用enum MoveAxisType_t中的类型）
    S8    Enable;   //是否可用, 0无动作，1使能电机，2使能电机
    S8    AutoHome; //自动归零， 0无动作，1自动归零
};

//请求当前位置
struct ViewReqPosition_t
{
    S32 ReqtPosition;
};

enum FanSpeedAction_t
{
    FAN_SPEED_GET = 1,
    FAN_SPEED_SET,
};

struct ViewFanSpeedMsg_t
{
    enum FanSpeedAction_t  Action;
    S32 Speed;    //风扇速度
};

struct ViewFanSpeedParamMsg_t
{
    struct ParamFanSpeed_t Fan;
    enum FanSpeedAction_t Action;
};

enum MsgType_t
{
    NORMAL_MSG,
    SPECIAL_MSG
};

struct MsgLine_t
{
    S16  MsgType;    //参考MsgType_t。（特殊消息在打印的过程中可以显示）
    S16  Finished;   //0:未结束， 1标识结束
    S8   Info[MAX_MSG_LENGTH];
};

/*********************View MsgType************************/

//驱动属性
struct StepperPins_t //CR_STEPPER_SET_PINS
{
    /* pin part */
    S8 *EnnPinName;      //使能引脚    （名称使用）
    S8 *DirPinName;      //方向引脚
    S8 *StepPinName;     //步进引脚
    S8 *PdUartName;      //串口名称   （串口使能了后，寄存器就会被使用）
    U8 EnableLevel;      //高使能(1)or低使能(0)
    U8 PositiveDirLevel; //正方向是高电平(1)or低电平(0)
};

//电机的基础属性
struct StepperAttr_t  //CR_STEPPER_SET_ATTR
{
    U32   SubDivision; //细分 16 32 64 256
    U32   StepsPerMm;  //步进
    FLOAT StepAngle;   //步距角
    FLOAT Perimeter;   //周长   uint:mm  （电机轴转动一周的长度,若为Z/E轴则表示Z/E轴导程）
};

//运动属性
struct StepperMotionAttr_t
{
    S32 Velocity;     //速度
    S32 MaxVelocity;  //最大速度
    S32 MinVelocity;  //最小速度
    S32 Acceleration; //加速度
    S32 Deceleration; //减速加速度
    S32 Jerk;         //Jerk，加速度导数.(加速度变化率)
};

struct StepperPos_t
{
    BOOL EndStopped;      //是否已经归零
    S32  CurPosition;     //当前绝对位置,实际长度
};

//运动坐标(上报的坐标的内容)
struct MotionCoord_t
{
    FLOAT CoorX;   //X轴坐标
    FLOAT CoorY;   //Y轴坐标
    FLOAT CoorZ;   //Z轴坐标
    FLOAT CoorE;   //E轴坐标
};

//探针动作
enum ProbePtRaise
{
    PROBE_PT_NONE = 0,  // No raise or stow after run_z_probe
    PROBE_PT_STOW,  // Do a complete stow after run_z_probe
    PROBE_PT_RAISE, // Raise to "between" clearance after run_z_probe
    PROBE_PT_BIG_RAISE  // Raise to big clearance after run_z_probe
};

//处理 Gcode 运动坐标，含进料速度
struct MotionGcode_t
{
    U8 RetractFlag;
    struct MotionCoord_t Coord;
    FLOAT FeedRate;
    enum ProbePtRaise ProbeRaise;
};

//含四个浮点数的结构体
struct XYZEFloat_t
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
    FLOAT E;
};


//含三个浮点数的结构体
struct XYZFloat_t
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
};

//含两个浮点数的结构体
struct XYFloat_t
{
    FLOAT X;
    FLOAT Y;
};

#endif

