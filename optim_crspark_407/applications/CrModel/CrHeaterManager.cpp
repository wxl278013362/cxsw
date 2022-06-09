/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     CrHeaterManager.cpp
  Author :        chenli
  Version:        1.0
  Description:    Gpio Posix接口定义
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenli         2020-12-28     V1.0.0.1
****************************************************************************/

#include "CrHeaterManager.h"
#include <stdio.h>
#include <rtthread.h>
#include "CrGpio/CrGpio.h"
#include "Common/Include/CrSleep.h"
#include "Common/Include/CrTime.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrGcode.h"

//PWM满功率周期数
#define PWM_COUNT_MAX               (255)
//PID有效温度范围
#define PID_EFFECT_RANGE            (10)
//温度上限
#define UPPER_TEMPERATURE_LIMIT     (350)
//温度下限
#define LOWER_TEMPERATURE_LIMIT     (-40)

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define fABs(n) ((n) > 0.000001 ? (n) : ((n) < -0.000001) ? -(n) : 0.0f)

static U32 NameId = 0;

#define RECV_QUEUE_NAME "/HeartRecv"
#define RECV_QUEUE_PRI   1

#define SEND_QUEUE_NAME "/HeartSend"
#define SEND_QUEUE_PRI   1

S16  CrHeaterManager::ExtrudeMinTemper = 0;   //最小挤出温度
BOOL CrHeaterManager::EnableColdExtrude = 0;  //冷挤出

CrHeaterManager *HotEndArr[HOTENDS] = {NULL};    //挤出机的数量
CrHeaterManager *BedHeater = NULL;
CrHeaterManager *ProbeHeater = NULL;
CrHeaterManager *ChamberHeater = NULL;

/**
 * 函数功能: 加热管理对象构造函数
 * 函数参数: Pid:PID对象指针,Heater:加热器对象指针,Sensor:温度传感器对象指针
 * 输入: Pid:PID对象指针,Heater:加热器对象指针,Sensor:温度传感器对象指针
 * 输出: 无
 * 返回值: 无
 */
