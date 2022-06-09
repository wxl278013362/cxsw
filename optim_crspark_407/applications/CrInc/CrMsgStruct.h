#ifndef _CRMSGSTRUCT_H
#define _CRMSGSTRUCT_H

#include "CrType.h"
#include "CrMsgType.h"

#define  MSG_TYPE_INTERVAL  1000
#define  MSG_TYPE_CUSTOM    2000    //自定义消息的起始ID
//#define  PRINT_FILE_NAME_LENGTH   80
#define  PRINTER_NAME_LENGTH 12
#define  PRINTER_INFO_LENGTH 200

//消息类型
enum MsgType
{
    None = 0,
    //请求消息
    RequestGetAllInfo,                  //请求获取所有信息
    RequestGetPrintInfo,                //请求获取打印信息
    RequestStartPrintMsg,               //请求开始打印
    RequestStopPrintMsg,                //请求停止打印
    RequestPausePrintMsg,               //请求暂停打印
    RequestResumePrintMsg,              //请求恢复打印
    RequestGetPrinterInfo,              //请求打印机信息
    RequestGetPrintTime,                //请求打印时间
    RequestGetPrintProgress,            //请求打印进度
    RequestSetBedTemperature,           //请求设置热床的温度
    RequestGetBedTemperature,           //请求获取热床的温度
    RequestSetNozzleTemperature,        //请求设置喷嘴的温度
    RequestGetNozzleTemperature,        //请求获取喷嘴的温度
    RequestSetFanSpeed,                 //请求设置风扇速度
    RequestGetFanSpeed,                 //请求获取风扇速度
    RequestSetAbsPreheatInfo,           //请求设置ABS预热信息
    RequestGetAbsPreheatInfo,           //请求获取ABS预热信息
    RequestGetNozzleStatus,             //请求获取喷嘴的状态
    RequestGetBedStatus,                //请求获取热床的状态
    RequestGetFanStatus,                //请求获取风扇的状态
    RequestSetPrintFileName,            //请求设置打印文件的名称
    RequestGetPrintingFileName,         //请求获取正在打印文件的名称
    RequestSetFilamentExtrudeSpeed,     //请求设置长丝的挤出速度
    RequestGetFilamentExtrudeSpeed,     //请求获取长丝的挤出速度
    RequestSetOffsetOfNozzleToBed,      //请求设置挤出头到热床的偏移量
    RequestGetOffsetOfNozzleToBed,      //请求获取挤出头到热床的偏移量
    RequestSetAutoBackOrigin,           //请求自动归零
    RequestGetSdCardStatus,             //请求获取SD卡状态
    RequestSetX_AxisMotorStep,          //请求设置X轴电机步进值
    RequestGetX_AxisMotorStep,          //请求获取X轴电机步进值
    RequestSetY_AxisMotorStep,          //请求设置Y轴电机步进值
    RequestGetY_AxisMotorStep,          //请求获取Y轴电机步进值
    RequestSetZ_AxisMotorStep,          //请求设置Z轴电机步进值
    RequestGetZ_AxisMotorStep,          //请求获取Z轴电机步进值
    RequestSetExtruderMotorStep,        //请求设置挤出头电机步进值
    RequestGetExtruderMotorStep,        //请求获取挤出头电机步进值
    RequestSetX_AxisMotorMaxSpeed,      //请求设置X轴电机最大移动速度
    RequestGetX_AxisMotorMaxSpeed,      //请求获取X轴电机最大移动速度
    RequestSetY_AxisMotorMaxSpeed,      //请求设置Y轴电机最大移动速度
    RequestGetY_AxisMotorMaxSpeed,      //请求获取Y轴电机最大移动速度
    RequestSetZ_AxisMotorMaxSpeed,      //请求设置Z轴电机最大移动速度
    RequestGetZ_AxisMotorMaxSpeed,      //请求获取Z轴电机最大移动速度
    RequestSetExtruderMotorMaxSpeed,    //请求设置挤出头电机最大移动速度
    RequestGetExtruderMotorMaxSpeed,    //请求获取挤出头电机最大移动速度
    RequestSetMotorMinSpeed,            //请求设置挤出头电机最小移动速度
    RequestGetMotorMinSpeed,            //请求获取挤出头电机最小移动速度
    RequestSetX_AixePosition,           //请求设置X轴的位置
    RequestGetX_AixePosition,           //请求获取X轴的位置
    RequestSetY_AixePosition,           //请求设置Y轴的位置
    RequestGetY_AixePosition,           //请求获取Y轴的位置
    RequestSetZ_AixePosition,           //请求设置Z轴的位置
    RequestGetZ_AixePosition,           //请求获取Z轴的位置
    RequestSetExtruderPosition,         //请求设置挤出机的位置
    RequestGetExtruderPosition,         //请求获取挤出机的位置（其实是进的料的长度，在预热完成后才可以显示的表示）

