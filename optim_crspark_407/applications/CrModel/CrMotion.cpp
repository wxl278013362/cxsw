#include "CrMotion.h"

#include "CrMotion/CrEndStop.h"
#include "Common/Include/CrSleep.h"
#include "Common/Include/CrTime.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrMotion/CrMotionPlanner.h"
#include "CrMotion/Bresenham.h"

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdio.h>

char *SteppersName[] = {(S8 *)X_STEPPER_NAME, (S8 *)Y_STEPPER_NAME, (S8 *)Z_STEPPER_NAME, (S8 *)E_STEPPER_NAME};
#define MM_TO_INCH(n)   ((FLOAT)(n) / 25.4 )
#define INCH_TO_MM(n)   ((FLOAT)(n) * 25.4 )

CrMotion *MotionModel = NULL;
struct MsgLine_t Lines[10] = {0};


FLOAT CrMotion::Deltamm[AxisNum] = {0};

//struct XYZFloat_t HotendOffset[HOTENDS] = {0};
/**
 * 函数功能: Motion构造函数
 * 函数参数: Driver:硬件信息,Attr:运动参数
 * 输入: Driver:硬件信息,Attr:运动参数
 * 输出: 无
 * 返回值: 无
 */
CrMotion::CrMotion(struct MotionDriver_t &Driver, struct MotionAttr_t &Attr)
    : CrModel(),
      Diameter(FIL_DIA),
      EnableVolumePattern(0)
{

    /* enable PB4 PB3 gpio,disable JTAG DEBUG mode.*/
    //__HAL_RCC_AFIO_CLK_ENABLE();
    //__HAL_AFIO_REMAP_SWJ_NOJTAG();

    /**
     * BigTree Board mircostep setting. MS1:PC12   MS2:PC10
     */
    rt_pin_mode(GET_PIN(C,12), PIN_MODE_OUTPUT);
    rt_pin_mode(GET_PIN(C,10), PIN_MODE_OUTPUT);
    rt_pin_write(GET_PIN(C,12), PIN_HIGH);
    rt_pin_write(GET_PIN(C,10), PIN_HIGH);

    
    StepperX_Fd = CrStepperOpen(SteppersName[X], 0, 0);
    if (StepperX_Fd < 0)
    {
        printf("Open X Stepper failed!\n");
        return;
    }
    StepperY_Fd = CrStepperOpen(SteppersName[Y], 0, 0);
    if(StepperY_Fd < 0)
    {
        printf("Open Y Stepper failed!\n");
        return;
    }
    StepperZ_Fd = CrStepperOpen(SteppersName[Z], 0, 0);
    if(StepperY_Fd < 0)
    {
        printf("Open Z Stepper failed!\n");
        return;
    }
    StepperE_Fd = CrStepperOpen(SteppersName[E], 0, 0);
    if(StepperE_Fd < 0)
    {
        printf("Open E Stepper failed!\n");
        return;
    }

    struct StepperPins_t *Pins = &Driver.StepperX_Pin;
    struct StepperPins_t *PinsY = &Driver.StepperY_Pin;
    struct StepperPins_t *PinsZ = &Driver.StepperZ_Pin;
    struct StepperPins_t *PinsE = &Driver.StepperE_Pin;

    if (CrStepperIoctl(StepperX_Fd,CRSTEPPER_SET_PINS,Pins) < 0)
    {
        printf("Set X Pins Failed!\n");
        return;
    }
    if (CrStepperIoctl(StepperY_Fd,CRSTEPPER_SET_PINS,PinsY) < 0)
    {
        printf("Set Y Pins Failed!\n");
        return;
    }
    if (CrStepperIoctl(StepperZ_Fd,CRSTEPPER_SET_PINS,PinsZ) < 0)
    {
        printf("Set Z Pins Failed!\n");
        return;
    }
    if (CrStepperIoctl(StepperE_Fd,CRSTEPPER_SET_PINS,PinsE) < 0)
    {
        printf("Set E Pins Failed!\n");
        return;
    }

//    /* disable Stepper.*/
//    if (CrStepperIoctl(StepperX_Fd, CRSTEPPER_SET_DISABLE, NULL) < 0)
//    {
//        printf("Set Stepper Enable Failed!\n");
//        return;
//    }

    struct StepperAttr_t *AttrX = &Attr.AttrX;
    struct StepperAttr_t *AttrY = &Attr.AttrY;
    struct StepperAttr_t *AttrZ = &Attr.AttrZ;
    struct StepperAttr_t *AttrE = &Attr.AttrE;

    if (CrStepperIoctl(StepperX_Fd,CRSTEPPER_SET_ATTR,AttrX) < 0)
    {
        printf("Set X Attr failed!\n");
        return;
    }
    if (CrStepperIoctl(StepperY_Fd,CRSTEPPER_SET_ATTR,AttrY) < 0)
    {
        printf("Set Y Attr failed!\n");
        return;
    }
    if (CrStepperIoctl(StepperZ_Fd,CRSTEPPER_SET_ATTR,AttrZ) < 0)
    {
        printf("Set Z Attr failed!\n");
        return;
    }
    if (CrStepperIoctl(StepperE_Fd,CRSTEPPER_SET_ATTR,AttrE) < 0)
    {
        printf("Set E Attr failed!\n");
        return;
    }
    
    struct StepperMotionAttr_t *MotionAttrX = &Attr.MotionAttrX;
    struct StepperMotionAttr_t *MotionAttrY = &Attr.MotionAttrY;
    struct StepperMotionAttr_t *MotionAttrZ = &Attr.MotionAttrZ;
    struct StepperMotionAttr_t *MotionAttrE = &Attr.MotionAttrE;
 
    if (CrStepperIoctl(StepperX_Fd, CRSTEPPER_SET_MOTION_ATTR, MotionAttrX) < 0)
    {
        printf("Set Motion Attr Failed!\n");
        return;
    }
    if (CrStepperIoctl(StepperY_Fd, CRSTEPPER_SET_MOTION_ATTR, MotionAttrY) < 0)
    {
        printf("Set Motion Attr Failed!\n");
        return;
    }
    if (CrStepperIoctl(StepperZ_Fd, CRSTEPPER_SET_MOTION_ATTR, MotionAttrZ) < 0)
    {
        printf("Set Motion Attr Failed!\n");
        return;
    }
    if (CrStepperIoctl(StepperE_Fd, CRSTEPPER_SET_MOTION_ATTR, MotionAttrE) < 0)
    {
        printf("Set Motion Attr Failed!\n");
        return;
    }

    CrStepperIoctl(StepperX_Fd, CRSTEPPER_SET_POSITIVE_DIR, NULL);
    CrStepperIoctl(StepperY_Fd, CRSTEPPER_SET_POSITIVE_DIR, NULL);
    CrStepperIoctl(StepperZ_Fd, CRSTEPPER_SET_POSITIVE_DIR, NULL);

    /* EndStop initialization.*/
    if ((EndStopX_Fd = CrEndStopOpen(Driver.EndStopX_PinName, 0, 0)) < 0)
    {
        printf("X EndStop Init Failed!\n");
        return;
    }
    if ((EndStopY_Fd = CrEndStopOpen(Driver.EndStopY_PinName, 0, 0)) < 0)
    {
        printf("Y EndStop Init Failed!\n");
        return;
    }
    if ((EndStopZ_Fd = CrEndStopOpen(Driver.EndStopZ_PinName, 0, 0)) < 0)
    {
        printf("Z EndStop Init Failed!\n");
        return;
    }

    if (CrEndStopIoctl(EndStopX_Fd, CRENDSTOP_SET_TRAGLEVEL, &Driver.EndStopX_TrigLevel) < 0)
    {
        return;
    }
    if (CrEndStopIoctl(EndStopY_Fd, CRENDSTOP_SET_TRAGLEVEL, &Driver.EndStopY_TrigLevel) < 0)
    {
        return;
    }
    if (CrEndStopIoctl(EndStopZ_Fd, CRENDSTOP_SET_TRAGLEVEL, &Driver.EndStopZ_TrigLevel) < 0)
    {
        return;
    }
    
    S32 StepperFd[AxisNum] = {0};
    StepperFd[X] = StepperX_Fd;
    StepperFd[Y] = StepperY_Fd;
    StepperFd[Z] = StepperZ_Fd;
    StepperFd[E] = StepperE_Fd;

    S32 EndStopFd[AxisNum] = {0};
    EndStopFd[X] = EndStopX_Fd;
    EndStopFd[Y] = EndStopY_Fd;
    EndStopFd[Z] = EndStopZ_Fd;

    if (0 > StepperRunInit(StepperFd,AxisNum))
        printf("<error> Stepper Init Failed!\n");
    
    StepperInit(StepperFd,EndStopFd,AxisNum);
    PlannerInit(StepperFd,AxisNum);

    /* Init Send and Receive Queue.*/
    RecvReqPosMsg = InValidMsgQueue();
    RecvStepperMoveMsg = InValidMsgQueue();
    RecvSetMotionAttrMsg = InValidMsgQueue();
    SendMotionAttrMsg = NULL;
    SendPosMsg = NULL;

    CrMsgQueueAttr_t MsgAttr;
    MsgAttr.mq_flags = O_NONBLOCK;
    MsgAttr.mq_maxmsg = 2;

    MsgAttr.mq_msgsize = sizeof(struct ViewReqPosition_t);
    RecvReqPosMsg = QueueOpen(MOTION_POS_REQ_QUEUE, O_CREAT | O_RDWR, 0x0777, &MsgAttr);   //使用普通队列

    MsgAttr.mq_msgsize = sizeof(struct ViewStepperMoveMsg_t);
    RecvStepperMoveMsg = QueueOpen(MOTION_MOVE_OPT_QUEUE, O_CREAT | O_RDWR, 0x0777, &MsgAttr);

    MsgAttr.mq_msgsize = sizeof(struct ViewMotionMsg_t);
    RecvSetMotionAttrMsg = QueueOpen(MOTION_ATTR_SET_QUEUE, O_CREAT | O_RDWR, 0x0777, &MsgAttr);

    MsgAttr.mq_msgsize = sizeof(struct ViewMotionMsg_t);
    SendMotionAttrMsg = SharedQueueOpen(MOTION_ATTR_REPORT_QUEUE, O_CREAT | O_RDWR, 0x0777, &MsgAttr);

    MsgAttr.mq_msgsize = sizeof(struct MotionPositionMsg_t);
    SendPosMsg = SharedQueueOpen(MOTION_POS_REPORT_QUEUE, O_CREAT | O_RDWR, 0x0777, &MsgAttr);

    /* Initialzation Finish Point Coordinate.*/
    LatestCoord.CoorX = 0.0;
    LatestCoord.CoorY = 0.0;
    LatestCoord.CoorZ = 0.0;
    LatestCoord.CoorE = 0.0;
    /* Initialzation Motion and extrude absolute mode.*/
    IsRelativeExtrude = 0;
    IsRelativeMotion = 0;
    MotionUnit = Millimeter;

    //使能电机
    CrStepperIoctl(StepperX_Fd, CRSTEPPER_SET_DISABLE, NULL);
    CrStepperIoctl(StepperY_Fd, CRSTEPPER_SET_DISABLE, NULL);
    CrStepperIoctl(StepperZ_Fd, CRSTEPPER_SET_DISABLE, NULL);
    CrStepperIoctl(StepperE_Fd, CRSTEPPER_SET_DISABLE, NULL);

    /**
     * TO DO: calculate ticks thread    --pth03
     */
    CrPthreadAttr_t AttrThread;
    memset(&AttrThread, 0, sizeof(AttrThread));
    CrPthreadAttrInit(&AttrThread);
    AttrThread.stackaddr = TicksThreadBuff;
    AttrThread.stacksize = sizeof(TicksThreadBuff);
    AttrThread.schedparam.sched_priority = RT_MAIN_THREAD_PRIORITY;//RT_MAIN_THREAD_PRIORITY + 1;

    CrPthreadAttrSetDetachState(&AttrThread, PTHREAD_CREATE_DETACHED);
    
    S32 Res = CrPthreadCreate(&CalcThread, &AttrThread, CrMotion::CalcTicksThread, this);
    if ( Res != 0 )
    {
        rt_kprintf("Calc Ticks thread create fail\n");
        return ;
    }
    CrPthreadAttrDestroy(&AttrThread);
}
    /**
     * 函数功能: Motion析构函数
     * 函数参数: 无
     * 输入: 无
     * 输出: 无
     * 返回值: 无
     */

