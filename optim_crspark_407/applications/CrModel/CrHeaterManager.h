/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     CrHeaterManager.cpp
  Author :        chenguojin
  Version:        1.0
  Description:    Gpio Posix接口定义
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenguojin      2020-12-28     V1.0.0.1
****************************************************************************/

#ifndef _CRHEATERMANAGER_H
#define _CRHEATERMANAGER_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"
#include "CrHeater/SenSor/CrSenSor.h"
#include "CrHeater/Pid/CrPid.h"
#include "CrHeater/Pwm/CrHeater.h"
#include "CrModel.h"
#include "Common/Include/CrTimer.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"


/************** Extened By TingFengXuan **************/
enum HeaterStatus{
    IDLE,
    HEATING,
    COOLDOWN,
    PID_KEEP,
    RUNAWAY,
};

enum TemperUnits_t
{
    CELSIUS,
    THERMODYNAMIC,
    FAHRENHEIT
};

typedef enum : S32 {
  INDEX_NONE = -5,
  H_PROBE, H_REDUNDANT, H_CHAMBER, H_BED,
  H_E0, H_E1, H_E2, H_E3, H_E4, H_E5, H_E6, H_E7
} heater_ind_t;



#define BED_MAXTEMP      150    //热床的最大温度
#ifndef BED_OVERSHOOT
  #define BED_OVERSHOOT 10
#endif
#define BED_MAX_TARGET (BED_MAXTEMP - (BED_OVERSHOOT))

#define CHAMBER_MAXTEMP  60     //腔室最大温度

/*****************************************************/

class CrHeaterManager : public CrModel
{
public:
    CrHeaterManager(CrPid *Pid, CrHeater *Heater, CrSensor *Sensor, S8 *SendQueueName, S8 *RecvQueueName);
    ~CrHeaterManager();

    VOID Exec();

    BOOL ReachedTargetTemper();   //是否达到目标温度
    CrPid    *GetPid(){return Pid;}
    CrHeater *GetHeater(){return Heater;}
    CrSensor *GetSensor(){return Sensor;}
    
    enum TemperUnits_t GetTemperUnit() { return Unit; }
    VOID SetTemperUnit(enum TemperUnits_t& TemperUnit) { Unit = TemperUnit; }

    /************************************************************/
    VOID CrHeaterMagagerSetAttr(struct HeaterManagerAttr_t &Attributes);
    const struct HeaterManagerAttr_t CrHeaterMagagerGetAttr();
    FLOAT CrHeaterManagerGetTemper();
    VOID CrHeaterManagerStart();
    VOID CrHeaterManagerStop();
    /************************************************************/
    BOOL SetWaitForHeat(S8 Flag);
    static FLOAT CelsiusToThermodynamic(FLOAT Temper);   //摄氏度转开尔文
    static FLOAT ThermodynamicToCelsius(FLOAT Temper);   //开尔文转摄氏度
    static FLOAT CelsiusToFahrenheit(FLOAT Temper);   //摄氏度转华氏温度
    static FLOAT FahrenheitToCelsius(FLOAT Temper);   //华氏温度转摄氏度

    //
    static S32 GetExtrudeMinTemper();
    static VOID SetExtrudeMinTemper(S32 Temper);
    static BOOL GetColdExtrude();
    static VOID SetColdExtrude(BOOL Enable);

    S32 HandleTemperMsg(S8 *Buff, S32 Len);

protected:


    static VOID TimeOutHandle(VOID *Param);
    void CrHeaterManagerProtection();

private:
    CrPid    *Pid;
    CrHeater *Heater;
    CrSensor *Sensor;
    CrMsgQueue_t RecvQueue;
    CrMsg_t SendQueue;   //向view发送HeaterManagerAttr_t
    struct HeaterManagerAttr_t Attr;

    CrTimer_t   Timer;
    BOOL        HeaterManagerStart;  //加热器管理是否开启
    
    /********************** Extened By TingFengXuan **********************/
    enum HeaterStatus CurStatus;
    enum HeaterStatus LastStatus;
    CrTimeSpec_t   FirstErrTime;
    FLOAT          LastErr;
    BOOL WaitForHeat;
    enum TemperUnits_t Unit;

    static S16  ExtrudeMinTemper;   //最小挤出温度
    static BOOL EnableColdExtrude;  //冷挤出
    BOOL   IsHeated;   //是否是加热
};

extern CrHeaterManager *HotEndArr[HOTENDS];
extern CrHeaterManager *BedHeater;
extern CrHeaterManager *ProbeHeater;    //不用交互，只获取温度，不用加热。
extern CrHeaterManager *ChamberHeater;

extern VOID WaitForProbe(S16 TargetTemp, BOOL NoWaitForCooling);

#endif

