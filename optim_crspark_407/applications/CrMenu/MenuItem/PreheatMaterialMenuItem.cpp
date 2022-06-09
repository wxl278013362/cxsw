/*
#include <Menu.h-bak>
#include <PreheatMaterialMenuItem.h>
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-16     cx2470       the first version
 */
#include "PreheatMaterialMenuitem.h"
#include "TemperatureMenuItem.h"
#include "../MenuType.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrMsgQueue/CommonQueue/CrMsgQueue.h"
#include "SpecialDisplay.h"
#include "CrInc/CrMsgType.h"
#include "CrInc/CrConfig.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

//速度与parammodel交互和运动的交互
static CrMsg_t ParamTemperRecv = NULL;
static CrMsg_t HeaterBedRecv = NULL;
static CrMsg_t HeaterNozzleRecv = NULL;
static CrMsg_t FansParamRecv = NULL;

static CrMsgQueue_t ParamTemperSend = NULL;
static CrMsgQueue_t HeaterBedSend = NULL;
static CrMsgQueue_t HeaterNozzleSend = NULL;
static CrMsgQueue_t FansParamSend = NULL;
static CrMsgQueue_t FansSetSend = NULL;

struct ViewFanSpeedParamMsg_t Fans = {0};
struct ViewFanSpeedMsg_t CurrentFans = {FAN_SPEED_GET,0};

static struct ViewTargetTemperSetMsg_t TemperSet = {0};
struct ViewHeaterMsg_t Bed;
struct ViewHeaterMsg_t Nozzle;
static S32 TempPrevTick = 0;
static S32 FanPrevTick = 0;

#define OK_ACTION_SEND_SUCCESS_WAIT   80

VOID TargetTemperQueueInit()
{
    ParamTemperSend = InValidMsgQueue();
    HeaterBedSend = InValidMsgQueue();
    HeaterNozzleSend = InValidMsgQueue();

    ParamTemperRecv = ViewRecvQueueOpen((S8 *)PARAM_TEMPER_REPORT_QUEUE, sizeof(struct ViewTargetTemperSetMsg_t));
    ParamTemperSend = ViewSendQueueOpen((S8 *)PARAM_TEMPER_SET_QUEUE, sizeof(struct ViewTargetTemperSetMsg_t));
    HeaterBedSend = ViewSendQueueOpen((S8 *)TEMPER_BED_REQUEST_QUEUE, sizeof(struct ViewHeaterMsg_t));
    HeaterBedRecv = ViewRecvQueueOpen((S8 *)TEMPER_BED_REPORT_QUEUE, sizeof(struct ViewHeaterMsg_t));
    HeaterNozzleSend = ViewSendQueueOpen((S8 *)TEMPER_NOZZLE_REQUEST_QUEUE, sizeof(struct ViewHeaterMsg_t));
    HeaterNozzleRecv = ViewRecvQueueOpen((S8 *)TEMPER_NOZZLE_REPORT_QUEUE, sizeof(struct ViewHeaterMsg_t));


    FansParamSend = ViewSendQueueOpen((S8 *)PARAM_FANS_REQUEST_QUEUE, sizeof(struct ViewFanSpeedParamMsg_t));
    FansParamRecv = ViewRecvQueueOpen((S8 *)PARAM_FANS_REPORT_QUEUE, sizeof(struct ViewFanSpeedParamMsg_t));

    FansSetSend = ViewSendQueueOpen((S8 *)FANS_REQUEST_QUEUE, sizeof(struct ViewFanSpeedMsg_t));

}

S32 NozzleTemperGetParm(struct ViewHeaterMsg_t *Accele, struct ViewHeaterMsg_t *OutAttr)
{
    if ( !Accele || !OutAttr || !IsValidMsgQueue(HeaterNozzleSend) || !HeaterNozzleRecv )
        return 0;

    if ( QueueSendMsg(HeaterNozzleSend, (S8 *)Accele, sizeof(struct ViewHeaterMsg_t), 1) != 0 )
        return 0;

    CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);

    U32 MsgPri = 1;

    S8 Buff[sizeof(struct ViewHeaterMsg_t)] = {0};
    if ( CrMsgRecv(HeaterNozzleRecv, Buff, sizeof(Buff), &MsgPri) < 0)
    {
        return 0;
    }

    memcpy(OutAttr, Buff, sizeof(struct ViewHeaterMsg_t));

    return 1;
}