    //响应消息
    ResponeGetAllInfo = RequestGetAllInfo + MSG_TYPE_INTERVAL,                      //获取所有信息响应
    ResponeGetPrintInfo = RequestGetPrintInfo + MSG_TYPE_INTERVAL,                  //获取打印信息响应
    ResponeStartPrintMsg = RequestStartPrintMsg + MSG_TYPE_INTERVAL,                //开始打印响应
    ResponeStopPrintMsg = RequestStopPrintMsg + MSG_TYPE_INTERVAL,                  //停止打印响应
    ResponePausePrintMsg = RequestPausePrintMsg + MSG_TYPE_INTERVAL,                //站厅打印响应
    ResponeResumePrintMsg = RequestResumePrintMsg + MSG_TYPE_INTERVAL,              //继续打印响应
    ResponeGetPrinterInfo = RequestGetPrinterInfo + MSG_TYPE_INTERVAL,              //打印机信息响应
    ResponeGetPrintTime = RequestGetPrintTime + MSG_TYPE_INTERVAL,                  //打印时间响应
    ResponeGetPrintProgress = RequestGetPrintProgress + MSG_TYPE_INTERVAL,          //打印进度响应
    ResponeSetBedTemperature = RequestSetBedTemperature + MSG_TYPE_INTERVAL,        //设置热床的温度响应
    ResponeGetBedTemperature = RequestGetBedTemperature + MSG_TYPE_INTERVAL,        //获取热床的温度响应
    ResponeSetNozzleTemperature = RequestSetNozzleTemperature + MSG_TYPE_INTERVAL,  //设置喷嘴的温度响应
    ResponeGetNozzleTemperature = RequestGetNozzleTemperature + MSG_TYPE_INTERVAL,  //获取喷嘴的温度响应
    ResponeGetNozzleStatus = RequestGetNozzleStatus + MSG_TYPE_INTERVAL,             //获取喷嘴的状态响应
    ResponeGetBedStatus = RequestGetBedStatus + MSG_TYPE_INTERVAL,                   //获取热床的状态响应
    ResponeGetFanStatus = RequestGetFanStatus + MSG_TYPE_INTERVAL,                   //获取风扇的状态响应
    ResponeSetFanSpeed = RequestSetFanSpeed + MSG_TYPE_INTERVAL,                     //设置风扇速度响应
    ResponeGetFanSpeed = RequestGetFanSpeed + MSG_TYPE_INTERVAL,                     //获取风扇速度响应
    ResponeSetPrintFileName = RequestSetPrintFileName + MSG_TYPE_INTERVAL,           //设置打印文件的名称响应
    ResponeGetPrintingFileName = RequestGetPrintingFileName + MSG_TYPE_INTERVAL,     //获取正在打印文件的名称响应
    ResponeSetFilamentExtrudeSpeed = RequestSetFilamentExtrudeSpeed + MSG_TYPE_INTERVAL, //设置长丝的挤出速度响应
    ResponeGetFilamentExtrudeSpeed = RequestGetFilamentExtrudeSpeed + MSG_TYPE_INTERVAL, //获取长丝的挤出速度响应
    ResponeSetOffsetOfNozzleToBed = RequestSetOffsetOfNozzleToBed + MSG_TYPE_INTERVAL,   //设置挤出头到热床的偏移量响应
    ResponeGetOffsetOfNozzleToBed = RequestGetOffsetOfNozzleToBed + MSG_TYPE_INTERVAL,   //获取挤出头到热床的偏移量响应
    ResponeSetAutoBackOrigin = RequestSetAutoBackOrigin + MSG_TYPE_INTERVAL,         //自动归零响应
    ResponeGetSdCardStatus = RequestGetSdCardStatus + MSG_TYPE_INTERVAL,             //获取SD卡状态响应
    ResponeSetX_AxisMotorStep = RequestSetX_AxisMotorStep + MSG_TYPE_INTERVAL,       //设置X轴电机步进值响应
    ResponeGetX_AxisMotorStep = RequestGetX_AxisMotorStep + MSG_TYPE_INTERVAL,       //获取X轴电机步进值响应
    ResponeSetY_AxisMotorStep = RequestSetY_AxisMotorStep + MSG_TYPE_INTERVAL,       //设置Y轴电机步进值响应
    ResponeGetY_AxisMotorStep = RequestGetY_AxisMotorStep + MSG_TYPE_INTERVAL,       //获取Y轴电机步进值响应
    ResponeSetZ_AxisMotorStep = RequestSetZ_AxisMotorStep + MSG_TYPE_INTERVAL,       //设置Z轴电机步进值响应
    ResponeGetZ_AxisMotorStep = RequestGetZ_AxisMotorStep + MSG_TYPE_INTERVAL,       //获取Z轴电机步进值响应
    ResponeSetExtruderMotorStep = RequestSetExtruderMotorStep + MSG_TYPE_INTERVAL,   //设置挤出头电机步进值响应
    ResponeGetExtruderMotorStep = RequestGetExtruderMotorStep + MSG_TYPE_INTERVAL,   //获取挤出头电机步进值响应
    ResponeSetX_AxisMotorMaxSpeed = RequestSetX_AxisMotorMaxSpeed + MSG_TYPE_INTERVAL,       //设置X轴电机最大移动速度响应
    ResponeGetX_AxisMotorMaxSpeed = RequestGetX_AxisMotorMaxSpeed + MSG_TYPE_INTERVAL,       //获取X轴电机最大移动速度响应
    ResponeSetY_AxisMotorMaxSpeed = RequestSetY_AxisMotorMaxSpeed + MSG_TYPE_INTERVAL,       //设置Y轴电机最大移动速度响应
    ResponeGetY_AxisMotorMaxSpeed = RequestGetY_AxisMotorMaxSpeed + MSG_TYPE_INTERVAL,       //获取Y轴电机最大移动速度响应
    ResponeSetZ_AxisMotorMaxSpeed = RequestSetZ_AxisMotorMaxSpeed + MSG_TYPE_INTERVAL,       //设置Z轴电机最大移动速度响应
    ResponeGetZ_AxisMotorMaxSpeed = RequestGetZ_AxisMotorMaxSpeed + MSG_TYPE_INTERVAL,       //获取Z轴电机最大移动速度响应
    ResponeSetExtruderMotorMaxSpeed = RequestSetExtruderMotorMaxSpeed + MSG_TYPE_INTERVAL,   //设置挤出头电机最大移动速度响应
    ResponeGetExtruderMotorMaxSpeed = RequestGetExtruderMotorMaxSpeed + MSG_TYPE_INTERVAL,   //获取挤出头电机最大移动速度响应
    ResponeSetMotorMinSpeed = RequestSetMotorMinSpeed + MSG_TYPE_INTERVAL,               //设置电机最小移动速度响应
    ResponeGetMotorMinSpeed = RequestGetMotorMinSpeed + MSG_TYPE_INTERVAL,               //获取电机最小移动速度响应
    ResponeSetX_AixePosition  = RequestSetX_AixePosition + MSG_TYPE_INTERVAL,            //设置X轴的位置响应
    ResponeGetX_AixePosition  = RequestGetX_AixePosition + MSG_TYPE_INTERVAL,            //获取X轴的位置响应
    ResponeSetY_AixePosition  = RequestSetY_AixePosition + MSG_TYPE_INTERVAL,            //设置Y轴的位置响应
    ResponeGetY_AixePosition  = RequestGetY_AixePosition + MSG_TYPE_INTERVAL,            //获取Y轴的位置响应
    ResponeSetZ_AixePosition  = RequestSetZ_AixePosition + MSG_TYPE_INTERVAL,            //设置Z轴的位置响应
    ResponeGetZ_AixePosition  = RequestGetZ_AixePosition + MSG_TYPE_INTERVAL,            //获取Z轴的位置响应
    ResponeSetExtruderPosition  = RequestSetExtruderPosition + MSG_TYPE_INTERVAL,        //设置挤出机的位置响应
    ResponeGetExtruderPosition = RequestGetExtruderPosition + MSG_TYPE_INTERVAL,         //获取挤出机的位置响应
    ResponeSetAbsPreheatInfo = RequestSetAbsPreheatInfo + MSG_TYPE_INTERVAL,             //设置ABS预热信息响应
    ResponeGetAbsPreheatInfo = RequestGetAbsPreheatInfo + MSG_TYPE_INTERVAL,             //获取ABS预热信息响应