CrMotion::~CrMotion()
{
    CrStepperClose(StepperX_Fd);
    CrStepperClose(StepperY_Fd);
    CrStepperClose(StepperZ_Fd);
    CrStepperClose(StepperE_Fd);
    
    CrEndStopClose(EndStopX_Fd);
    CrEndStopClose(EndStopY_Fd);
    CrEndStopClose(EndStopZ_Fd);
}
/**
 * 函数功能: 线程入口函数,计算各个轴Ticks,并将结果放入Ticks队列
 * 函数参数: Arg: Motion的对象指针
 * 输入: Arg: Motion的对象指针
 * 输出: 无
 * 返回值: 无
 */
//S32 Ticks[AxisNum][CALC_TICK_NUM] = {0};//缓存计算的数据

VOID *CrMotion::CalcTicksThread(VOID *Arg)
{
#define MOTION_PERIOD_NUM   256

//    MotionPeriod_t MotionPeriod[MOTION_PERIOD_NUM] = {0};

    S32 Ret = 0;

    U32 MotionNum = MOTION_PERIOD_NUM;

    U32 Index = 0;

    while(1)
    {
        EndStopHandler();

//        MotionNum = MOTION_PERIOD_NUM;

        Ret = SyncAlgorithm.Calc(LineBuff,MotionNum);       //计算运动参数,可能一次计算出多个运动参数。在对应频率下一次需要走的步数

        if(Ret <= 0)
        {
            CrM_Sleep(10);                                                   //LineBuff空，休眠一段时间
        }

/*
        do                                                                  //尽可能多的计算运动参数
        {
            if(MotionPeriod[Index].StepPeriod == 0 || ((MotionPeriod[Index].MotionFlag & 0xFF) == 0))
            {
                continue;
            }

            if(MotionFlag != MotionPeriod[Index].MotionFlag)
            {
                MotionFlag = MotionPeriod[Index].MotionFlag;

                SetStepperDir(MotionFlag);
            }

//            rt_kprintf("MotionPeriod[%d].StepPeriod=%d\r\n",Index,MotionPeriod[Index].StepPeriod);
//
//            rt_kprintf("MotionPeriod[%d].MotionFlag=%d\r\n",Index,MotionPeriod[Index].MotionFlag);

            Ret = SyncAlgorithm.Put2TickBuffer(&AxisFifo, MotionPeriod[Index]);     //将计算结果入队

//            rt_kprintf("Ret = %d, Index = %d\r\n",Ret, Index);

            if(Ret != 1)
            {
                CrM_Sleep(5);

                continue;
            }

        }while(Ret > 0 && Index++ < MotionNum);

        Index = 0;

        memset(MotionPeriod, 0, MOTION_PERIOD_NUM * sizeof(MotionPeriod_t));

        CrM_Sleep(5);                                                       //运动参数缓冲空，休眠一段时间
*/
    }
}
/**
 * 函数功能: Motion Model的执行函数,处理与Model通信消息
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::Exec()
{
    CurPosMsgOpt();
    SetMotionAttrMsgOpt();
    StepperMoveMsgOpt();

    return;
}
/**
 * 函数功能: 处理当前电机位置信息消息
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::CurPosMsgOpt()
{
    if ( !IsValidMsgQueue(RecvReqPosMsg) || !SendPosMsg )
        return ;

    struct ViewReqPosition_t req = {0};
    S32 Pri = 0;
    if (RecvMsg(RecvReqPosMsg, (S8 *)&req, sizeof(struct ViewReqPosition_t), &Pri) > 0)
    {
        HandleCurPosMsg((S8 *)&req, sizeof(struct ViewReqPosition_t));
        CrMsgSend(SendPosMsg,(S8 *)&CurPos, sizeof(struct MotionPositionMsg_t), 1);
    }

    return ;
}
/**
 * 函数功能: 处理设置运动参数消息
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::SetMotionAttrMsgOpt()
{
    if ( !IsValidMsgQueue(RecvSetMotionAttrMsg) || !SendMotionAttrMsg )
        return ;

    struct ViewMotionMsg_t RecvAttr = {0};
    S32 Pri = 0;
    if ( RecvMsg(RecvSetMotionAttrMsg, (S8 * )&RecvAttr, sizeof(struct ViewMotionMsg_t), &Pri) > 0 )
    {
        struct ViewMotionMsg_t OutAttr = {0};
        if ( HandleSetMotionAttrMsg((S8 * )&RecvAttr, sizeof(struct ViewMotionMsg_t), &OutAttr) < 0);
            return ;

        if (RecvAttr.Action == 1)
            CrMsgSend(SendMotionAttrMsg, (S8 *)&OutAttr, sizeof(struct ViewMotionMsg_t), 1);
    }

    return ;
}
/**
 * 函数功能: 处理电机运动消息
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::StepperMoveMsgOpt()
{
    if ( !IsValidMsgQueue(RecvStepperMoveMsg) )
        return ;

    struct ViewStepperMoveMsg_t Move = {0};
    S32 Pri = 0;
    if ( RecvMsg(RecvStepperMoveMsg, (S8 * )&Move, sizeof(struct ViewStepperMoveMsg_t), &Pri) <= 0 )
        return ;

    HandleStepperMoveMsg((S8 * )&Move, sizeof(struct ViewStepperMoveMsg_t));
    if ( SendPosMsg )
        CrMsgSend(SendPosMsg, (S8 *)&CurPos, sizeof(struct MotionPositionMsg_t), 1);

   return;
}
/**
 * 函数功能: 处理当前位置消息
 * 函数参数: Buff:消息内容,Len:消息长度
 * 输入: Buff:消息内容,Len:消息长度
 * 输出: 无
 * 返回值: 0:处理成功，小于0:处理失败
 */