S32 BedTemperGetParm(struct ViewHeaterMsg_t *Accele, struct ViewHeaterMsg_t *OutAttr)
{
    if ( !Accele || !OutAttr || !IsValidMsgQueue(HeaterBedSend) || !HeaterBedRecv )
        return 0;

    U32 MsgPri = 1;

    if ( QueueSendMsg(HeaterBedSend, (S8 *)Accele, sizeof(struct ViewHeaterMsg_t), MsgPri) != 0 )
        return 0;

    CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);

    S8 Buff[sizeof(struct ViewHeaterMsg_t)] = {0};
    if ( CrMsgRecv(HeaterBedRecv, Buff, sizeof(Buff), &MsgPri) < 0)
    {
        return 0;
    }

    memcpy(OutAttr, Buff, sizeof(struct ViewHeaterMsg_t));

    return 1;
}


S32 TemperGetTemperInfo(struct ViewTargetTemperSetMsg_t *Accele, struct ViewTargetTemperSetMsg_t *OutAttr)
{
    if ( !Accele || !OutAttr || !IsValidMsgQueue(ParamTemperSend) || !ParamTemperRecv )
        return 0;

    U32 MsgPri = 1;

    if ( 0 != QueueSendMsg(ParamTemperSend, (S8 *)Accele, sizeof(struct ViewTargetTemperSetMsg_t), MsgPri))
        return 0;

    CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);


    S8 Buff[sizeof(struct ViewTargetTemperSetMsg_t)] = {0};
    if ( CrMsgRecv(ParamTemperRecv, Buff, sizeof(Buff), &MsgPri) < 0)
        return 0;

    memcpy(OutAttr, Buff, sizeof(struct ViewTargetTemperSetMsg_t));

    return 1;
}

static VOID UpdateTempSet(VOID)
{
    /*Check the interval between getting parameters */
    CHECK_TIMEOUT(TempPrevTick, rt_tick_get());

    TemperSet.Action = GET_TEMPER_MSG;
    struct ViewTargetTemperSetMsg_t  TemperAttr = {0};

    if ( TemperGetTemperInfo(&TemperSet, &TemperAttr) > 0 )
        memcpy(&TemperSet, &TemperAttr, sizeof(ViewTargetTemperSetMsg_t));

}

static VOID UpdateFanSet(VOID)
{
    /*Check the interval between getting parameters */
    CHECK_TIMEOUT(FanPrevTick, rt_tick_get());

    if ( !IsValidMsgQueue(FansParamSend) || !FansParamRecv )
        return ;

    Fans.Action = FAN_SPEED_GET;

    if ( QueueSendMsg(FansParamSend, (S8 *)&Fans, sizeof(struct ViewFanSpeedParamMsg_t), 1) != 0 )
        return ;

    CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);

    U32 MsgPri = 1;

    S8 Buff[sizeof(struct ViewFanSpeedParamMsg_t)] = {0};
    if ( CrMsgRecv(FansParamRecv, Buff, sizeof(Buff), &MsgPri) < 0)
        return ;

    memcpy(&Fans, Buff, sizeof(struct ViewFanSpeedParamMsg_t));

    return ;
}

VOID *PreheatPLAMenuItemOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    /*Update Temper Param*/
    UpdateTempSet();
    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_ROOT;

    struct ViewHeaterMsg_t HeaterTemper;
    BedTemperGetParm(&Bed, &HeaterTemper);
    HeaterTemper.Attr.TargetTemper = TemperSet.Set.Pla.Bed;
    HeaterTemper.Action = SET_HEATER_MANAGER_ATTR;
    S32 Ret = QueueSendMsg(HeaterBedSend, (S8 *)&HeaterTemper, sizeof(HeaterTemper), 1);

    NozzleTemperGetParm(&Nozzle, &HeaterTemper);
    HeaterTemper.Attr.TargetTemper = TemperSet.Set.Pla.Nozzle;
//    HeaterTemper.Attr.FanSpeed = TemperSet.Set.Pla.FansSpeed;
    HeaterTemper.Action = SET_HEATER_MANAGER_ATTR;
    S32 Result = QueueSendMsg(HeaterNozzleSend, (S8 *)&HeaterTemper, sizeof(HeaterTemper), 1);

    /*Update Fan Speed Param*/
    UpdateFanSet();
    CurrentFans.Action = FAN_SPEED_SET;
    CurrentFans.Speed = Fans.Fan.Pla.Speed;

    S32 FanRet = QueueSendMsg(FansSetSend, (S8 * )&CurrentFans, sizeof(CurrentFans), 1);

    if ( !Ret || !Result || !FanRet)
    {
        CrBeepAppShortBeeps(); /* 短鸣 */
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
    }

    return (VOID *)BackParent;
}

VOID *PreheatPLABedMenuItemOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    UpdateTempSet();

    struct ViewHeaterMsg_t HeaterTemper;
    BedTemperGetParm(&Bed, &HeaterTemper);
    HeaterTemper.Attr.TargetTemper = TemperSet.Set.Pla.Bed;
    HeaterTemper.Action = SET_HEATER_MANAGER_ATTR;
    if ( !QueueSendMsg(HeaterBedSend, (S8 *)&HeaterTemper, sizeof(HeaterTemper), 1))
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        CrBeepAppShortBeeps(); /* 短鸣 */
    }
    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_ROOT;

    return (VOID *)BackParent;
}

VOID *PreheatPLANozMenuItemOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    UpdateTempSet();

    struct ViewHeaterMsg_t HeaterTemper;
    NozzleTemperGetParm(&Nozzle, &HeaterTemper);
    HeaterTemper.Attr.TargetTemper = TemperSet.Set.Pla.Nozzle;
    HeaterTemper.Action = SET_HEATER_MANAGER_ATTR;
    if ( !QueueSendMsg(HeaterNozzleSend, (S8 *)&HeaterTemper, sizeof(HeaterTemper), 1))
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        CrBeepAppShortBeeps(); /* 短鸣 */
    }

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_ROOT;

    return (VOID *)BackParent;
}

VOID *PreheatABSMenuItemOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    UpdateTempSet();

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_ROOT;

    struct ViewHeaterMsg_t  HeaterTemper;

    BedTemperGetParm(&Bed, &HeaterTemper);

    HeaterTemper.Attr.TargetTemper = TemperSet.Set.Abs.Bed;
    HeaterTemper.Action = SET_HEATER_MANAGER_ATTR;
    S32 Ret = QueueSendMsg(HeaterBedSend, (S8 *)&HeaterTemper, sizeof(HeaterTemper), 1);

    NozzleTemperGetParm(&Nozzle, &HeaterTemper);
    HeaterTemper.Attr.TargetTemper = TemperSet.Set.Abs.Nozzle;
//    HeaterTemper.Attr.FanSpeed = TemperSet.Set.Abs.FansSpeed;
    HeaterTemper.Action = SET_HEATER_MANAGER_ATTR;
    S32 Result = QueueSendMsg(HeaterNozzleSend, (S8 *)&HeaterTemper, sizeof(HeaterTemper), 1);

    /*Update Fan Speed Param*/
    UpdateFanSet();
    CurrentFans.Action = FAN_SPEED_SET;
    CurrentFans.Speed = Fans.Fan.Abs.Speed;
    S32 FanRet = QueueSendMsg(FansSetSend, (S8 * )&CurrentFans, sizeof(CurrentFans), 1);

    if ( !Ret || !Result || !FanRet)
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        CrBeepAppShortBeeps(); /* 短鸣 */
    }

    return (VOID *)BackParent;
}
VOID *PreheatABSBedMenuItemOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    UpdateTempSet();

    struct ViewHeaterMsg_t HeaterTemper;
    BedTemperGetParm(&Bed, &HeaterTemper);
    HeaterTemper.Attr.TargetTemper = TemperSet.Set.Abs.Bed;
    HeaterTemper.Action = SET_HEATER_MANAGER_ATTR;

    if ( !QueueSendMsg(HeaterBedSend, (S8 *)&HeaterTemper, sizeof(HeaterTemper), 1) )
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        CrBeepAppShortBeeps(); /* 短鸣 */
    }

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_ROOT;

    return (VOID *)BackParent;
}

VOID *PreheatABSNozMenuItemOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    UpdateTempSet();

    struct ViewHeaterMsg_t HeaterTemper;
    NozzleTemperGetParm(&Nozzle, &HeaterTemper);
    HeaterTemper.Attr.TargetTemper = TemperSet.Set.Abs.Nozzle;
    HeaterTemper.Action = SET_HEATER_MANAGER_ATTR;

    if ( !QueueSendMsg(HeaterNozzleSend, (S8 *)&HeaterTemper, sizeof(HeaterTemper), 1) )
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        CrBeepAppShortBeeps(); /* 短鸣 */
    }

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_ROOT;

    return (VOID *)BackParent;
}