    //model主动发送信息
    PrintFinished   = MSG_TYPE_CUSTOM,              //打印完成
    ReportPrintInfoMessage,                         //controller上报打印信息
    ReportAxisPositionInfo,                         //上报坐标信息
};

//消息头
struct MsgHead_t
{
    S8  Flag[4];
    S16 MsgId;
    S16 MsgLen;
};

//回应消息头
struct ResponeHead_t
{
    struct MsgHead_t    Head;
    S32                 ReqResult;  //请求结果 ： 0 请求失败， 1 请求成功
};

//消息结构体
//打印时间
struct PrintTime_t
{
    S16     Hour;       //小时
    S8      Minter;     //分钟
    S8      Second;     //秒数
};

//打印操作结果
struct PrintOptResult_t
{
    S32     Res;     //操作结果：0 操作失败， 1 操作成功
};

//打印进度
struct PrintProgress_t
{
    S32     Progress;
};

//打印机信息
struct PrinterInfo
{
    S8      Info[PRINTER_INFO_LENGTH];
};

//SD卡状态
struct SdCardStatus_t
{
    S32     Status;     //状态：0 不在线， 1 在线
};

//打印文件
struct PrintFile_t
{
    S8 Name[FILE_NAME_LENGTH];
};

//温度
struct Temperature_t
{
    float   Temp;
};