S32 CrMotion::HandleCurPosMsg(S8 *Buff, S32 Len)
{
    if (Buff == NULL || Len <= 0)
    {
        return -1;
    }

    struct StepperPos_t PosX = {0};
    struct StepperPos_t PosY = {0};
    struct StepperPos_t PosZ = {0};
    
    struct StepperAttr_t AttrX;
    struct StepperAttr_t AttrY;
    struct StepperAttr_t AttrZ;

    StepperGetAttr(&AttrX,X);
    StepperGetAttr(&AttrY,Y);
    StepperGetAttr(&AttrZ,Z);
    
    StepperGetPos(&PosX,X);
    StepperGetPos(&PosY,Y);
    StepperGetPos(&PosZ,Z);

    CurPos.PosX = (FLOAT)PosX.CurPosition / AttrX.StepsPerMm;
    CurPos.PosY = (FLOAT)PosY.CurPosition / AttrY.StepsPerMm;
    CurPos.PosZ = (FLOAT)PosZ.CurPosition / AttrZ.StepsPerMm;
    /* Unit Conversion.*/
    if ( IsMotionUnitTypeInch() )
    {
        CurPos.PosX = MM_TO_INCH( CurPos.PosX );
        CurPos.PosY = MM_TO_INCH( CurPos.PosY );
        CurPos.PosZ = MM_TO_INCH( CurPos.PosZ );
    }
    struct FeedrateAttr_t FeedRateAttr = {0};
    StepperGetFeedrateAttr(&FeedRateAttr);

    CurPos.PrintSpeed = FeedRateAttr.FeedrateRatio;

    CurPos.HomeX = IsHomeX();
    CurPos.HomeY = IsHomeY();
    CurPos.HomeZ = IsHomeZ();

    CurPos.StepperX_Enable = SteppersEnableStatus(StepperX_Fd);
    CurPos.StepperY_Enable = SteppersEnableStatus(StepperY_Fd);
    CurPos.StepperZ_Enable = SteppersEnableStatus(StepperZ_Fd);
    return 1;
}
/**
 * 函数功能: 处理运动属性消息
 * 函数参数: Buff:消息内容,Len:消息长度,OutAttr:运动属性指针地址
 * 输入: Buff:消息内容,Len:消息长度
 * 输出: OutAttr:运动属性指针地址
 * 返回值: 0:处理成功，小于0:处理失败
 */
