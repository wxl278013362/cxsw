/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-11     cx2470       the first version
 */
#ifndef _CRFANS_H_
#define _CRFANS_H_

#include "CrInc/CrType.h"
#include "CrModel.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrHeater/Pwm/CrPwm.h"

#define FANS_DEFAULT_SPEED  (0)
#define FANS_IDLE_SPEED     (0)
#define FAN_FULL_SPEED      (255)
#define FANS_IDLE_TIME      (60)

#define PWM_FAN_COUNT 1

class CrFans : public CrModel
{
public:
    CrFans(S32 FansFd, BOOL VaildLevel, S8 *SendQueueName, S8 *RecvQueueName, FuncType Func);
    ~CrFans();

    VOID Exec();
    VOID DelayedTurnOff(S32 DelayTime);

    struct PwmControllerAttr_t GetFanAttr(){return Pwm->CrPwmControllerGetAttr();}
    VOID SetFanAttr(struct PwmControllerAttr_t &Attr){Pwm->CrPwmControllerSetAttr(Attr);}
    S32 GetFanFd(){return Fd;}
protected:

    S32 HandleRecvMsg(S8 *Buff, S32 Len);
    static VOID DelayedTurnOffCallback(VOID *Param);

private:
    CrPwmController *Pwm;

    CrMsgQueue_t RecvQueue;
    CrMsg_t SendQueue;   //向view发送HeaterManagerAttr_t
    CrTimer_t Timer;
    S32 Fd;
};


extern CrFans* PwmFan[PWM_FAN_COUNT];

#endif /* _CRFANS_H_ */
