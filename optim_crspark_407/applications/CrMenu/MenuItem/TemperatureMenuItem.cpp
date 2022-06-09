/*
#include <TemperatureMenuItem.cpp>
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-16     cx2470       the first version
 */
#include "TemperatureMenuItem.h"
#include "../Show/LcmShow.h"
#include "PreheatMaterialMenuitem.h"
#include "../Show/ShowAction.h"
#include "SpecialDisplay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtthread.h>
#include "CrInc/CrMsgType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrLcdShowStruct.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

struct ViewHeaterMsg_t BedDeploy = {0};
struct ViewHeaterMsg_t NozDeploy = {0};
struct ViewFanSpeedMsg_t FanDeploy = {FAN_SPEED_GET,0};

static S32 NozPrevTick = 0;
static S32 FanPrevTick = 0;
static S32 BedPrevTick = 0;

static CrMsg_t HeaterBedRecv = NULL;
static CrMsgQueue_t HeaterBedSend = NULL;
static CrMsg_t HeaterNozRecv = NULL;
static CrMsgQueue_t HeaterNozSend = NULL;

static CrMsg_t FansRecv = NULL;
static CrMsgQueue_t FansSend = NULL;

/*Wait after sending successfully  (ms)*/
#define OK_ACTION_SEND_SUCCESS_WAIT   20

/*interval time (ms)*/
#define UPDATE_INFO_TIMEOUT     2000

VOID TemperQueueInit()
{
    HeaterNozSend = InValidMsgQueue();
    HeaterBedSend = InValidMsgQueue();
    FansSend = InValidMsgQueue();

    HeaterBedSend = ViewSendQueueOpen((S8 *)TEMPER_BED_REQUEST_QUEUE, sizeof(struct ViewHeaterMsg_t));
    HeaterBedRecv = ViewRecvQueueOpen((S8 *)TEMPER_BED_REPORT_QUEUE, sizeof(struct ViewHeaterMsg_t));
    HeaterNozSend = ViewSendQueueOpen((S8 *)TEMPER_NOZZLE_REQUEST_QUEUE, sizeof(struct ViewHeaterMsg_t));
    HeaterNozRecv = ViewRecvQueueOpen((S8 *)TEMPER_NOZZLE_REPORT_QUEUE, sizeof(struct ViewHeaterMsg_t));

    FansSend = ViewSendQueueOpen((S8 *)FANS_REQUEST_QUEUE, sizeof(struct ViewFanSpeedMsg_t));
    FansRecv = ViewRecvQueueOpen((S8 *)FANS_REPORT_QUEUE, sizeof(struct ViewFanSpeedMsg_t));

}

static S32 GetTemperInfo(CrMsgQueue_t SendHand, CrMsg_t RecvHand, S32 Size, VOID *Outer, VOID *Inner)
{
    if ( !Outer || !Inner || !IsValidMsgQueue(SendHand) || !RecvHand )
        return 0;

    S32 Ret = QueueSendMsg(SendHand, (S8 *)Inner, Size, 1);

    if ( Ret != 0 )
        return 0;

    CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);

    U32 MsgPri = 1;
    S8 Buff[Size] = {0};
    if ( CrMsgRecv(RecvHand, Buff, sizeof(Buff), &MsgPri) < 0)
        return 0;

    memcpy(Outer, Buff, Size);

    return 1;
}

static VOID UpdateDeployInfo(S32 Flag)
{
    S32 Temp = rt_tick_get();

    if ( Flag == 1 )
    {
        /*Check the interval between getting parameters */
        CHECK_TIMEOUT(BedPrevTick, Temp);

        struct ViewHeaterMsg_t Bed = {0};
        Bed.Action = GET_HEATER_MANAGER_ATTR;
        GetTemperInfo(HeaterBedSend, HeaterBedRecv, sizeof(Bed), (VOID *)&BedDeploy, (VOID *)&Bed);
    }

    if ( Flag == 0 )
    {
        /*Check the interval between getting parameters */
        CHECK_TIMEOUT(NozPrevTick, Temp);

        struct ViewHeaterMsg_t Noz = {0};
        Noz.Action = GET_HEATER_MANAGER_ATTR;
        GetTemperInfo(HeaterNozSend, HeaterNozRecv, sizeof(Noz), (VOID *)&NozDeploy, (VOID *)&Noz);
    }

    if ( Flag == 2 )
    {
        /*Check the interval between getting parameters */
        CHECK_TIMEOUT(FanPrevTick, Temp);

        struct ViewFanSpeedMsg_t Fan = {FAN_SPEED_GET, 0};
        GetTemperInfo(FansSend, FansRecv, sizeof(Fan), (VOID *)&FanDeploy, (VOID *)&Fan);
    }
}