CrHeaterManager::CrHeaterManager(CrPid *Pid, CrHeater *Heater, CrSensor *Sensor, S8 *SendQueueName, S8 *RecvQueueName)
    : CrModel(),
      SendQueue(NULL),
      Timer(NULL),
      HeaterManagerStart(0),
      WaitForHeat(true),
      Unit(CELSIUS),
      IsHeated(false)
{
    this->Pid = Pid;
    this->Heater = Heater;
    this->Sensor = Sensor;
    RecvQueue = InValidMsgQueue();

    CrMsgQueueAttr_t MsgAttr;
    MsgAttr.mq_flags = O_NONBLOCK;
    MsgAttr.mq_msgsize = sizeof( struct ViewHeaterMsg_t );
    MsgAttr.mq_maxmsg = 1;

    RecvQueue = QueueOpen(RecvQueueName, O_CREAT | O_RDWR, 0x777, &MsgAttr);

    MsgAttr.mq_msgsize = sizeof( struct HeaterManagerAttr_t );
    SendQueue = SharedQueueOpen(SendQueueName, O_CREAT | O_RDWR, 0x777, &MsgAttr);

    struct CrTimerAttr_t TimeAttr;
    sprintf(TimeAttr.Name,"HotMa%d", NameId);
    TimeAttr.HardwareTimer = 0;
    TimeAttr.Mode = RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER;
    Timer = &TimeAttr;

    CrSignalEvent_t Sig;
    Sig.sigev_notify_function = (VOID (*) (union SigVal_t))CrHeaterManager::TimeOutHandle;
    Sig.sigev_value.sival_ptr = (VOID *)this;
    CrClockid_t ClockId = 0;
    
    CrTimerCreate(ClockId, &Sig, &Timer);
    if ( Timer == NULL)
    {
        rt_kprintf("**** CrHeaterManager Create failed! ****\n");
    }
    memset(&Attr, 0, sizeof(Attr));
    Attr.PidAttr = Pid->GetPidAttr();
    Attr.PidParam = Pid->GetPidParam();
    if ( Heater )
        Attr.HeaterAttr = Heater->CrHeaterGetAttr();

    this->CrHeaterManagerStart();

    NameId ++;

    /* init temperature protect.*/
    CurStatus = IDLE;
}
/**
 * 函数功能: 加热管理对象析构函数
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
CrHeaterManager::~CrHeaterManager()
{
    if ( SendQueue )
    {
        CrMsgClose(SendQueue);
        SendQueue = NULL;
     }

    if ( IsValidMsgQueue(RecvQueue) )
     {
         CrMsgQueueClose(RecvQueue);

         RecvQueue = InValidMsgQueue();
     }
}
/**
 * 函数功能: 定时器超时函数
 * 函数参数: Param:CrHeaterManager对象指针
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrHeaterManager::TimeOutHandle(VOID * Param)
{
    CrHeaterManager *Manager = ( CrHeaterManager * ) Param;

    FLOAT Duty = (Manager->GetPid())->PidCalc((Manager->CrHeaterMagagerGetAttr()).TargetTemper, Manager->CrHeaterManagerGetTemper());
    
    struct PwmControllerAttr_t PwmAttr;

    PwmAttr.DefaultLevel = 1;
    PwmAttr.Freq = 100;
    PwmAttr.Duty = (S32)Duty;
    PwmAttr.Times = 0;

    if(Manager->GetHeater())
        Manager->GetHeater()->CrHeaterSetAttr(PwmAttr);

    //更新温度

}
/**
 * 函数功能: 获取温度
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
FLOAT CrHeaterManager::CrHeaterManagerGetTemper()
{
    return Sensor->GetVaule();
}
/**
 * 函数功能: 设置加热管理属性
 * 函数参数: Attributes:HeaterManagerAttr_t结构体指针
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrHeaterManager::CrHeaterMagagerSetAttr(struct HeaterManagerAttr_t &Attributes)
{
    if ( Pid )
    {
        struct PidAttr_t PidAttr = {0};
        memcpy(&PidAttr,&Attributes.PidAttr,sizeof(struct PidAttr_t));
        Pid->SetPidAttr(PidAttr);
        
        struct PidParam_t PidParam = {0};
        memcpy(&PidParam,&Attributes.PidParam,sizeof(struct PidParam_t));
        Pid->SetPidParam(PidParam);
    }
    if ( Heater )
    {
        Heater->CrHeaterSetAttr(Attributes.HeaterAttr);
    }
    Attr = Attributes;

    if ( Sensor->GetVaule() < Attributes.TargetTemper )
    {
        IsHeated = true;
    }
    else
    {
        IsHeated = false;
    }

    //开启定时器
    if ( !HeaterManagerStart )
        CrHeaterManagerStart();

    return ;
}

/**
 * 函数功能: 处理温度消息
 * 函数参数: Buff:消息内容,Len:消息长度
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 1:处理成功,0:处理失败
 */
S32 CrHeaterManager::HandleTemperMsg(S8 *Buff, S32 Len)
{
    if ( !Buff || (Len < (S32)sizeof(struct ViewHeaterMsg_t)) )
        return 0;

    struct ViewHeaterMsg_t *Heater = (struct ViewHeaterMsg_t *)Buff;

    if ( Heater->Action ==  SET_HEATER_MANAGER_ATTR)
    {
//        if ( Heater->Attr.TargetTemper < 0.000001 )
//            Heater->Attr.TargetTemper = 0;

            Attr.TargetTemper = Heater->Attr.TargetTemper;
    }

    Attr.CurTemper = CrHeaterManagerGetTemper();
    Attr.ReachedTargetTemper = ReachedTargetTemper();

    Attr.HeaterAttr = GetHeater()->CrHeaterGetAttr();

    return 1;
}
/**
 * 函数功能: 获取加热管理属性
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 加热管理属性
 */
const struct HeaterManagerAttr_t CrHeaterManager::CrHeaterMagagerGetAttr()
{
    return Attr;
}
/**
 * 函数功能: 加热管理启动
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrHeaterManager::CrHeaterManagerStart()
{
    if (Timer != NULL)
    {
        CrTimerSpec_t OldTimerSpac = {0};
        CrTimerSpec_t NewTimerSpac = {{0,200000000}};
        CrTimerSetTime(Timer, 1, &NewTimerSpac, &OldTimerSpac);
        HeaterManagerStart = 1;
    }
    else
    {
        printf("timer is NULL!\n");
    }
}
/**
 * 函数功能: 加热管理停止
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */

VOID CrHeaterManager::CrHeaterManagerStop()
{
    if ( !HeaterManagerStart )
        return;

    if (Timer != NULL)
    {
        CrTimerSpec_t OldTimerSpac = {0};
        CrTimerSpec_t NewTimerSpac = {{0}};
        CrTimerSetTime(Timer, 1, &NewTimerSpac, &OldTimerSpac);
    }
    else
    {
        printf("timer is NULL");
    }
    
    HeaterManagerStart = 0;
    struct PwmControllerAttr_t Attr;
    memset(&Attr, 0, sizeof( struct PwmControllerAttr_t));
    if ( Heater )
        Heater->CrHeaterSetAttr(Attr);
}
/**
 * 函数功能: 加热模块与其他Model的消息的处理
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrHeaterManager::Exec()
{
    /* Heater protect. --extened by TingFengXuan */
    CrHeaterManagerProtection();

    if ( !IsValidMsgQueue(RecvQueue) || !SendQueue )
        return ;

    S8 RecvBuf[sizeof( struct ViewHeaterMsg_t )] = {0};
    S32 Pri = RECV_QUEUE_PRI;
    if ( RecvMsg(RecvQueue, RecvBuf, sizeof(RecvBuf), &Pri) != sizeof(RecvBuf) )
        return ;

    if ( HandleTemperMsg(RecvBuf, sizeof(RecvBuf)) > 0 )
        CrMsgSend( SendQueue, (S8 *)&Attr, sizeof( struct HeaterManagerAttr_t ), SEND_QUEUE_PRI);
    
//    /* Heater protect. --extened by TingFengXuan */
//    CrHeaterManagerProtection();

    return ;
}
/**
 * 函数功能: 判断是否到达目标温度
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 1:到达目标温度,0:未到达
 */
BOOL CrHeaterManager::ReachedTargetTemper()
{
    FLOAT CurDegree = Sensor->GetVaule();
    if (Attr.TargetTemper > -0.000001 && Attr.TargetTemper < 0.000001)
    {
        return true;
    }

    //FLOAT Devation = fABs(Attr.TargetTemper - CurDegree);
    FLOAT Devation = CurDegree - Attr.TargetTemper;
    BOOL Reach = false;
    if ( IsHeated )  //
    {
         if( Devation >= 0.000001f )
             Reach = true;
    }
    else
    {
        if ( fABs(Devation) < 1.0f )
            Reach = true;
    }

    return Reach;
}
/**
 * 函数功能: 温度保护
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 无
 * 注：如温度异常则会关闭加热
 */
void CrHeaterManager::CrHeaterManagerProtection( )
{
    FLOAT Err = 0;
    CrTimeSpec_t Time = {0};

    struct HeaterManagerAttr_t Attr = {0};
    Attr = CrHeaterMagagerGetAttr();
    FLOAT Degreen = CrHeaterManagerGetTemper();
    /* update heater state */
    if ( fABs(Attr.TargetTemper - Degreen) < PID_EFFECT_RANGE )
    {
        CurStatus = PID_KEEP;
    }
    if ( Attr.TargetTemper < Degreen - PID_EFFECT_RANGE )
    {
        CurStatus = COOLDOWN;
    }
    if ( Attr.TargetTemper > Degreen + PID_EFFECT_RANGE )
    {
        CurStatus = HEATING;
    }
    if ( CurStatus != LastStatus )
    {
        /* clear first error time recording.*/
        memset(&FirstErrTime,0,sizeof(CrTimeSpec_t));
        LastStatus = CurStatus;
    }
    
    switch ( CurStatus )
    {
        case IDLE:
            /* waitting for setting temperature */
            break;
        case HEATING:
            Err = Attr.TargetTemper - Degreen;
            if(Err > LastErr)
            {
                if (FirstErrTime.tv_nsec == 0 && FirstErrTime.tv_sec == 0)
                {
                    CrGetCurrentSystemTime(&FirstErrTime);
                }
                else
                {
                    CrGetCurrentSystemTime(&Time);
                    if (FirstErrTime.tv_sec - Time.tv_sec > 60)//more than 1min,heater is runaway.
                    {
                        CurStatus = RUNAWAY;
                    }
                }
            }
        break;
        case PID_KEEP:
            Err = Attr.TargetTemper - Degreen;
            if( Err > PID_EFFECT_RANGE || Err < -PID_EFFECT_RANGE )
            {
                CurStatus = RUNAWAY;
            }
        break;    
        case COOLDOWN:
        {
            struct HeaterManagerAttr_t Attr = {0};
            Attr = CrHeaterMagagerGetAttr();
            /* Confirm whether to cool down by judging the duty cycle.*/
            if(Attr.HeaterAttr.Duty != 0)
            {
                if (FirstErrTime.tv_nsec == 0 && FirstErrTime.tv_sec == 0)
                {
                    CrGetCurrentSystemTime(&FirstErrTime);
                }
                else
                {
                    CrGetCurrentSystemTime(&Time);
                    if (FirstErrTime.tv_sec - Time.tv_sec > 60)//more than 1min,heater is runaway.
                    {
                        CurStatus = RUNAWAY;
                    }
                }
            }
        }
        break;
        case RUNAWAY:
            rt_kprintf("Heater is wrong,close heater!\n");
            CrHeaterManagerStop();
        break;
    }
    if( Degreen >= UPPER_TEMPERATURE_LIMIT || Degreen < LOWER_TEMPERATURE_LIMIT )
    {
        //rt_kprintf("Heater is too high or low,close heater!\n");
        CrHeaterManagerStop();
    }
}