S32 CrMotion::HandleSetMotionAttrMsg(S8 *Buff, S32 Len, struct ViewMotionMsg_t *OutAttr)
{
    if ( !Buff || (Len < (S32)sizeof(struct ViewMotionMsg_t)) || !OutAttr )
        return -1;

    struct ViewMotionMsg_t *RecvAttr = (struct ViewMotionMsg_t *)Buff;

    struct StepperMotionAttr_t MotionAttr = {0};
    struct StepperAttr_t Attr = {0};
    
    if ( RecvAttr->Action == GET_MOTION_MSG )
    {
        /* Get X Axis Motion Attr.*/
        StepperGetMotionAttr(&MotionAttr, X);
        StepperGetAttr(&Attr, X);
        
        OutAttr->MotionParam.StepperXAttr.MaxVel = MotionAttr.MaxVelocity;
        OutAttr->MotionParam.StepperXAttr.MaxAccel = MotionAttr.Acceleration;
        OutAttr->MotionParam.StepperXAttr.Jerk = MotionAttr.Jerk;
        
        OutAttr->MotionParam.StepperXAttr.StepsPerMm = Attr.StepsPerMm;

        /* Get Y Axis Motion Attr.*/
        memset(&MotionAttr,0,sizeof(struct StepperMotionAttr_t));
        memset(&Attr,0,sizeof(struct StepperAttr_t));
        StepperGetMotionAttr(&MotionAttr, Y);
        StepperGetAttr(&Attr, Y);
        
        OutAttr->MotionParam.StepperYAttr.MaxVel = MotionAttr.MaxVelocity;
        OutAttr->MotionParam.StepperYAttr.MaxAccel = MotionAttr.Acceleration;
        OutAttr->MotionParam.StepperYAttr.Jerk = MotionAttr.Jerk;
        OutAttr->MotionParam.StepperYAttr.StepsPerMm = Attr.StepsPerMm;
        
        /* Get Z Axis Motion Attr.*/
        memset(&MotionAttr,0,sizeof(struct StepperMotionAttr_t));
        memset(&Attr,0,sizeof(struct StepperAttr_t));
        StepperGetMotionAttr(&MotionAttr, Z);
        StepperGetAttr(&Attr, Z);
        
        OutAttr->MotionParam.StepperZAttr.MaxVel = MotionAttr.MaxVelocity;
        OutAttr->MotionParam.StepperZAttr.MaxAccel = MotionAttr.Acceleration;
        OutAttr->MotionParam.StepperZAttr.Jerk = MotionAttr.Jerk;
        OutAttr->MotionParam.StepperZAttr.StepsPerMm = Attr.StepsPerMm;
        
        /* Get E Axis Motion Attr.*/
        memset(&MotionAttr,0,sizeof(struct StepperMotionAttr_t));
        memset(&Attr,0,sizeof(struct StepperAttr_t));
        struct FeedrateAttr_t FeedRateAttr = {0};
        
        StepperGetMotionAttr(&MotionAttr, E);
        StepperGetAttr(&Attr, E);
        StepperGetFeedrateAttr(&FeedRateAttr);
        
        OutAttr->MotionParam.StepperEAttr.Param.MaxVel = MotionAttr.MaxVelocity;
        OutAttr->MotionParam.StepperEAttr.Param.MaxAccel = MotionAttr.Acceleration;
        OutAttr->MotionParam.StepperEAttr.Param.Jerk = MotionAttr.Jerk;
        
        OutAttr->MotionParam.StepperEAttr.Param.StepsPerMm = Attr.StepsPerMm;
        
        OutAttr->MotionParam.StepperEAttr.RetractAccel = FeedRateAttr.RetractAccel;
        
        OutAttr->MotionParam.VelMinXYZ = FeedRateAttr.VelMinXYZ;
        OutAttr->MotionParam.TravelAccelXYZ = FeedRateAttr.TravelAccelXYZ;
        OutAttr->MotionParam.TravelVelMinXYZ = FeedRateAttr.TravelVelMinXYZ;
        OutAttr->MotionParam.AccelXYZ = FeedRateAttr.AccelXYZ;
        OutAttr->MotionParam.PrintRatio = FeedRateAttr.FeedrateRatio;
        OutAttr->MotionParam.FlowRatio = FeedRateAttr.FlowRatio;
        OutAttr->Diameter = Diameter;
        OutAttr->FilamentPatternAction = EnableVolumePattern;
    }
    else if ( RecvAttr->Action == SET_MOTION_MSG )
    {
        /* Set X Axis Motion Attr.*/
        StepperGetMotionAttr(&MotionAttr, X);
        StepperGetAttr(&Attr, X);
        
        MotionAttr.MaxVelocity = RecvAttr->MotionParam.StepperXAttr.MaxVel;
        MotionAttr.Acceleration = RecvAttr->MotionParam.StepperXAttr.MaxAccel;
        MotionAttr.Jerk = RecvAttr->MotionParam.StepperXAttr.Jerk;
        Attr.StepsPerMm = RecvAttr->MotionParam.StepperXAttr.StepsPerMm;
        
        StepperSetMotionAttr(&MotionAttr, X);
        StepperSetAttr(&Attr, X);
        
        /* Set Y Axis Motion Attr.*/
        memset(&MotionAttr,0,sizeof(struct StepperMotionAttr_t));
        memset(&Attr,0,sizeof(struct StepperAttr_t));

        StepperGetMotionAttr(&MotionAttr, Y);
        StepperGetAttr(&Attr, Y);
        
        MotionAttr.MaxVelocity = RecvAttr->MotionParam.StepperYAttr.MaxVel;
        MotionAttr.Acceleration = RecvAttr->MotionParam.StepperYAttr.MaxAccel;
        MotionAttr.Jerk = RecvAttr->MotionParam.StepperYAttr.Jerk;
        Attr.StepsPerMm = RecvAttr->MotionParam.StepperYAttr.StepsPerMm;
        
        StepperSetMotionAttr(&MotionAttr, Y);
        StepperSetAttr(&Attr, Y);

        /* Set Z Axis Motion Attr.*/
        memset(&MotionAttr,0,sizeof(struct StepperMotionAttr_t));
        memset(&Attr,0,sizeof(struct StepperAttr_t));

        StepperGetMotionAttr(&MotionAttr, Z);
        StepperGetAttr(&Attr, Z);
        
        MotionAttr.MaxVelocity = RecvAttr->MotionParam.StepperZAttr.MaxVel;
        MotionAttr.Acceleration = RecvAttr->MotionParam.StepperZAttr.MaxAccel;
        MotionAttr.Jerk = RecvAttr->MotionParam.StepperZAttr.Jerk;
        Attr.StepsPerMm = RecvAttr->MotionParam.StepperZAttr.StepsPerMm;
        
        StepperSetMotionAttr(&MotionAttr, Z);
        StepperSetAttr(&Attr, Z);

        /* Set E Axis Motion Attr.*/
        memset(&MotionAttr,0,sizeof(struct StepperMotionAttr_t));
        memset(&Attr,0,sizeof(struct StepperAttr_t));
        struct FeedrateAttr_t FeedRateAttr = {0};
        
        StepperGetMotionAttr(&MotionAttr, E);
        StepperGetAttr(&Attr, E);
        StepperGetFeedrateAttr(&FeedRateAttr);
        
        MotionAttr.MaxVelocity = RecvAttr->MotionParam.StepperEAttr.Param.MaxVel;
        MotionAttr.Acceleration = RecvAttr->MotionParam.StepperEAttr.Param.MaxAccel;
        MotionAttr.Jerk = RecvAttr->MotionParam.StepperEAttr.Param.Jerk;
        
        Attr.StepsPerMm = RecvAttr->MotionParam.StepperEAttr.Param.StepsPerMm;
        
        FeedRateAttr.RetractAccel = RecvAttr->MotionParam.StepperEAttr.RetractAccel;
        
        FeedRateAttr.VelMinXYZ = RecvAttr->MotionParam.VelMinXYZ;
        FeedRateAttr.TravelAccelXYZ = RecvAttr->MotionParam.TravelAccelXYZ;
        FeedRateAttr.TravelVelMinXYZ = RecvAttr->MotionParam.TravelVelMinXYZ;
        FeedRateAttr.AccelXYZ = RecvAttr->MotionParam.AccelXYZ;
        FeedRateAttr.FeedrateRatio = RecvAttr->MotionParam.PrintRatio;
        FeedRateAttr.FlowRatio = RecvAttr->MotionParam.FlowRatio;

        StepperSetMotionAttr(&MotionAttr, E);
        StepperSetAttr(&Attr, E);
        StepperSetFeedrateAttr(&FeedRateAttr);
        memcpy(OutAttr, RecvAttr, sizeof(struct ViewMotionMsg_t));

        Diameter = RecvAttr->Diameter;
        EnableVolumePattern = RecvAttr->FilamentPatternAction;
    }
    
    return 0;
}
/**
 * 函数功能: Model消息处理
 * 函数参数: Buff:消息内容,Len:消息长度
 * 输入: Buff:消息内容,Len:消息长度
 * 输出: 处理是否成功
 * 返回值: 0:处理成功，小于0:处理失败
 */
