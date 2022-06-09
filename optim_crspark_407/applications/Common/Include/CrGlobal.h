#ifndef _CRGLOBAL_H
#define _CRGLOBAL_H

#include "CrInc/CrType.h"
#include "CrInc/CrMsgType.h"

#define SLOT_COUNT (2)

#ifdef SLOT_COUNT
extern struct MotionCoord_t StoredPos[SLOT_COUNT];
#endif

extern BOOL EnableArc;                 //曲线运动
extern BOOL EnableSerialOnOff;         //串口控制
extern BOOL EnableBinaryFileTransfer;  //二进制文件移动
extern BOOL EnableEeprom;              //EEPROM
extern BOOL EnableVolumetric;          //容积模式
extern BOOL EnableAutoReportTemper;    //自动上报温度
extern BOOL EnableAutoLevel;           //自动调平
extern BOOL EnableRunOutSensor;        //断料检查
extern BOOL EnableBedProbe;            //Z探针
extern BOOL EnableMeshLeveling;        //床平数据（网格数据）
extern BOOL EnableSetProgressManually; //设置LCD显示的打印进度
extern BOOL EnablePsuCtrl;             //电源控制
extern BOOL EnableCaseLight;           //切换灯
extern BOOL EnableCaseLightBrightness; //切换灯的亮度
extern BOOL EnableEmergencyParser;     //紧急情况解析器
extern BOOL EnablePromptSupport;       //迅速支持
extern BOOL EnableSdSupport;           //支持SD卡
extern BOOL EnableAutoReportSdStatus;  //自动上报SD卡状态
extern BOOL EnableLongFileNameSupport; //长文件名支持
extern BOOL EnableThermallySafe;       //过热保护
extern BOOL EnableMotionModes;         //运动模式
extern BOOL EnableBabySteps;           //微步进
extern BOOL EnableHeatedChamber;       //腔体加热
extern BOOL EnableGeometryReport;      //机器几何报告

#endif