/**====================================================================================
 *                           PLA材料预设温度菜单的特殊显示
 * ===================================================================================*/
S32 PLANozzleTempSpecialDisplay(VOID *Arg)
{
    UpdateTempSet();

    S8 Temp[8] = {0};

    snprintf(Temp, sizeof(Temp), "%d", TemperSet.Set.Pla.Nozzle);
    return SpecialMenuDisplay(Arg, Temp);
}

S32 PLABedTempSpecialDisplay(VOID *Arg)
{
    UpdateTempSet();

    S8 Temp[8] = {0};

    snprintf(Temp, sizeof(Temp), "%d", TemperSet.Set.Pla.Bed);
    return SpecialMenuDisplay(Arg, Temp);
}

VOID* PLAPreheatBedMenuUPAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( TemperSet.Set.Pla.Bed < BED_MAX_TEMP )
        (TemperSet.Set.Pla.Bed)++;

    return NULL;
}

VOID* PLAPreheatBedMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( TemperSet.Set.Pla.Bed > 0 )
        (TemperSet.Set.Pla.Bed)--;

    return NULL;
}

VOID* PLAPreheatNozzleMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( TemperSet.Set.Pla.Nozzle < NOZZLE_MAX_TEMP )
        (TemperSet.Set.Pla.Nozzle)++;

    return NULL;
}

VOID* PLAPreheatNozzleMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( TemperSet.Set.Pla.Nozzle > 0 )
        (TemperSet.Set.Pla.Nozzle)--;

    return NULL;
}

S32 PLAPreheatBedMenuShow(VOID *Arg)
{
    UpdateTempSet();

    return SpecialContenDisplay(TUNE_ITEM_BED_NAME_ID, TemperSet.Set.Pla.Bed, NULL);
}

S32 PLAPreheatNozzleMenuShow(VOID *Arg)
{
    UpdateTempSet();

    return SpecialContenDisplay(TUNE_ITEM_NOZZLE_NAME_ID, TemperSet.Set.Pla.Nozzle, NULL);
}

VOID* PLAPreheatNozzleMenuOkAction(VOID *ArgOut, VOID *ArgIn)
{
    TemperSet.Action = SET_TEMPER_MSG;

    if ( ParamTemperSend )
        if ( !QueueSendMsg(ParamTemperSend, (S8 *)&TemperSet, sizeof(TemperSet), 1) )
        {
            CrBeepAppShortBeeps(); /* 短鸣 */
            CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        }
    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return NULL;
}

VOID* PLAPreheatBedMenuOkAction(VOID *ArgOut, VOID *ArgIn)
{
    TemperSet.Action = SET_TEMPER_MSG;

    if ( ParamTemperSend )
        if ( !QueueSendMsg(ParamTemperSend, (S8 *)&TemperSet, sizeof(TemperSet), 1) )
        {
            CrBeepAppShortBeeps(); /* 短鸣 */
            CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        }
    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return NULL;
}

/**====================================================================================
 *                           ABS材料预设温度菜单的特殊显示
 * ===================================================================================*/
S32 ABSNozzleTempSpecialDisplay(VOID *Arg)
{
    UpdateTempSet();

    S8 Temp[8] = {0};
    snprintf(Temp, sizeof(Temp), "%d", TemperSet.Set.Abs.Nozzle);
    return SpecialMenuDisplay(Arg, Temp);
}

S32 ABSBedTempSpecialDisplay(VOID *Arg)
{
    UpdateTempSet();

    S8 Temp[8] = {0};
    snprintf(Temp, sizeof(Temp), "%d", TemperSet.Set.Abs.Bed);
    return SpecialMenuDisplay(Arg, Temp);
}

VOID* ABSPreheatNozzleMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( TemperSet.Set.Abs.Nozzle < NOZZLE_MAX_TEMP )
        (TemperSet.Set.Abs.Nozzle)++;

    return NULL;
}

VOID* ABSPreheatNozzleMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( TemperSet.Set.Abs.Nozzle  > 0 )
        (TemperSet.Set.Abs.Nozzle)--;

    return NULL;
}

VOID* ABSPreheatBedMenuUPAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( TemperSet.Set.Abs.Bed < BED_MAX_TEMP )
        (TemperSet.Set.Abs.Bed)++;

    return NULL;
}