S32 CrMotion::HandleStepperMoveMsg(S8 *Buff, S32 Len)
{
    if ( Buff == NULL || Len <= 0 )
    {
        return -1;
    }
    struct Coordinate3d_t CurFinish = {0};
    struct Coordinate3d_t Target = {0};
    struct ViewStepperMoveMsg_t *Move = (struct ViewStepperMoveMsg_t *)Buff;
    
    if (Move->Enable == STEPPER_ACTION_ENABLE)
    {
        SteppersEnable(StepperX_Fd);
    }
    else if (Move->Enable == STEPPER_ACTION_DISABLE)
    {
        DisableAllSteppers();
    }
    else
    {
        if (Move->AutoHome == 1)
        {
            U8 Mark = (HOME_OPT_X | HOME_OPT_Y | HOME_OPT_Z);
            AutoMoveHome(Mark);
        }
        else
        {
            SteppersEnable(StepperX_Fd);
            
            CurFinish.X = LatestCoord.CoorX;
            CurFinish.Y = LatestCoord.CoorY;
            CurFinish.Z = LatestCoord.CoorZ;
            CurFinish.E = LatestCoord.CoorE;
            
            Target.X = LatestCoord.CoorX + Move->MoveDisX;
            Target.Y = LatestCoord.CoorY + Move->MoveDisY;
            Target.Z = LatestCoord.CoorZ + Move->MoveDisZ;
            Target.E = LatestCoord.CoorE + Move->MoveDisE;

            while ( PlanLine(LineBuff, &CurFinish, &Target) < 0)
            {
                CrM_Sleep(10);
            }

            /* Update Current Finish Point Coordinate.*/
            LatestCoord.CoorX = Target.X;
            LatestCoord.CoorY = Target.Y;
            LatestCoord.CoorZ = Target.Z;
            LatestCoord.CoorE = Target.E;

        }
    }
    return 0;
}
/**
 * 函数功能: 归零操作
 * 函数参数: Mark:归零参数,参考G28运动指令参数
 * 输入: Mark
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::AutoMoveHome(U8 Mark)
{
    SteppersEnable(StepperX_Fd);
    SteppersEnable(StepperY_Fd);
    SteppersEnable(StepperZ_Fd);

    CrMotion::Deltamm[X] = 0.0f;
    CrMotion::Deltamm[Y] = 0.0f;
    CrMotion::Deltamm[Z] = 0.0f;
    CrMotion::Deltamm[E] = 0.0f;

    /* if cordination  has been konwn,do not homing.*/
    if ( (Mark & HOME_OPT_O) && IsHome())
    {
        return;
    }
    if ( Mark & HOME_OPT_R )
    {
        struct Coordinate3d_t CurFinish = {0};
        struct Coordinate3d_t Target = {0};

        CurFinish.X = LatestCoord.CoorX;
        CurFinish.Y = LatestCoord.CoorY;
        CurFinish.Z = LatestCoord.CoorZ;
        CurFinish.E = LatestCoord.CoorE;

        Target.X = LatestCoord.CoorX;
        Target.Y = LatestCoord.CoorY;
        Target.Z = LatestCoord.CoorZ + 2.0f;
        Target.E = LatestCoord.CoorE;
        /* raise z axis before home.*/
        //S64 Time = CrGetSystemTimeMilSecs();
        while ( PlanLine(LineBuff, &CurFinish, &Target) < 0 )
        {
//            if ( CrGetSystemTimeMilSecs() - Time > 1000 )
//                printf("echo:%s\n", STR_BUSY_PROCESSING);

            CrM_Sleep(SAFE_DELAY_MS);
        }
    }
    if ( (Mark & HOME_OPT_X) && MoveHomeX())
    {
        CurPos.PosX = 0.0f;
        LatestCoord.CoorX = 0.0f;
    }
    if ( (Mark & HOME_OPT_Y) && MoveHomeY())
    {
        CurPos.PosY = 0.0f;
        LatestCoord.CoorY = 0.0f;
    }
    if ( (Mark & HOME_OPT_Z) && MoveHomeZ())
    {
        CurPos.PosZ = 0.0f;
        LatestCoord.CoorZ = 0.0f;
    }
}
/**
 * 函数功能: 等待电机运动完成,失能所有电机
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::DisableAllSteppers()
{
    while ( !CrBlockIsFinished() )
    {
        /* waitting for moving finish.*/
        CrM_Sleep(10);
    }
    S32 ret = CrStepperIoctl(StepperX_Fd, CRSTEPPER_SET_DISABLE, 0);
    ret = CrStepperIoctl(StepperY_Fd, CRSTEPPER_SET_DISABLE, 0);
        ret = CrStepperIoctl(StepperZ_Fd, CRSTEPPER_SET_DISABLE, 0);
        ret = CrStepperIoctl(StepperE_Fd, CRSTEPPER_SET_DISABLE, 0);
    if (ret < 0)
    {
        printf("Err: Disable Failed!\n");
    }
}
/**
 * 函数功能: 获取最新目标坐标
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
const struct MotionCoord_t& CrMotion::GetLatestCoord()
{
    return LatestCoord;
}
/**
 * 函数功能: 判断所有运动是否完成
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 1:完成,0:未完成
 */
BOOL CrMotion::IsMotionMoveFinished()
{
    return CrBlockIsFinished();
}
/**
 * 函数功能: 设置最新目标坐标
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::SetLatestCoord(struct MotionCoord_t& Coord)
{
    LatestCoord.CoorX = Coord.CoorX;
    LatestCoord.CoorY = Coord.CoorY;
    LatestCoord.CoorZ = Coord.CoorZ;
    LatestCoord.CoorE = Coord.CoorE;

    struct StepperAttr_t AttrX = {0};
    struct StepperAttr_t AttrY = {0};
    struct StepperAttr_t AttrZ = {0};
    struct StepperAttr_t AttrE = {0};

    StepperGetAttr(&AttrX,X);
    StepperGetAttr(&AttrY,Y);
    StepperGetAttr(&AttrZ,Z);
    StepperGetAttr(&AttrE,E);

    struct StepperPos_t Pos_X = {0};
    struct StepperPos_t Pos_Y = {0};
    struct StepperPos_t Pos_Z = {0};
    struct StepperPos_t Pos_E = {0};

    if ( IsMotionUnitTypeInch() )
    {/* Unit Conversion.*/
        Coord.CoorX = INCH_TO_MM( Coord.CoorX );
        Coord.CoorY = INCH_TO_MM( Coord.CoorY );
        Coord.CoorZ = INCH_TO_MM( Coord.CoorZ );
        Coord.CoorE = INCH_TO_MM( Coord.CoorE );
    }
    Pos_X.CurPosition = (S32)(Coord.CoorX * AttrX.StepsPerMm);
    Pos_Y.CurPosition = (S32)(Coord.CoorY * AttrY.StepsPerMm);
    Pos_Z.CurPosition = (S32)(Coord.CoorZ * AttrZ.StepsPerMm);
    Pos_E.CurPosition = (S32)(Coord.CoorE * AttrE.StepsPerMm);
    
    StepperSetPos(&Pos_X, X);
    StepperSetPos(&Pos_Y, Y);
    StepperSetPos(&Pos_Z, Z);
    StepperSetPos(&Pos_E, E);
}

static FLOAT CalcSqrt(FLOAT Num)
{
    if (Num >= -EOL_DATA && Num <= EOL_DATA)
        return 0;
    FLOAT Xhalf = 0.5f * Num;
    U32 i = *(U32*)&Num;
    i = 0x5f3759df - (i >> 1);
    Num = *(float *)&i;
    Num = Num * (1.5f - Xhalf * Num * Num);
    return (1.0f / Num);
}

/**
 * 函数功能: 直线运动入队
 * 函数参数: Coord:运动的目标坐标与速度参数
 * 输入: Coord:运动的目标坐标与速度参数
 * 输出: 参考返回值
 * 返回值: 1:成功入队,否则入队失败
 */