VOID* PreheatBedMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( BedDeploy.Attr.TargetTemper < BED_MAX_TEMP )
        (BedDeploy.Attr.TargetTemper)++;

    return NULL;
}

VOID* PreheatBedMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
   if ( BedDeploy.Attr.TargetTemper > 0 )
       (BedDeploy.Attr.TargetTemper)--;

    return NULL;
}

VOID* PreheatNozzleMenuUpAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( NozDeploy.Attr.TargetTemper < NOZZLE_MAX_TEMP )
        (NozDeploy.Attr.TargetTemper)++;

    return NULL;
}

VOID* PreheatNozzleMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
   if ( NozDeploy.Attr.TargetTemper > 0 )
        (NozDeploy.Attr.TargetTemper)--;

    return NULL;
}

S32 PreheatNozzleMenuShow(VOID *Arg)
{
    UpdateDeployInfo(0);

    return SpecialContenDisplay(TUNE_ITEM_NOZZLE_NAME_ID, NozDeploy.Attr.TargetTemper, NULL);
}

S32 PreheatBedMenuShow(VOID *Arg)
{
    UpdateDeployInfo(1);

    return SpecialContenDisplay(TUNE_ITEM_BED_NAME_ID, BedDeploy.Attr.TargetTemper, NULL);
}

VOID* PreheatBedMenuOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    BedDeploy.Action = SET_HEATER_MANAGER_ATTR;

    if ( 0 == QueueSendMsg(HeaterBedSend, (S8 *)&BedDeploy, sizeof(BedDeploy), 1) )
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
//        CrBeepAppShortBeeps(); /* 短鸣 */
    }
    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return NULL;
}

VOID* PreheatNozzleMenuOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;

    NozDeploy.Action = SET_HEATER_MANAGER_ATTR;

    if ( 0 == QueueSendMsg(HeaterNozSend, (S8 *)&NozDeploy, sizeof(NozDeploy), 1) )
    {
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
        CrBeepAppShortBeeps(); /* 短鸣 */
    }
    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return NULL;
}

/**====================================================================================
 *                           系统当前目标温度菜单的特殊显示
 * ===================================================================================*/
S32 NozzleTempSpecialDisplay(VOID *Arg)
{
    UpdateDeployInfo(0);

    S8 Temp[8] = {0};
    snprintf(Temp, sizeof(Temp), "%d", (S32)NozDeploy.Attr.TargetTemper);
    Temp[strlen(Temp) + 1] = '\0';

    return SpecialMenuDisplay(Arg, Temp);
}

S32 BedTempSpecialDisplay(VOID *Arg)
{
    UpdateDeployInfo(1);

    S8 Temp[8] = {0};
    snprintf(Temp, sizeof(Temp), "%d", (S32)BedDeploy.Attr.TargetTemper);
    Temp[strlen(Temp) + 1] = '\0';

    return SpecialMenuDisplay(Arg, Temp);
}

S32 FanSpeedSpecialDisplay(VOID *Arg)
{
    UpdateDeployInfo(2);

    S8 Temp[5] = {0};
    snprintf(Temp, sizeof(Temp), "%d%%", FanDeploy.Speed);
    Temp[strlen(Temp) + 1] = '\0';

    return SpecialMenuDisplay(Arg, Temp);
}

VOID* FansSpeedMenuUPAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( FanDeploy.Speed < 100 )
        (FanDeploy.Speed)++;

    return (VOID *)(&FanDeploy.Speed);
}

VOID* FansSpeedMenuDownAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( FanDeploy.Speed > 0 )
        (FanDeploy.Speed)--;

    return (VOID *)(&FanDeploy.Speed);
}

VOID* FansSpeedMenuOKAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return NULL;


    FanDeploy.Action = FAN_SPEED_SET;

    if ( 0 == QueueSendMsg(FansSend, (S8 *)&FanDeploy, sizeof(FanDeploy), 1) )
    {
        CrBeepAppShortBeeps(); /* 短鸣 */
        CrM_Sleep(OK_ACTION_SEND_SUCCESS_WAIT);
    }

    S32 *BackParent = (S32 *)ArgOut;
    *BackParent = BACK_PARENT;

    return (VOID *)&FanDeploy.Speed;
}


S32 FansSpeedMenuShow(VOID *Arg)
{
    UpdateDeployInfo(2);

    S8 Temp[5] = {0};
    snprintf(Temp, sizeof(Temp), "%3d%%", FanDeploy.Speed);
    Temp[strlen(Temp) + 1] = '\0';

    return SpecialContenDisplay(TUNE_ITEM_FANSPEED_NAME_ID, 0, Temp);
}

VOID ClearTemptureShowTime(VOID)
{
    NozPrevTick = 0;
    FanPrevTick = 0;
    BedPrevTick = 0;
}