VOID* ABSPreheatBedMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( TemperSet.Set.Abs.Bed > 0 )
        (TemperSet.Set.Abs.Bed)--;

    return NULL;
}
S32 ABSPreheatBedMenuShow(VOID *Arg)
{
    UpdateTempSet();

    return SpecialContenDisplay(TUNE_ITEM_BED_NAME_ID, TemperSet.Set.Abs.Bed, NULL);
}

S32 ABSPreheatNozzleMenuShow(VOID *Arg)
{
    UpdateTempSet();

    return SpecialContenDisplay(TUNE_ITEM_NOZZLE_NAME_ID, TemperSet.Set.Abs.Nozzle, NULL);
}

VOID* ABSPreheatNozzleMenuOkAction(VOID *ArgOut, VOID *ArgIn)
{
    TemperSet.Action = SET_TEMPER_MSG;

//    CrBeepAppShortBeeps(); /* 短鸣 */

    if ( ParamTemperSend )
        if ( !QueueSendMsg(ParamTemperSend, (S8 *)&TemperSet, sizeof(TemperSet), 1) )
            CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return NULL;
}

VOID* ABSPreheatBedMenuOkAction(VOID *ArgOut, VOID *ArgIn)
{
    TemperSet.Action = SET_TEMPER_MSG;

//    CrBeepAppShortBeeps(); /* 短鸣 */

    if ( ParamTemperSend )
        if ( !QueueSendMsg(ParamTemperSend, (S8 *)&TemperSet, sizeof(TemperSet), 1) )
            CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return NULL;
}

S32 PLAFanSpeedSpecialDisplay(VOID *Arg)
{
    UpdateFanSet();

    S8 Temp[5] = {0};
    snprintf(Temp, sizeof(Temp), "%d%%", Fans.Fan.Pla.Speed);
    return SpecialMenuDisplay(Arg, Temp);
}

S32 ABSFanSpeedSpecialDisplay(VOID *Arg)
{
    UpdateFanSet();

    S8 Temp[5] = {0};
    snprintf(Temp, sizeof(Temp), "%d%%",  Fans.Fan.Abs.Speed);
    return SpecialMenuDisplay(Arg, Temp);
}

VOID* PLAFansSpeedMenuUPAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( Fans.Fan.Pla.Speed < 100 )
        (Fans.Fan.Pla.Speed)++;

    return (VOID *)(&Fans.Fan.Pla.Speed);
}

VOID* PLAFansSpeedMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( Fans.Fan.Pla.Speed > 0 )
        (Fans.Fan.Pla.Speed)--;

    return (VOID *)(&Fans.Fan.Pla.Speed);
}

VOID* PLAFansSpeedMenuOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
    return NULL;

    Fans.Action = FAN_SPEED_SET;
    if ( !QueueSendMsg(FansParamSend, (S8 *)&Fans, sizeof(Fans), 1))
        CrBeepAppShortBeeps(); /* 短鸣 */

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return (VOID *)&Fans.Fan.Pla.Speed;
}

S32 PLAFansSpeedMenuShow(VOID *Arg)
{
    UpdateFanSet();

    S8 Temp[5] = {0};
    snprintf(Temp, sizeof(Temp), "%3d%%", Fans.Fan.Pla.Speed);
    return SpecialContenDisplay(TUNE_ITEM_FANSPEED_NAME_ID, 0, Temp);
}

VOID* ABSFansSpeedMenuUPAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( Fans.Fan.Abs.Speed < 100 )
        (Fans.Fan.Abs.Speed)++;

    return (VOID *)(&Fans.Fan.Abs.Speed);
}

VOID* ABSFansSpeedMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( Fans.Fan.Abs.Speed > 0 )
        (Fans.Fan.Abs.Speed)--;

    return (VOID *)(& Fans.Fan.Abs.Speed );
}

S32 ABSFansSpeedMenuShow(VOID *Arg)
{
    UpdateFanSet();

    S8 Temp[5] = {0};
    snprintf(Temp, sizeof(Temp), "%3d%%", Fans.Fan.Abs.Speed );
    return SpecialContenDisplay(TUNE_ITEM_FANSPEED_NAME_ID, 0, Temp);
}

VOID* ABSFansSpeedMenuOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
    return NULL;

//    CrBeepAppShortBeeps(); /* 短鸣 */
    Fans.Action = FAN_SPEED_SET;
    QueueSendMsg(FansParamSend, (S8 *)&Fans, sizeof(Fans), 1);

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return (VOID *)& Fans.Fan.Abs.Speed ;
}

VOID ClearPreheatShowTime(VOID)
{
    TempPrevTick = 0;
    FanPrevTick = 0;
}