S32 CrMotion::PutMotionCoord(struct MotionGcode_t& Coord)
{
    struct Coordinate3d_t CurCoordinate = {0};
    struct Coordinate3d_t Target = {0};
    S32 ret = -1;

    if ( IsMotionUnitTypeInch() )
    {
        CurCoordinate.X = INCH_TO_MM(LatestCoord.CoorX);
        CurCoordinate.Y = INCH_TO_MM(LatestCoord.CoorY);
        CurCoordinate.Z = INCH_TO_MM(LatestCoord.CoorZ);
        CurCoordinate.E = INCH_TO_MM(LatestCoord.CoorE);
    }
    else
    {
        CurCoordinate.X = LatestCoord.CoorX;
        CurCoordinate.Y = LatestCoord.CoorY;
        CurCoordinate.Z = LatestCoord.CoorZ;
        CurCoordinate.E = LatestCoord.CoorE;
    }
    /* Unit Conversion.*/
    if ( IsMotionUnitTypeInch() )
    {
        Target.X = INCH_TO_MM(Coord.Coord.CoorX);
        Target.Y = INCH_TO_MM(Coord.Coord.CoorY);
        Target.Z = INCH_TO_MM(Coord.Coord.CoorZ);
        Target.E = INCH_TO_MM(Coord.Coord.CoorE);
    }
    else
    {
        Target.X = Coord.Coord.CoorX;
        Target.Y = Coord.Coord.CoorY;
        Target.Z = Coord.Coord.CoorZ;
        Target.E = Coord.Coord.CoorE;
    }
    if (Coord.FeedRate > 0.000001)
    {
        struct FeedrateAttr_t Feedrate = {0};
        StepperGetFeedrateAttr(&Feedrate);
        Feedrate.Feedrate = Coord.FeedRate / 60;
        /* Unit Conversion.*/
        if ( IsMotionUnitTypeInch() )
        {
            Feedrate.Feedrate = INCH_TO_MM( Feedrate.Feedrate );
        }
        
        StepperSetFeedrateAttr(&Feedrate);
    }
    /**
     * TO DO:
     */
	/*
    printf("-----1 LastE,TargetE:(");
    PrintFloat(LatestCoord.CoorE);
    PrintFloat(Target.E);
    printf(")\n");
	*/
    //CrPlanCalc(&CurCoordinate,&Target, Coord.RetractFlag);

    if ( !AllSteppersEnabled() )
    {
        EnableAllSteppers();
    }

#if 0

    struct Coordinate3d_t Result = {0};
    FLOAT DeltaAxis[] = {Target.X - CurCoordinate.X, Target.Y - CurCoordinate.Y, Target.Z - CurCoordinate.Z, Target.E - CurCoordinate.E};

    U32 LineLength = CalcSqrt(SQR(DeltaAxis[X]) + SQR(DeltaAxis[Y]) + SQR(DeltaAxis[Z]) + SQR(DeltaAxis[E]));
//
//    rt_kprintf("CurCoordinate.Y");
//    PrintFloat(CurCoordinate.Y);
//    rt_kprintf("Target.Y");
//    PrintFloat(Target.Y);
//    rt_kprintf("LineLength");
//    PrintFloat(LineLength);
//    rt_kprintf("\r\n");

//    if(LineLength < EOL_DATA)                   //如果线段太短，则放弃入队
//    {
//        return 0;
//    }

    if(!DeltaAxis[X] && !DeltaAxis[Y])        //当XY没有移动时，不需要分段入队
    {
        if(Coord.RetractFlag)
        {
            ret = PlanRetract(LineBuff, &CurCoordinate, &Target);
        }
        else
        {
            ret = PlanLine(LineBuff, &CurCoordinate, &Target);
        }

        if (ret < 0)
        {
            return ret;
        }

        CorrectTargetCoordinate(&CurCoordinate, &Target, &Result);    //LastCoord使用的是毫米，所以不用进行英寸转毫米
        LatestCoord.CoorX = Result.X;
        LatestCoord.CoorY = Result.Y;
        LatestCoord.CoorZ = Result.Z;
        LatestCoord.CoorE = Result.E;
    }
    else
    {
        U32 SegmentCnt = MAX(1,LineLength / SEGMENT_LENGTH); //分段数。如果线段长度小于SEGMENT_LENGTH，则不分段
        BOOL Segment = 0;
        FLOAT InverSegmentCnt = 1.0 / SegmentCnt;
        FLOAT SegmentLen[AxisNum] = {0.0};
        struct Coordinate3d_t SegmentCur = {.X = CurCoordinate.X,.Y = CurCoordinate.Y,.Z = CurCoordinate.Z,.E = CurCoordinate.E};
        struct Coordinate3d_t SegmentTar = {0};

        if(SegmentCnt > 1)
        {
            Segment = 1;
        }

        LOOP_XYZE(i)
        {
            SegmentLen[i] = DeltaAxis[i] * InverSegmentCnt;  //计算各个轴分段长度
        }

        U32 Index = 1;

        do
        {
            SegmentTar.X = SegmentCur.X + SegmentLen[X];

            SegmentTar.Y = SegmentCur.Y + SegmentLen[Y];

            SegmentTar.Z = SegmentCur.Z + SegmentLen[Z];

            SegmentTar.E = SegmentCur.E + SegmentLen[E];

            if(Coord.RetractFlag)
            {
                ret = PlanRetract(LineBuff, &SegmentCur, &SegmentTar,Segment);
            }
            else
            {
                ret = PlanLine(LineBuff, &SegmentCur, &SegmentTar,Segment);
            }

            if (ret < 0)
            {
//                return ret;
                CrM_Sleep(1);

                continue;
            }

//            rt_kprintf("SegmentCur.Y(%d/%d)",Index,SegmentCnt);
//            PrintFloat(SegmentCur.Y);
//            rt_kprintf("SegmentTar.Y(%d/%d)",Index,SegmentCnt);
//            PrintFloat(SegmentTar.Y);
//            rt_kprintf("\r\n");

            CorrectTargetCoordinate(&SegmentCur, &SegmentTar, &Result);    //LastCoord使用的是毫米，所以不用进行英寸转毫米

            LatestCoord.CoorX = Result.X;
            LatestCoord.CoorY = Result.Y;
            LatestCoord.CoorZ = Result.Z;
            LatestCoord.CoorE = Result.E;

            SegmentCur = SegmentTar;

            Index++;

            //CrM_Sleep(1);

        }while(SegmentCnt >= Index);
    }

#else

    if(Coord.RetractFlag)
        ret = PlanRetract(LineBuff, &CurCoordinate, &Target);
    else
        ret = PlanLine(LineBuff, &CurCoordinate, &Target);
//    printf("-----2 LastE,TargetE:(");
//    PrintFloat(LatestCoord.CoorE);
//    PrintFloat(Target.E);
//    printf(")\n");
    if (ret < 0)
    {
        return ret;
    }
    struct Coordinate3d_t Result = {0};
    CorrectTargetCoordinate(&CurCoordinate, &Target, &Result);    //LastCoord使用的是毫米，所以不用进行英寸转毫米
    LatestCoord.CoorX = Result.X;
    LatestCoord.CoorY = Result.Y;
    LatestCoord.CoorZ = Result.Z;
    LatestCoord.CoorE = Result.E;
	/*
    printf("-----3 LastE,TargetE:(");
    PrintFloat(LatestCoord.CoorE);
    PrintFloat(Target.E);
    printf(")\n");
	*/

#endif
    return 1;
#if 0
    /* Update Current Finish Point Coordinate.*/
    LatestCoord.CoorX = CurCoordinate.X;
    LatestCoord.CoorY = CurCoordinate.Y;
    LatestCoord.CoorZ = CurCoordinate.Z;
    LatestCoord.CoorE = CurCoordinate.E;
#endif
}
/**
 * 函数功能: 紧急停止
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 0:成功入队,否则入队失败
 */