FLOAT CrHeaterManager::CelsiusToThermodynamic(FLOAT Temper)
{
    Temper = (Temper + 273.15);

    if ( Temper < 0 )   //热力学温度的最小值是0
        Temper = 0;

    return Temper;
}

FLOAT CrHeaterManager::ThermodynamicToCelsius(FLOAT Temper)
{
    if ( Temper < 0 )
        Temper = 0;

    Temper = Temper - 273.15;

    return Temper;
}

FLOAT CrHeaterManager::CelsiusToFahrenheit(FLOAT Temper)
{
    Temper = (9.0 / 5) * Temper +32;

    return Temper;
}

FLOAT CrHeaterManager::FahrenheitToCelsius(FLOAT Temper)
{
    Temper = (5.0 / 9) * (Temper - 32);

    return Temper;
}

S32 CrHeaterManager::GetExtrudeMinTemper()
{
    return CrHeaterManager::ExtrudeMinTemper;
}

VOID CrHeaterManager::SetExtrudeMinTemper(S32 Temper)
{
    CrHeaterManager::ExtrudeMinTemper = Temper;
}

BOOL CrHeaterManager::GetColdExtrude()
{
    return CrHeaterManager::EnableColdExtrude;
}

BOOL CrHeaterManager::SetWaitForHeat(S8 Flag)
{
    return (Flag == -1) ? WaitForHeat : (WaitForHeat = Flag);
}

VOID CrHeaterManager::SetColdExtrude(BOOL Enable)
{
    CrHeaterManager::EnableColdExtrude = Enable;
}

VOID WaitForProbe(S16 TargetTemp, BOOL NoWaitForCooling)
{
    CrMsg_t LineMsgQueue = NULL;
    MsgLine_t LineMsg = {0};

    const BOOL WantToCool = (ProbeHeater->CrHeaterManagerGetTemper() > TargetTemp) ? true : false;
    const BOOL WillWait = !(WantToCool && NoWaitForCooling);

    LineMsgQueue = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));

    // 判断是否需要等待，需要则输出提示是升温等待还是降温等待
    if ( WillWait && WantToCool )
    {
        printf("Waiting for probe to cool down.\n");
    }
    else
    {
        printf("Waiting for probe to heat up.\n");
    }

    // 进入等待需要执行的操作
    if ( GcodeModel )
    {
        GcodeModel->StartWaiting();
    }


    LineMsg.MsgType = SPECIAL_MSG;
    strcpy(LineMsg.Info,"Probe Heating");

    if ( LineMsgQueue )
    {
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
    }

    // 开始等待
    while ( !ProbeHeater->ReachedTargetTemper() ) //未达到设定温度
    {
        if ( GcodeModel )
        {
            if ( !GcodeModel->IsWaiting() ) //不处于等待状态则退出
            {
                break;
            }
        }

        CrM_Sleep(50);
    }

    // 完成操作后输出提示
    if ( GcodeModel && GcodeModel->IsWaiting() )
    {
        GcodeModel->StopWaiting();
    }
    printf("M192 stop waiting heatting.\n");

    // 发送结束消息到队列中
    LineMsg.MsgType = SPECIAL_MSG;
    strcpy(LineMsg.Info,"");
    LineMsg.Finished = 1;
    if ( LineMsgQueue )
    {
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
    }
}
