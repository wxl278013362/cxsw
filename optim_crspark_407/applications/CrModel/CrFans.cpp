/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-11     cx2470       the first version
 */
#include "CrFans.h"
#include "CrHeater/Fan/CrFanCtrl.h"
#include "Common/Include/CrTimer.h"
#include <stdio.h>
#include "CrMsgQueue/QueueCommonFuns.h"

CrFans* PwmFan[PWM_FAN_COUNT] = {NULL};

CrFans::CrFans(S32 FansFd, BOOL VaildLevel, S8 *SendQueueName, S8 *RecvQueueName, FuncType Func)
{
    if ( !SendQueueName || !RecvQueueName || !Func)
        return ;

    Fd = FansFd;
    Pwm = new CrPwmController(FansFd, Func);
    struct PwmControllerAttr_t Attr = GetFanAttr();
    Attr.DefaultLevel = (S32)VaildLevel;
    Attr.Freq = 30;
    Attr.Duty = FANS_DEFAULT_SPEED;
    Attr.Times = 0;
    SetFanAttr(Attr);

    CrMsgQueueAttr_t MsgAttr;
    MsgAttr.mq_flags = O_NONBLOCK;
    MsgAttr.mq_msgsize = sizeof( struct ViewFanSpeedMsg_t );
    MsgAttr.mq_maxmsg = 1;
    RecvQueue = QueueOpen(RecvQueueName, O_CREAT | O_RDWR, 0x777, &MsgAttr);

    MsgAttr.mq_msgsize = sizeof( struct ViewFanSpeedMsg_t );
    SendQueue = SharedQueueOpen(SendQueueName, O_CREAT | O_RDWR, 0x777, &MsgAttr);
}

CrFans::~CrFans()
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

    CrGpioClose(Pwm->CrPwmControllerGetPwmFd());
    if ( Pwm )
        delete Pwm;

}

S32 CrFans::HandleRecvMsg(S8 *Buff, S32 Len)
{
    if ( !Buff || (Len < (S32)sizeof(struct ViewFanSpeedMsg_t)) )
        return -1;

    struct ViewFanSpeedMsg_t *Temp = (struct ViewFanSpeedMsg_t *)Buff;

    if ( Temp->Action == FAN_SPEED_SET )
    {
        struct PwmControllerAttr_t Attr = GetFanAttr();
        Attr.Duty = (Temp->Speed * FAN_FULL_SPEED ) / 100;
        if ( Pwm )
            Pwm->CrPwmControllerSetAttr(Attr);
    }

    return 1;
}

VOID CrFans::Exec()
{
    if ( !IsValidMsgQueue(RecvQueue) || !SendQueue )
        return ;

    S8 RecvBuf[sizeof(struct ViewFanSpeedMsg_t)];
    S32 Pri = 1;

    if ( RecvMsg(RecvQueue, RecvBuf, sizeof(RecvBuf), &Pri) > 0 )
    {
        if ( HandleRecvMsg(RecvBuf, sizeof(RecvBuf)) > 0 )
        {
            struct ViewFanSpeedMsg_t Fans = {FAN_SPEED_GET, 0};
            S32 DUty = (GetFanAttr().Duty * 100);

            Fans.Speed = (S32)((((float)DUty / (float)FAN_FULL_SPEED) * 10) + 5) / 10;

            CrMsgSend( SendQueue, (S8 *)&Fans, sizeof( struct ViewFanSpeedMsg_t ), 1);
        }
    }
}

/*Close fans*/
VOID CrFans::DelayedTurnOffCallback(VOID * Param)
{
    CrFans *Fan = (CrFans*)Param;
    struct PwmControllerAttr_t Attr = Fan->GetFanAttr();

    Attr.Duty = 0;

    if ( Fan->Pwm )
        Fan->Pwm->CrPwmControllerSetAttr(Attr);

    return;
}

VOID CrFans::DelayedTurnOff(S32 DelayTime)
{
    struct CrTimerAttr_t TimeAttr;
    TimeAttr.HardwareTimer = 0;
    TimeAttr.Mode = RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER;
    Timer = &TimeAttr;

    CrSignalEvent_t Sig;
    Sig.sigev_notify_function =  (VOID (*) (union SigVal_t))CrFans::DelayedTurnOffCallback;
    Sig.sigev_value.sival_ptr = (VOID *)this;
    CrClockid_t ClockId = 111;

    CrTimerCreate(ClockId, &Sig, &Timer);

    CrTimerSpec_t OldTimerSpac = {0};
    CrTimerSpec_t NewTimerSpac = {{((DelayTime <= 0) ? FANS_IDLE_TIME : DelayTime), 0}};
    if ( Timer )
        CrTimerSetTime(Timer, 1, &NewTimerSpac, &OldTimerSpac);
}