//速度
struct CustomSpeed_t
{
    float  Speed;
};

#if 0
//风扇速度
struct FanSpeed_t
{
    struct CustomSpeed_t  FanSpeed;
};
#endif

//预热信息
struct PreheatInfo_t
{
    struct Temperature_t    BedTemp;
    struct Temperature_t    NozzleTemp;
    struct CustomSpeed_t    FanSpeed;
};

//速度信息
struct  MotorMaxSpeed_t
{
    struct CustomSpeed_t      X_MaxSpeed;
    struct CustomSpeed_t      Y_MaxSpeed;
    struct CustomSpeed_t      Z_MaxSpeed;
    struct CustomSpeed_t      ExtruderMaxSpeed;
};

//电机最小速度
struct MotorMinSpeed_t
{
    struct CustomSpeed_t      MinSpeed;
};

//设备状态
struct DeviceStatus_t
{
    S8  BedStatus;      //热床状态
    S8  NozzleStatus;   //喷嘴状态
    S8  FanSpeed;      //风扇状态
    S8  Reserve;        //预留    (可以认为是打印机状态)
};

//位置信息
struct  Position_t
{
    float      PosValue;
};

//坐标轴的位置
struct AxisPosition_t
{
    struct  Position_t      X_Pos;
    struct  Position_t      Y_Pos;
    struct  Position_t      Z_Pos;
};

//所有电机的位置信息
struct  AllPositions_t
{
    struct AxisPosition_t   AxisPos;
    struct Position_t       ExtruderPos;    //此位置信息其实是挤出机挤出长丝的长度。
};

//热床和喷头之间的偏移量
struct OffsetNozzleToBed_t
{
    float       Offset;
};

//电机步进值
struct MotorStep_t
{
    float   StepValue;    //步进值
};

//所有电机的步进值
struct AllMotorsStep_t
{
    struct MotorStep_t X_Step;
    struct MotorStep_t Y_Step;
    struct MotorStep_t Z_Step;
    struct MotorStep_t ExtruderStep;  //挤出机电机步进
};

//主机面显示的打印信息
struct  ShowPrintInfo_t
{
    S8                      Name[PRINTER_NAME_LENGTH];   //打印机名称
    struct AxisPosition_t   AixsPos;
    struct Temperature_t    BedTemp;
    struct Temperature_t    NozzleTemp;
    struct Temperature_t    PreheatBedTemp;
    struct Temperature_t    PreheatNozzleTemp;
    struct SdCardStatus_t   SdStatus;
    struct DeviceStatus_t   DevStatus;
    struct PrintFile_t      File;
    struct PrintTime_t      Time;
    struct PrintProgress_t  Progress;
    S16                     FeedrateRatio;
    S8                      StepperX_Enable;
    S8                      StepperY_Enable;
    S8                      StepperZ_Enable;
    S8                      HomeX;
    S8                      HomeY;
    S8                      HomeZ;
};

#endif