S32 CrMotion::StopMotionUrgently()
{
    SteppersStopQuick();
    struct StepperPos_t PosX = {0};
    struct StepperPos_t PosY = {0};
    struct StepperPos_t PosZ = {0};
    struct StepperPos_t PosE = {0};
    
    struct StepperAttr_t AttrX = {0};
    struct StepperAttr_t AttrY = {0};
    struct StepperAttr_t AttrZ = {0};
    struct StepperAttr_t AttrE = {0};

    StepperGetAttr(&AttrX, X);
    StepperGetAttr(&AttrY, Y);
    StepperGetAttr(&AttrZ, Z);
    StepperGetAttr(&AttrE, E);

    StepperGetPos(&PosX, X);
    StepperGetPos(&PosY, Y);
    StepperGetPos(&PosZ, Z);
    StepperGetPos(&PosE, E);

    /* update current finish point coordination.*/
    LatestCoord.CoorX = (FLOAT)PosX.CurPosition / AttrX.StepsPerMm;
    LatestCoord.CoorY = (FLOAT)PosY.CurPosition / AttrY.StepsPerMm;
    LatestCoord.CoorZ = (FLOAT)PosZ.CurPosition / AttrZ.StepsPerMm;
    LatestCoord.CoorE = (FLOAT)PosE.CurPosition / AttrE.StepsPerMm;
    return 0;
}
/**
 * 函数功能: 等待所有运动完成后，归零
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 0:成功
 */
S32 CrMotion::StopMotionSafely()
{
    SteppersStopSafely();

    U8 Mark = 0;
    Mark = Mark | HOME_OPT_X | HOME_OPT_Y | HOME_OPT_Z | HOME_OPT_R;
    AutoMoveHome( Mark );

    return 0;
}
/********************** extened by TingFengXuan **********************/
/**
 * 函数功能: 设置相对模式运动
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::SetRelativeMotion()
{
    IsRelativeMotion = 1;
    IsRelativeExtrude = 1;
}
/**
 * 函数功能: 设置坐标绝对模式运动
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::SetAbsluteMotion()
{
    IsRelativeMotion = 0;
    IsRelativeExtrude = 0;
}
/**
 * 函数功能: 设置相对模式挤出
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::SetRelativeExtrude()
{
    IsRelativeExtrude = 1;
}
/**
 * 函数功能: 设置绝对坐标模式挤出
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::SetAbsluteExtrude()
{
    IsRelativeExtrude = 0;
}
/**
 * 函数功能: 判断是否为相对运动模式
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 1:相对模式,0:绝对模式
 */
BOOL CrMotion::IsRelativeMotionMode()
{
    return (IsRelativeMotion == 1);
}
/**
 * 函数功能: 判断是否为相对挤出模式
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 1:相对模式,0:绝对模式
 */
BOOL CrMotion::IsRelativeExtrudeMode()
{
    return (IsRelativeExtrude == 1);
}
/**
 * 函数功能: 运动暂停
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::MotionPause()
{
    struct Coordinate3d_t CurCoordinate = {0};
    struct Coordinate3d_t Target = {0};

    CurCoordinate.X = LatestCoord.CoorX;
    CurCoordinate.Y = LatestCoord.CoorY;
    CurCoordinate.Z = LatestCoord.CoorZ;
    CurCoordinate.E = LatestCoord.CoorE;
    /* retract E Axis.*/
    Target.X = LatestCoord.CoorX;
    Target.Y = LatestCoord.CoorY;
    Target.Z = LatestCoord.CoorZ;
    Target.E = LatestCoord.CoorE - 5.0;
    while( PlanRetract(LineBuff, &CurCoordinate, &Target) < 0 )
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }
    /* Rasie Z Axis.*/
    Target.X = CurCoordinate.X;
    Target.Y = CurCoordinate.Y;
    Target.Z = CurCoordinate.Z + 2.0;
    Target.E = CurCoordinate.E;
    while(PlanLine(LineBuff, &CurCoordinate, &Target) < 0)
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }
    /* X,Y home.*/
    Target.X = 2.0f;
    Target.Y = 2.0f;
    Target.Z = CurCoordinate.Z;
    Target.E = CurCoordinate.E;
    while( PlanLine(LineBuff, &CurCoordinate, &Target) < 0)
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }
    /* Save pause Coordinate.*/
    PauseCoord.CoorX = CurCoordinate.X;
    PauseCoord.CoorY = CurCoordinate.Y;
    PauseCoord.CoorZ = CurCoordinate.Z;
    PauseCoord.CoorE = CurCoordinate.E;
}
/**
 * 函数功能: 运动恢复
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::MotionResume()
{
    struct Coordinate3d_t CurCoordinate = {0};
    struct Coordinate3d_t Target = {0};

    CurCoordinate.X = PauseCoord.CoorX;
    CurCoordinate.Y = PauseCoord.CoorY;
    CurCoordinate.Z = PauseCoord.CoorZ;
    CurCoordinate.E = PauseCoord.CoorE;
    /* At First X,Y Resume.*/
    Target.X = LatestCoord.CoorX;
    Target.Y = LatestCoord.CoorY;
    Target.Z = CurCoordinate.Z;
    Target.E = CurCoordinate.E;
    while( PlanLine(LineBuff, &CurCoordinate, &Target) < 0 )
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }

    /* Then,Z Axis Resume.*/
    Target.X = CurCoordinate.X;
    Target.Y = CurCoordinate.Y;
    Target.Z = LatestCoord.CoorZ;
    Target.E = CurCoordinate.E;
    while( PlanLine(LineBuff, &CurCoordinate, &Target) < 0 )
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }
    /* extrude E Axis.*/
    Target.X = CurCoordinate.X;
    Target.Y = CurCoordinate.Y;
    Target.Z = CurCoordinate.Z;
    Target.E = LatestCoord.CoorE;
    while( PlanRetract(LineBuff, &CurCoordinate, &Target) < 0)
    {
        CrM_Sleep(SAFE_DELAY_MS);
    }
}
/**
 * 函数功能: 运动停止
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */

VOID CrMotion::MotionStop()
{
//    MotionPause();
    StopMotionUrgently();
    rt_kprintf("%s\n", __FUNCTION__);
    U8 Mark = 0;
    Mark = Mark | HOME_OPT_X | HOME_OPT_Y | HOME_OPT_Z | HOME_OPT_R;
    AutoMoveHome(Mark);
    /* disable stepper when stop print.*/
    DisableAllSteppers();

    memset(&LatestCoord, 0, sizeof(struct MotionCoord_t));
}
/**
 * 函数功能: 判断运动单位是否为英寸
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 1:单位为英寸,0:单位不是英寸
 */
BOOL CrMotion::IsMotionUnitTypeInch()           //运动单位是否为英寸
{
    return MotionUnit == Inch;
}
VOID CrMotion::SetMotionUnitType(enum MotionUnitType_t Unit) //设置运动单位
{
    if (MotionUnit == Unit)
    {
        return;
    }
    
    /* motion unit change to inch*/
    if(Inch == Unit )
    {
        LatestCoord.CoorX = MM_TO_INCH(LatestCoord.CoorX);
        LatestCoord.CoorY = MM_TO_INCH(LatestCoord.CoorY);
        LatestCoord.CoorZ = MM_TO_INCH(LatestCoord.CoorZ);
        LatestCoord.CoorE = MM_TO_INCH(LatestCoord.CoorE);

        PauseCoord.CoorX = MM_TO_INCH(PauseCoord.CoorX);
        PauseCoord.CoorY = MM_TO_INCH(PauseCoord.CoorY);
        PauseCoord.CoorZ = MM_TO_INCH(PauseCoord.CoorZ);
        PauseCoord.CoorE = MM_TO_INCH(PauseCoord.CoorE);
    }
    else
    {
        LatestCoord.CoorX = INCH_TO_MM(LatestCoord.CoorX);
        LatestCoord.CoorY = INCH_TO_MM(LatestCoord.CoorY);
        LatestCoord.CoorZ = INCH_TO_MM(LatestCoord.CoorZ);
        LatestCoord.CoorE = INCH_TO_MM(LatestCoord.CoorE);

        PauseCoord.CoorX = INCH_TO_MM(PauseCoord.CoorX);
        PauseCoord.CoorY = INCH_TO_MM(PauseCoord.CoorY);
        PauseCoord.CoorZ = INCH_TO_MM(PauseCoord.CoorZ);
        PauseCoord.CoorE = INCH_TO_MM(PauseCoord.CoorE);
    }
    MotionUnit = Unit;
}
/**
 * 函数功能: 获取运动速度属性
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 当前运动属性结构体
 */
struct FeedrateAttr_t CrMotion::GetMotionFeedRateAttr()
{
    struct FeedrateAttr_t Attr = {0};
    StepperGetFeedrateAttr(&Attr);
    return Attr;
}
/**
 * 函数功能: 设置运动速度属性
 * 函数参数: Feedrate:新运动参数引用
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::SetMotionFeedRateAttr(struct FeedrateAttr_t &Feedrate)
{
    StepperSetFeedrateAttr(&Feedrate);
}

/****************************** extened by guojin·chen ******************************/
VOID CrMotion::DisableStepper(enum StepperType_t Type)     //失能电机
{
    while ( !CrBlockIsFinished() )
    {
        /* waitting for moving finish.*/
        CrM_Sleep(10);
    }

    switch(Type)
    {
    case StepperX:
        SteppersDisable( StepperX_Fd );
    break;
    case StepperY:
        SteppersDisable( StepperY_Fd );
    break;
    case StepperZ:
        SteppersDisable( StepperZ_Fd );
    break;
    case StepperE:
        SteppersDisable( StepperE_Fd );
    break;
    }
}

VOID CrMotion::EnableStepper(enum StepperType_t Type)      //使能电机
{
    switch(Type)
    {
    case StepperX:
        SteppersEnable( StepperX_Fd );
    break;
    case StepperY:
        SteppersEnable( StepperY_Fd );
    break;
    case StepperZ:
        SteppersEnable( StepperZ_Fd );
    break;
    case StepperE:
        SteppersEnable( StepperE_Fd );
    break;
    }

}
/**
 * 函数功能: 清空运动直线队列
 * 函数参数: 无
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::ClearPlanQueue()                            //清空规划队列
{
    PlanLineBuffClear(LineBuff);
}
/**
 * 函数功能: 设置电机属性
 * 函数参数: MotionAttr:属性结构体，Axis:要设置的电机轴
 * 输入: 无
 * 输出: 无
 * 返回值: 无
 */
VOID CrMotion::SetStepperMotion(struct StepperMotionAttr_t MotionAttr, enum StepperType_t Axis)
{
    S32 ret = -1;
    switch (Axis)
    {
    case StepperX:
        ret = StepperSetMotionAttr(&MotionAttr, X);
    break;
    case StepperY:
        ret = StepperSetMotionAttr(&MotionAttr, Y);
    break;
    case StepperZ:
        ret = StepperSetMotionAttr(&MotionAttr, Z);
    break;
    case StepperE:
        ret = StepperSetMotionAttr(&MotionAttr, E);
    break;
    }
    if ( ret < 0 )
    {
        rt_kprintf("Set Motion Attr Failed!\n");
    }
}
/**
 * 函数功能: 获取电机属性
 * 函数参数: Axis:要获取的电机轴
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 电机属性
 */
struct StepperMotionAttr_t CrMotion::GetSetpperMotion(enum StepperType_t Axis)
{
    struct StepperMotionAttr_t MotionAttr = {0};
    S32 ret = -1;
    switch (Axis)
    {
    case StepperX:
        ret = StepperGetMotionAttr(&MotionAttr, X);
    break;
    case StepperY:
        ret = StepperGetMotionAttr(&MotionAttr, Y);
    break;
    case StepperZ:
        ret = StepperGetMotionAttr(&MotionAttr, Z);
    break;
    case StepperE:
        ret = StepperGetMotionAttr(&MotionAttr, E);
    break;
    }
    if ( ret < 0)
    {
        rt_kprintf("Get Motion Attr Failed!\n");
    }
    return MotionAttr;
}
/**
 * 函数功能: 获取当前坐标位置
 * 函数参数: 无
 * 输入: 无
 * 输出: 参考返回值
 * 返回值: 电机当前坐标
 */
struct MotionPositionMsg_t CrMotion::GetCurPosition()
{
    struct StepperPos_t PosX = {0};
    struct StepperPos_t PosY = {0};
    struct StepperPos_t PosZ = {0};
    
    struct StepperAttr_t AttrX;
    struct StepperAttr_t AttrY;
    struct StepperAttr_t AttrZ;

    StepperGetAttr(&AttrX,X);
    StepperGetAttr(&AttrY,Y);
    StepperGetAttr(&AttrZ,Z);
    
    StepperGetPos(&PosX,X);
    StepperGetPos(&PosY,Y);
    StepperGetPos(&PosZ,Z);

    CurPos.PosX = (FLOAT)PosX.CurPosition / AttrX.StepsPerMm;
    CurPos.PosY = (FLOAT)PosY.CurPosition / AttrY.StepsPerMm;
    CurPos.PosZ = (FLOAT)PosZ.CurPosition / AttrZ.StepsPerMm;
    return CurPos;
}

VOID CrMotion::EndStopGloballyMark(BOOL Mark)
{

    return;
}

BOOL CrMotion::EndStopStatus(enum Axis_t Axis)
{
    return true;
}


VOID CrMotion::SetAxisHomeOffset(enum Axis_t Axis, FLOAT Offset)
{

    return;
}

FLOAT GetAxisHomeOffset(enum Axis_t axis)
{

    return 0.0f;
}

VOID CrMotion::SetSoftEndStop(U8 Enabled)
{

}

BOOL CrMotion::SoftEndStopEnabled()
{

    return true;
}

BOOL CrMotion::AllSteppersEnabled()
{
    BOOL ENABLED = true;
    if ( !SteppersEnableStatus(StepperE_Fd) || !SteppersEnableStatus(StepperX_Fd)
            || !SteppersEnableStatus(StepperY_Fd) || !SteppersEnableStatus(StepperZ_Fd) )
        ENABLED = false;

    return ENABLED;
}

VOID CrMotion::EnableAllSteppers()
{
    SteppersEnable(StepperE_Fd);
    SteppersEnable(StepperZ_Fd);
    SteppersEnable(StepperX_Fd);
    SteppersEnable(StepperY_Fd);
}

S32 CrMotion::GetStepperFd(enum StepperType_t Axis)
{
    S32 Fd = -1;
	
    switch (Axis)
    {
        case StepperX:
        {
            Fd = StepperX_Fd;
            break;
        }
        case StepperY:
        {
            Fd = StepperY_Fd;
            break;
        }
        case StepperZ:
        {
            Fd = StepperZ_Fd;
            break;
        }
        case StepperE:
        {
            Fd = StepperE_Fd;
            break;
        }
        default:
        {
            break;
        }
    }

    return Fd;
}
