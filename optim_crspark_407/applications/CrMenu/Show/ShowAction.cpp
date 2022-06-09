#include "ShowAction.h"
#include <stdlib.h>
#include <rtthread.h>

#include "CrInc/CrLcdShowStruct.h"
#include "CrInc/CrMsgStruct.h"
#include "CrInc/CrMsgType.h"
#include "CrMenu/Action/MenuDefaultAction.h"
#include "CrMenu/MenuItem/PrintMenuItem.h"
#include "CrMenu/MenuItem/SdCardMenuItem.h"
#include "../language/Language.h"
#include "CrView/CrShowView.h"
#include "Common/Include/CrTimer.h"
#include "Common/Include/CrSleep.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrMsgQueue/QueueCommonFuns.h"

#if 1
#include "CrInc/MapData.h"
#endif

LcmShow *ShowPtr = NULL;
//S32 (*)(VOID*)     DisplayAction::PDefaultDisplayFun = NULL;
//static S32 (*DefaultShowFun)(VOID *Arg) = NULL; //默认显示DisplayAction::PDefaultDisplayFun = NULL;

static S32 StartIndex = 0;
static S32 EndIndex = 1;

static BOOL SwitchMode = 0;
struct ShowPrintInfo_t ShowInfo = {0};

static CrMsgQueue_t SdStateSend  = NULL;
static CrMsgQueue_t PrintFileSend = NULL;
static CrMsgQueue_t PrintInfoSend = NULL;
static CrMsgQueue_t LocationSend = NULL;
static CrMsgQueue_t BedTempInfoSend = NULL;
static CrMsgQueue_t NozTempInfoSend = NULL;
static CrMsgQueue_t FansSpeedSend = NULL;
//static CrMsgQueue_t FeedrateRatioSend = NULL;

//static CrMsg_t FeedrateRatioRecv = NULL;
static CrMsg_t SdStateRecv = NULL;
static CrMsg_t PrintFileRecv = NULL;
static CrMsg_t PrintInfoRecv = NULL;
static CrMsg_t LocationRecv = NULL;
static CrMsg_t BedTempInfoRecv = NULL;
static CrMsg_t NozTempInfoRecv = NULL;
static CrMsg_t FansSpeedRecv = NULL;
static CrMsg_t MsgLineMsg = NULL;
static S8 LineMsg[MAX_MSG_LENGTH] = {0};
static S8 ShowLineMsg = 0;
static S8 ShowSpecialMsg = 0;
static S8 SpecialMsg[MAX_MSG_LENGTH] = {0};   //在打印过程中显示的数据

/****************************************************/

static S32 SwitchPage(S32 CurIndex);
//static S32 SwitchPageSub(S32 CurIndex, struct MenuItem_t *MenuArrayPtr, S32 ArrayEleHide, S32 *StartIndex, S32 *EndIndex);
static S32 SwitchPageSub(S32 CurIndex, struct MenuItem_t *MenuArrayPtr, S32 *StartIndex, S32 *EndIndex);
//static S32 IsItemHide(S32 i, struct MenuItem_t *Array, S32 StartIndex, S32 CurItemIndex, S16 *Index, S8 *High, S32 *Temp, S32 *Flag);    //判断Item是否隐藏

static VOID GetSdCardStatus();
static VOID GetPrintInfo();
static VOID GetPrintFileName();
static VOID GetSteppersPos();
static VOID GetTemperInfo();
static VOID GetFanSpeed();
static VOID GetLineMsg();  //获取信息行信息
//static VOID GetFeedrateRatio();
/*****************************************************/

static CrTimer_t ShowActionTimer;

S32 SwitchPage(S32 CurIndex)
{
    if ( CurIndex < 0 || !ShowPtr)
        return 0;

    if ( ShowPtr->GetMaxRow() <= 0)
        return 0;

    if ( CurIndex < StartIndex )   //
    {
        if ( SwitchMode )
        {
            StartIndex = (CurIndex / ShowPtr->GetMaxRow()) * ShowPtr->GetMaxRow();
            EndIndex = StartIndex + ShowPtr->GetMaxRow();
        }
        else
        {
            StartIndex = CurIndex;
            EndIndex = StartIndex + ShowPtr->GetMaxRow();
        }
    }
    else if ( CurIndex >= EndIndex )
    {
        if ( SwitchMode )
        {
            StartIndex = (CurIndex / ShowPtr->GetMaxRow()) * ShowPtr->GetMaxRow();
            EndIndex = StartIndex + ShowPtr->GetMaxRow();
        }
        else
        {
            EndIndex = CurIndex + 1;
            StartIndex = EndIndex - ShowPtr->GetMaxRow();
        }
    }

    //做修正
    if ( StartIndex < 0 )
        StartIndex = 0;

    if ( EndIndex < 1 )
        EndIndex = 1;

    return 1;
}

S32 SwitchPageSub(S32 CurIndex, struct MenuItem_t *MenuArrayPtr, S32 *StartIndex, S32 *EndIndex)
{
    if ( !MenuArrayPtr || !StartIndex || !EndIndex )
        return 0;

    S32 Size = GetItemsCount((VOID *)MenuArrayPtr);
    S32 Flag = 0;
    S32 Temp[Size] = {0};

    for ( S32 i = 0; i < Size ; i++ )
    {
        if ( ((MenuArrayPtr->HideMask) & (HideMask)) == 0 )
        {
            Temp[Flag] = i;
            if ( CurIndex == Temp[Flag] )
            {
                *StartIndex = (Flag / ShowPtr->GetMaxRow()) * ShowPtr->GetMaxRow();
            }
            Flag ++;
        }
        MenuArrayPtr++;
    }

    *EndIndex = Temp[Flag - 1];

    return 1;
}

VOID ShowActTimerCallback(VOID *Param)
{
    GetSdCardStatus();
    GetTemperInfo();
    GetPrintInfo();
    GetPrintFileName();
    GetSteppersPos();
    GetFanSpeed();
//    GetFeedrateRatio();
    GetLineMsg();

    return;
}

VOID GetSdCardStatus()
{
    if ( !IsValidMsgQueue(SdStateSend) || !SdStateRecv )
        return ;

    static BOOL Status = 0;
    struct ViewReqDevStatusMsg_t StateReq = {1};

    if ( 0 != QueueSendMsg(SdStateSend, (S8 *)&StateReq, sizeof(struct ViewReqDevStatusMsg_t), 1) )
        return ;

    CrM_Sleep(20);

    U32 MsgPri = 0;
    S8 Buff[sizeof(struct SdStatusMsg_t)] = {0};
    if ( CrMsgRecv(SdStateRecv,  Buff, sizeof(Buff), &MsgPri) > 0 )
    {
        struct SdStatusMsg_t *SdStatusBuff = (struct SdStatusMsg_t *)Buff;
        if ( SdStatusBuff->Inserted ==  Status) //状态
            return;

        ClearCurrentPath();

        ShowInfo.SdStatus.Status  = SdStatusBuff->Inserted;
        if ( Status )   //原来时SD卡插入状态
        {
            //将状态转变
            HideMask |= MAINMENU_ITEM_PRINTFFROMTF_HID_MASK;
            HideMask |= MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK;
            HideMask |= MAINMENU_ITEM_RESUME_PRINTF_HID_MASK;
            HideMask |= MAINMENU_ITEM_STOP_PRINTF_HID_MASK;
            HideMask &= ~MAINMENU_ITEM_NOTFCARD_HID_MASK;
        }
        else
        {
            //将状态转变
            HideMask &= ~MAINMENU_ITEM_PRINTFFROMTF_HID_MASK;
            HideMask |= MAINMENU_ITEM_NOTFCARD_HID_MASK;
            HideMask |= MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK;
            HideMask |= MAINMENU_ITEM_RESUME_PRINTF_HID_MASK;
            HideMask |= MAINMENU_ITEM_STOP_PRINTF_HID_MASK;
        }
        Status = SdStatusBuff->Inserted;
    }

    return ;
}

VOID GetPrintInfo()
{
    if ( !IsValidMsgQueue(PrintInfoSend) || !PrintInfoRecv )
        return ;

    struct ViewPrintInfoMsg_t PrintReq = {1};

    if ( 0 != QueueSendMsg(PrintInfoSend, (S8 *)&PrintReq, sizeof(struct ViewPrintInfoMsg_t), 1))
        return ;

    CrM_Sleep(20);

    S8 Buff[sizeof(struct PrintInfoMsg_t)] = {0};
    U32 MsgPri = 0;
    if ( CrMsgRecv(PrintInfoRecv, Buff, sizeof(Buff), &MsgPri) > 0 )
    {
        struct PrintInfoMsg_t *PrintInfoBuff = (struct PrintInfoMsg_t *)Buff;
        ShowInfo.Progress.Progress = PrintInfoBuff->Progress;
        ShowInfo.Time.Hour = PrintInfoBuff->TimeElapsed / 3600;
        ShowInfo.Time.Minter = (PrintInfoBuff->TimeElapsed % 3600) / 60;
        ShowInfo.Time.Second = ((PrintInfoBuff->TimeElapsed % 3600) % 60);

        if ( PrintInfoBuff->Status != ShowInfo.DevStatus.Reserve )
        {
            if ( PrintInfoBuff->Status == STOP_PRINT)
            {    //将状态转变
                if ( HideMask &  MAINMENU_ITEM_NOTFCARD_HID_MASK)   //有SD卡（没有sd卡的菜单隐藏）
                {
                    HideMask &= ~MAINMENU_ITEM_PRINTFFROMTF_HID_MASK;
                }
                else
                {
                    HideMask |= MAINMENU_ITEM_PRINTFFROMTF_HID_MASK;
                }

                HideMask |= MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK;
                HideMask |= MAINMENU_ITEM_RESUME_PRINTF_HID_MASK;
                HideMask |= MAINMENU_ITEM_STOP_PRINTF_HID_MASK;

                //同时将准备和tune的也转换过来
                HideMask &= ~MAINMENU_ITEM_PREPARE_HID_MASK;
                HideMask |= MAINMENU_ITEM_TUNE_HID_MASK;

                ShowInfo.DevStatus.Reserve = STOP_PRINT;
            }
            else if ( PrintInfoBuff->Status == PAUSE_PRINT )
            {
                 //将状态转变
                HideMask |= MAINMENU_ITEM_PRINTFFROMTF_HID_MASK;
                HideMask |= MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK;
                HideMask &= ~MAINMENU_ITEM_RESUME_PRINTF_HID_MASK;
                HideMask &= ~MAINMENU_ITEM_STOP_PRINTF_HID_MASK;

                ShowInfo.DevStatus.Reserve = PAUSE_PRINT;
            }
            else if ( PrintInfoBuff->Status == PRINTING )
            {
                //将状态转变
                HideMask |= MAINMENU_ITEM_PRINTFFROMTF_HID_MASK;
                HideMask &= ~MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK;
                HideMask |= MAINMENU_ITEM_RESUME_PRINTF_HID_MASK;
                HideMask &= ~MAINMENU_ITEM_STOP_PRINTF_HID_MASK;
                HideMask |= MAINMENU_ITEM_PREPARE_HID_MASK;
                HideMask &= ~MAINMENU_ITEM_TUNE_HID_MASK;

                ShowInfo.DevStatus.Reserve = PrintInfoBuff->Status;
            }
            else if ( PrintInfoBuff->Status == RESUME_PRINT )
            {
                //将状态转变
                HideMask |= MAINMENU_ITEM_PRINTFFROMTF_HID_MASK;
                HideMask &= ~MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK;
                HideMask |= MAINMENU_ITEM_RESUME_PRINTF_HID_MASK;
                HideMask &= ~MAINMENU_ITEM_STOP_PRINTF_HID_MASK;
                HideMask |= MAINMENU_ITEM_PREPARE_HID_MASK;
                HideMask &= ~MAINMENU_ITEM_TUNE_HID_MASK;

                ShowInfo.DevStatus.Reserve = PrintInfoBuff->Status;
            }
            else
            {
                HideMask &= ~MAINMENU_ITEM_PRINTFFROMTF_HID_MASK;
                HideMask &= ~MAINMENU_ITEM_PREPARE_HID_MASK;
                HideMask |= MAINMENU_ITEM_PAUSE_PRINTF_HID_MASK;
                HideMask |= MAINMENU_ITEM_RESUME_PRINTF_HID_MASK;
                HideMask |= MAINMENU_ITEM_STOP_PRINTF_HID_MASK;
                HideMask |= MAINMENU_ITEM_TUNE_HID_MASK;
            }
        }
    }

    return;
}

VOID GetPrintFileName()
{
    if ( !PrintFileRecv )
        return ;
//    struct ViewReqPrintFileMsg_t FileReq = {1};

//    CrM_Sleep(20);

    S8 Buff[sizeof(struct SdCardPrintFileMsg_t)] = {0};
    U32 MsgPri = 0;
    if ( CrMsgRecv(PrintFileRecv, Buff, sizeof(Buff), &MsgPri) > 0 )
    {
        struct SdCardPrintFileMsg_t *PrintFileBuff = (struct SdCardPrintFileMsg_t *)Buff;
        if ( strlen (PrintFileBuff->Name) > 0 )
        {
            strncpy(ShowInfo.File.Name, PrintFileBuff->Name, sizeof(ShowInfo.File.Name) - 1);
        }
    }

    return ;
}

#if 0
VOID GetFeedrateRatio()
{
    if ( !IsValidMsgQueue(FeedrateRatioSend) || !FeedrateRatioRecv )
        return;

    struct ViewMotionMsg_t FeedrateRatio = {0};
    FeedrateRatio.Action = GET_MOTION_MSG;

    if ( 0!= QueueSendMsg(FeedrateRatioSend, (S8 *)&FeedrateRatio, sizeof(struct ViewMotionMsg_t), 1))
        return ;

    CrM_Sleep(20);

    U32 MsgPri = 0;
    if ( CrMsgRecv(FeedrateRatioRecv, (S8 *)&FeedrateRatio, sizeof(FeedrateRatio), &MsgPri) == sizeof(FeedrateRatio) )
    {
        ShowInfo.FeedrateRatio = FeedrateRatio.MotionParam.PrintRatio;
    }

    return ;
}
#endif

VOID GetSteppersPos()
{
    if ( !IsValidMsgQueue(LocationSend) || !LocationRecv )
        return ;

    struct ViewReqPosition_t LocationReq = {1};

    if ( 0 != QueueSendMsg(LocationSend, (S8 *)&LocationReq, sizeof(struct ViewReqPosition_t), 1))
        return ;

    CrM_Sleep(20);

    S8 Buff[30] = {0};
    U32 MsgPri = 0;
    if ( CrMsgRecv(LocationRecv, Buff, sizeof(Buff), &MsgPri) > 0 )
    {
        struct MotionPositionMsg_t *MotionBuff = (struct MotionPositionMsg_t *)Buff;
        ShowInfo.AixsPos.X_Pos.PosValue = MotionBuff->PosX;
        ShowInfo.AixsPos.Y_Pos.PosValue = MotionBuff->PosY;
        ShowInfo.AixsPos.Z_Pos.PosValue = MotionBuff->PosZ;
        ShowInfo.FeedrateRatio = MotionBuff->PrintSpeed;
        ShowInfo.StepperX_Enable = MotionBuff->StepperX_Enable;
        ShowInfo.StepperY_Enable = MotionBuff->StepperY_Enable;
        ShowInfo.StepperZ_Enable = MotionBuff->StepperZ_Enable;
        ShowInfo.HomeX = MotionBuff->HomeX;
        ShowInfo.HomeY = MotionBuff->HomeY;
        ShowInfo.HomeZ = MotionBuff->HomeZ;
    }
}

VOID GetTemperInfo()
{
    if ( !IsValidMsgQueue(BedTempInfoSend) || !IsValidMsgQueue(NozTempInfoSend) || !BedTempInfoRecv || !NozTempInfoRecv )
        return ;

    //请求温度信息
    struct ViewHeaterMsg_t BedTempReq = {0};
    BedTempReq.Action = GET_HEATER_MANAGER_ATTR;
    if ( 0 != QueueSendMsg(BedTempInfoSend, (S8 *)&BedTempReq, sizeof(struct ViewHeaterMsg_t), 1))
        return ;

//    struct ViewHeaterMsg_t NozTempReq = {0};
//    NozTempReq.Action = GET_HEATER_MANAGER_ATTR;
    if ( 0 != QueueSendMsg(NozTempInfoSend, (S8 *)&BedTempReq, sizeof(struct ViewHeaterMsg_t), 1))
        return ;

    CrM_Sleep(20);

    S8 Buff[sizeof(struct HeaterManagerAttr_t)] = {0};
    U32 MsgPri = 0;

    if ( CrMsgRecv(BedTempInfoRecv, Buff, sizeof(Buff), &MsgPri) > 0 )
    {
#if 1
        struct HeaterManagerAttr_t *BedTemperBuff = (struct HeaterManagerAttr_t * )Buff;
        ShowInfo.BedTemp.Temp = BedTemperBuff->CurTemper;
        ShowInfo.PreheatBedTemp.Temp = BedTemperBuff->TargetTemper;
//        rt_kprintf("bed target temper = %d, cur temper = %d\n", (S32)ShowInfo.PreheatBedTemp.Temp, (S32)ShowInfo.BedTemp.Temp);
        if ( ShowInfo.PreheatBedTemp.Temp >= ShowInfo.BedTemp.Temp )
        {
            ShowInfo.DevStatus.BedStatus = true;
        }
        else
        {
            ShowInfo.DevStatus.BedStatus = false;
        }
#endif
    }

#if 1
    memset(Buff, 0, sizeof(Buff));
    if ( CrMsgRecv(NozTempInfoRecv, Buff, sizeof(Buff), &MsgPri) > 0 )
    {
        struct HeaterManagerAttr_t *NozTemperBuff = (struct HeaterManagerAttr_t *)Buff;
        ShowInfo.NozzleTemp.Temp = NozTemperBuff->CurTemper;
        ShowInfo.PreheatNozzleTemp.Temp = NozTemperBuff->TargetTemper;
            //处理E轴移动item的显示
        if ( ShowInfo.NozzleTemp.Temp >= NOZZLE_MIN_EXTRUDED_TEMPER )
        {
//            HideMask |= FILAMENT_ITEM_FIL_DIA_HID_MASK;//隐藏
            HideMask |= MOVEAXISEMNU_ITEM_AXISE_TOOCOLD_HID_MAKS;
            HideMask &= ~MOVEAXISEMNU_ITEM_AXISE_MAXUNIT_HID_MAKS;
            HideMask &= ~MOVEAXISEMNU_ITEM_AXISE_MIDUNIT_HID_MAKS;
            HideMask &= ~MOVEAXISEMNU_ITEM_AXISE_MINUNIT_HID_MAKS;
        }
        else
        {
//            HideMask &= ~FILAMENT_ITEM_FIL_DIA_HID_MASK;//隐藏
            HideMask &= ~MOVEAXISEMNU_ITEM_AXISE_TOOCOLD_HID_MAKS;
            HideMask |= MOVEAXISEMNU_ITEM_AXISE_MAXUNIT_HID_MAKS;
            HideMask |= MOVEAXISEMNU_ITEM_AXISE_MIDUNIT_HID_MAKS;
            HideMask |= MOVEAXISEMNU_ITEM_AXISE_MINUNIT_HID_MAKS;
        }

//        rt_kprintf("Nozzle target temper = %d, cur temper = %d\n", (S32)ShowInfo.PreheatNozzleTemp.Temp, (S32)ShowInfo.NozzleTemp.Temp);
        if ( ShowInfo.PreheatNozzleTemp.Temp >= ShowInfo.NozzleTemp.Temp )
        {
            ShowInfo.DevStatus.NozzleStatus = true;
        }
        else
        {
            ShowInfo.DevStatus.NozzleStatus = false;
        }
    }
#endif

    return ;
}

VOID GetFanSpeed()
{
    if ( ! IsValidMsgQueue(FansSpeedSend) || !FansSpeedRecv )
        return ;

    struct ViewFanSpeedMsg_t LocationReq = {FAN_SPEED_GET};

    if ( 0 != QueueSendMsg(FansSpeedSend, (S8 *)&LocationReq, sizeof(struct ViewFanSpeedMsg_t), 1))
        return ;

    CrM_Sleep(20);

    S8 Buff[sizeof(struct ViewFanSpeedMsg_t)] = {0};
    U32 MsgPri = 0;
    if ( CrMsgRecv(FansSpeedRecv, Buff, sizeof(Buff), &MsgPri) > 0 )
    {
        struct ViewFanSpeedMsg_t *Fans = (struct ViewFanSpeedMsg_t *)Buff;
        ShowInfo.DevStatus.FanSpeed = Fans->Speed;
    }
}

VOID GetLineMsg()
{
    if ( !MsgLineMsg )
        return ;

    S8 Buff[sizeof(struct MsgLine_t)] = {0};
    U32 MsgPri = 0;
    if ( CrMsgRecv(MsgLineMsg, Buff, sizeof(Buff), &MsgPri) <= 0 )
        return ;

    struct MsgLine_t *Msg = (struct MsgLine_t *)Buff;
    if ( Msg->MsgType )
    {
        if ( strcmp(Msg->Info, SpecialMsg) == 0 )   //消息有更新
            return ;

        strcpy(SpecialMsg, Msg->Info);
        ShowSpecialMsg = !(Msg->Finished);
        //rt_kprintf("recv special msg : %s, status : %d\n", Msg->Info, Msg->Finished);
    }
    else
    {
        if ( strcmp(Msg->Info, LineMsg) == 0 )   //消息有更新
            return ;

        strcpy(LineMsg, Msg->Info);
        ShowLineMsg = 1;
    }

    return ;
}

S32 ShowInit(LcmShow *ShowObjectPtr, BOOL SwitchPageMode)
{
    if ( !ShowObjectPtr )
        return 0;

    ShowPtr = ShowObjectPtr;
    SwitchMode = SwitchPageMode;
    StartIndex = 0;
    EndIndex = ShowPtr->GetMaxRow();
    if ( EndIndex <= 0 )
        EndIndex = 1;

    strcpy(ShowInfo.Name, (S8 *)DEVICE_MODE_NAME);
    SdStateSend  = InValidMsgQueue();
    PrintFileSend = InValidMsgQueue();
    PrintInfoSend = InValidMsgQueue();
    LocationSend = InValidMsgQueue();
    BedTempInfoSend = InValidMsgQueue();
    NozTempInfoSend = InValidMsgQueue();

//    FeedrateRatioRecv = ViewRecvQueueOpen((S8 *)PARAM_MOTION_REPORT_QUEUE, sizeof(struct ViewMotionMsg_t));
    SdStateRecv = ViewRecvQueueOpen((S8 *)SD_STATUS_REPORT_QUEUE, sizeof(struct SdStatusMsg_t));
    PrintInfoRecv = ViewRecvQueueOpen((S8 *)PRINT_INFO_REPORT_QUEUE, sizeof(struct PrintInfoMsg_t));
    LocationRecv = ViewRecvQueueOpen((S8 *)MOTION_POS_REPORT_QUEUE, sizeof(struct MotionPositionMsg_t));
    BedTempInfoRecv = ViewRecvQueueOpen((S8 *)TEMPER_BED_REPORT_QUEUE, sizeof(struct HeaterManagerAttr_t));
    NozTempInfoRecv = ViewRecvQueueOpen((S8 *)TEMPER_NOZZLE_REPORT_QUEUE, sizeof(struct HeaterManagerAttr_t));
    PrintFileRecv = ViewRecvQueueOpen((S8 *)SD_PRINTFILE_REPORT_QUEUE, sizeof(struct SdCardPrintFileMsg_t));
    FansSpeedRecv = ViewRecvQueueOpen((S8 *)FANS_REPORT_QUEUE, sizeof(struct ViewFanSpeedMsg_t));
    MsgLineMsg = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));

//    FeedrateRatioSend = ViewSendQueueOpen((S8 *)PARAM_MOTION_SET_QUEUE, sizeof(struct ViewMotionMsg_t));
    PrintFileSend = ViewSendQueueOpen((S8 *)SD_PRINTFILE_REQ_QUEUE, sizeof(struct ViewReqPrintFileMsg_t));
    NozTempInfoSend = ViewSendQueueOpen((S8 *)TEMPER_NOZZLE_REQUEST_QUEUE, sizeof(struct ViewHeaterMsg_t));
    BedTempInfoSend = ViewSendQueueOpen((S8 *)TEMPER_BED_REQUEST_QUEUE, sizeof(struct ViewHeaterMsg_t));
    LocationSend = ViewSendQueueOpen((S8 *)MOTION_POS_REQ_QUEUE, sizeof(struct ViewReqPosition_t));
    PrintInfoSend = ViewSendQueueOpen((S8 *)PRINT_INFO_REQ_QUEUE, sizeof(struct ViewPrintInfoMsg_t));
    SdStateSend = ViewSendQueueOpen((S8 *)SD_STATUS_REQUEST_QUEUE, sizeof(struct ViewReqDevStatusMsg_t));
    FansSpeedSend = ViewSendQueueOpen((S8 *)FANS_REQUEST_QUEUE, sizeof(struct ViewFanSpeedMsg_t));


    CrSignalEvent_t SigEvent = {0};
    SigEvent.sigev_notify_function = (VOID (*) (union SigVal_t))ShowActTimerCallback;

    CrClockid_t ClockId = 0;

    struct CrTimerAttr_t CrTimerInfo = {0};
    strcpy(CrTimerInfo.Name, "ShowAction");
    CrTimerInfo.Mode = RT_TIMER_FLAG_PERIODIC;

    ShowActionTimer = &CrTimerInfo;
    CrTimerCreate(ClockId, &SigEvent, &ShowActionTimer);
    if ( !ShowActionTimer )
    {
        rt_kprintf("**** ShowInit create timer failed ****\n");
        return 0;
    }
#if 1
    CrTimerSpec_t OldTimerSpac = {0};
    CrTimerSpec_t NewTimerSpac = {{0, 1000000000}, {10}};
    CrTimerSetTime(ShowActionTimer, 1, &NewTimerSpac, &OldTimerSpac);
#endif

    return 1;
}

LcmShow* GetShowObject()
{
    return ShowPtr;
}

S32 ShowItemName(VOID *MenuInfoPtr)
{
    if ( !ShowPtr )
        return 0;

    //暂时不实现(改变内容的显示)
    struct MenuShowInfo_t *InfoPtr = (struct MenuShowInfo_t *)MenuInfoPtr;
   // rt_kprintf("item name %s, row = %d, light = %d", InfoPtr->ShowData, InfoPtr->Index, InfoPtr->Highligh);
    S32 Res = ShowPtr->ShowString((S8 *)(InfoPtr->ShowData), InfoPtr->Index, 0, InfoPtr->Highligh);
   //  Res = ShowPtr->WriteRow((S8 *)(InfoPtr->ShowData), strlen((S8*)(InfoPtr->ShowData)), InfoPtr->Index, 0);  //测试接口
    S8 Temp[2] = {0};

    if ( InfoPtr->MenuType == RollBackTypeMenu )
    {
        Temp[0] = '^';
//        ShowPtr->Ioctl(LCD_USING_ASCII_CHARATER_SET, Temp);
        ShowPtr->WriteString(Temp, (InfoPtr->Index + 1) * ShowPtr->GetRowPixelHeight() , 123, 0);
    }
    else if ( InfoPtr->MenuType == FatherTypeMenu )
    {
        Temp[0] = '>';
//        ShowPtr->Ioctl(LCD_USING_ASCII_CHARATER_SET, Temp);
        ShowPtr->WriteString(Temp, (InfoPtr->Index + 1) * ShowPtr->GetRowPixelHeight() , 123, 0);
    }

    return Res;
}

S32 ShowItems(struct MenuItem_t *MenuArrayPtr, S32 Count, S32 CurItemIndex)
{
    //printf("1 display = %p, menu array = %p, Count = %d, CurItemIndex = %d\n", ShowPtr, MenuArrayPtr, Count, CurItemIndex);
    if ( !ShowPtr || !MenuArrayPtr || (Count <= 0) || (CurItemIndex < 0) )
        return 0;

    if ( Count == 1 )
    {
        MenuArrayPtr->Show(NULL);
        return 1;
    }

    S32 CmdParam = 0;
    MenuItem_t  *ItemPtr = MenuArrayPtr;

    //切换页
//    SwitchPage(CurItemIndex);

    SwitchPageSub(CurItemIndex, MenuArrayPtr, &StartIndex, &EndIndex);

    S32 i = 0, Offset = 0, Flag = 0, Temp[Count]= {0};
    struct MenuShowInfo_t Info;

    ItemPtr = NULL;
    S32 MaxLen = ShowPtr->GetMaxCol() + 1;//获取到的MaxLen是最多能显示的列，但是'\0'也占用了一个 故加1
    ShowPtr->Ioctl(LCD_CLEAN, (VOID *)&CmdParam);
    S8 Buff[MaxLen];

    for ( i = 0; i < Count ; i++ )
    {
        ItemPtr = (struct MenuItem_t *)((S8 *)MenuArrayPtr + Offset);
        Offset += sizeof(MenuItem_t);

        Info.Highligh = 0;
        Info.MenuType = 0;

        if ( ((ItemPtr->HideMask) & (HideMask)) == 0 )
        {
            Temp[Flag] = i;
            Info.Index = Flag - StartIndex;
            if ( Temp[Flag] == CurItemIndex )
                Info.Highligh = 1;

            Flag++;
        }
        else
            continue;

        Languages::FindMenuItemNameById(ItemPtr->ItemNameId, Buff, MaxLen);
        Info.ShowData = Buff;

        if ( (!(ItemPtr->Next)) && (!(ItemPtr->OkAction)) )  //回退菜单
        {
            Info.MenuType = RollBackTypeMenu;
        }
        else if ( ItemPtr->Next )  //子菜单
        {
            Info.MenuType = FatherTypeMenu;
        }

        if ( (ItemPtr->Show) )
            ItemPtr->Show(&Info);
    }

    ShowPtr->Ioctl(LCD_FLUSH, (VOID *)&CmdParam);

    return 1;
}

S32 ShowRoot(VOID *DataPtr)
{
    if ( !ShowPtr)
        return 0;

    static S16 HasPrintFile = 0;    //是否打印过文件
    static S16 Count = 0;

    struct ShowPrintInfo_t *Info = &ShowInfo;
    struct LcdCtrlParam_t Lcd = {0};
    S32 CmdParam = 0;

    if ( ++Count > 1000 )
        Count = 0;

    ShowPtr->Ioctl(LCD_CLEAN, (VOID*)&CmdParam);
    if ( strlen (Info->Name) <= 7 )
    {
        Lcd.X = 0;
        Lcd.Y = 16;
        ShowPtr->WriteString(Info->Name, Lcd.Y, Lcd.X);
    }
    else
    {
        Lcd.X = 0;
        Lcd.Y = 16;
        S8 Ptr[10] = {0};
        snprintf(Ptr, 8, "%s", Info->Name);
        ShowPtr->WriteString(Ptr, Lcd.Y, Lcd.X);
        sprintf(Ptr, "%s", (Info->Name + 7));
        Lcd.X = (7 - strlen(Ptr)) * (128 / ShowPtr->GetMaxCol()) / 2;//居中显示
        Lcd.Y = 25;
        ShowPtr->WriteString(Ptr, Lcd.Y, Lcd.X);
    }
    S32 Temp = 0, SubTemp = 0;
    S8  Buff[MAX_MSG_LENGTH] = {0};

    Lcd.X = 42;
    Lcd.Y = 7;
    Temp = (S32)Info->PreheatNozzleTemp.Temp;
    sprintf(Buff, "%d°", Temp);
    ShowPtr->WriteString(Buff, Lcd.Y, Lcd.X);

    Lcd.X = 42;
    Lcd.Y = 28;
    Temp = Info->NozzleTemp.Temp;
    memset(Buff, 0, sizeof(Buff));
    sprintf(Buff, "%d°", Temp);
    ShowPtr->WriteString(Buff, Lcd.Y, Lcd.X);

    Lcd.X = 75;
    Lcd.Y = 7;
    Temp = (S32)Info->PreheatBedTemp.Temp;
    memset(Buff, 0, sizeof(Buff));
    sprintf(Buff, "%d°", Temp);
    ShowPtr->WriteString(Buff, Lcd.Y, Lcd.X);

    Lcd.X = 75;
    Lcd.Y = 28;
    Temp = Info->BedTemp.Temp;
    memset(Buff, 0, sizeof(Buff));
    sprintf(Buff, "%d°", Temp);
    ShowPtr->WriteString(Buff, Lcd.Y, Lcd.X);

    Lcd.X = 13;
    Lcd.Y = 50;
    memset(Buff, 0, sizeof(Buff));
    sprintf(Buff, "%d%%", ShowInfo.FeedrateRatio);
    ShowPtr->WriteString(Buff, Lcd.Y, Lcd.X);

    Lcd.X = 58;
    Lcd.Y = 48;
    memset(Buff, 0, sizeof(Buff));
    sprintf(Buff, "%d:%02d:%02d ", Info->Time.Hour, Info->Time.Minter, Info->Time.Second);
    ShowPtr->WriteString(Buff, Lcd.Y, Lcd.X);

    Lcd.X = 104;
    Lcd.Y = 51;
    memset(Buff, 0, sizeof(Buff));
    sprintf(Buff, "%d%%", Info->Progress.Progress);
    ShowPtr->WriteString(Buff, Lcd.Y, Lcd.X);

    //绘制XYZ位置
    //绘制坐标位置的外框
    Lcd.X = 0;
    Lcd.Y = 29;
    Lcd.Width = 128;
    Lcd.Height = 11;
    ShowPtr->Ioctl(LCD_WRITE_RECTANGLE, (VOID *)&Lcd);

    //绘制X坐标
    Lcd.X = 2;
    Lcd.Y = 38;
    Temp = Info->AixsPos.X_Pos.PosValue;
    SubTemp = abs((Info->AixsPos.X_Pos.PosValue - Temp) * 100);
    if ( (SubTemp % 10) == 9 )
    {
        SubTemp += (10 - (SubTemp % 10));
    }

    SubTemp /= 10;

    memset(Buff, 0, sizeof(Buff));
    if ( !ShowInfo.HomeX || !ShowInfo.StepperX_Enable )
    {
        if ( Count % 2 )
        {
            if ( !ShowInfo.HomeX )
            {
                strcpy(Buff, "X  ?");
            }
            else
            {
                strcpy(Buff, "X  ");
            }
        }
        else
        {
            sprintf(Buff, "X %d.%d", Temp, SubTemp);
        }
    }
    else
    {
        sprintf(Buff, "X %d.%d", Temp, SubTemp);
    }

    ShowPtr->WriteString(Buff, Lcd.Y, Lcd.X);

    //绘制Y坐标
    Lcd.X = 46;
    Lcd.Y = 38;
    Temp = Info->AixsPos.Y_Pos.PosValue;
    SubTemp = abs((Info->AixsPos.Y_Pos.PosValue - Temp) * 100);
    if ( (SubTemp % 10) == 9 )
    {
        SubTemp += (10 - (SubTemp % 10));
    }

    SubTemp /= 10;

    memset(Buff, 0, sizeof(Buff));
    if ( !ShowInfo.HomeY || !ShowInfo.StepperY_Enable )
    {
        if ( Count % 2 )
        {
            if ( !ShowInfo.HomeY )
            {
                strcpy(Buff, "Y  ?");
            }
            else
            {
                strcpy(Buff, "Y  ");
            }
        }
        else
        {
            sprintf(Buff, "Y %d.%d", Temp, SubTemp);
        }
    }
    else
    {
        sprintf(Buff, "Y %d.%d", Temp, SubTemp);
    }
    ShowPtr->WriteString(Buff, Lcd.Y, Lcd.X);

    //绘制Z坐标
    Lcd.X = 90;
    Lcd.Y = 38;
    Temp = Info->AixsPos.Z_Pos.PosValue;
    SubTemp = abs((Info->AixsPos.Z_Pos.PosValue - Temp) * 100);
    if ( (SubTemp % 10) == 9 )
    {
        SubTemp += (10 - (SubTemp % 10));
    }

    SubTemp /= 10;

    memset(Buff, 0, sizeof(Buff));
    if ( !ShowInfo.HomeZ || !ShowInfo.StepperZ_Enable )
    {
        if ( Count % 2 )
        {
            if ( !ShowInfo.HomeZ )
            {
                strcpy(Buff, "Z  ?");
            }
            else
            {
                strcpy(Buff, "Z  ");
            }
        }
        else
        {
            sprintf(Buff, "Z %d.%d", Temp, SubTemp);
        }
    }
    else
    {
        sprintf(Buff, "Z %d.%d", Temp, SubTemp);
    }
    ShowPtr->WriteString(Buff, Lcd.Y, Lcd.X);

    Lcd.X = 0;
    Lcd.Y = 62;
    memset(Buff, 0, sizeof(Buff));

    S8 NameBuf[20] = {0};
    if ( (Info->DevStatus.Reserve == STOP_PRINT) )
    {

        if ( ShowLineMsg )   //显示行信息
        {
            strncpy(Buff, LineMsg, sizeof(Buff) - 1);
        }
        else
        {
            if ( HasPrintFile )   //已打印过文件
            {
                Languages::FindMenuItemNameById(STOP_NAME_ID, NameBuf, sizeof(NameBuf));
                sprintf(Buff, "%s", NameBuf);
            }
            else
            {
                Languages::FindMenuItemNameById(READY_NAME_ID, NameBuf, sizeof(NameBuf));
                sprintf(Buff, "%s %s", DEVICE_MODE_NAME, NameBuf);
            }
        }

        ShowSpecialMsg = 0;  //停止后特殊信息也不显示
        //rt_kprintf("ShowLineMsg = %d, Buff = %s, LineMsg = %s\n", ShowLineMsg, Buff, LineMsg);
    }
    else if ( (Info->DevStatus.Reserve == PRINTING) || (Info->DevStatus.Reserve == RESUME_PRINT) )
    {
        if ( ShowSpecialMsg )
        {
            strncpy(Buff, SpecialMsg, sizeof(Buff) - 1);
        }
        else
        {
            snprintf(Buff, sizeof(Buff) - 1, "%s", Info->File.Name);
        }

        HasPrintFile = 1;
        ShowLineMsg = 0;
    }
    else if (Info->DevStatus.Reserve == PAUSE_PRINT)
    {
        Languages::FindMenuItemNameById(PAUSE_NAME_ID, NameBuf, sizeof(NameBuf));
        sprintf(Buff, "%s", NameBuf);
    }

    ShowPtr->WriteString(Buff, Lcd.Y, Lcd.X);

    Lcd.X = 43;
    Lcd.Y = 8;
    Lcd.Width = NOZZLE_MAP_WIDTH;
    Lcd.Height = NOZZLE_MAP_HEIGHT;
    ShowPtr->Ioctl(LCD_WRITE_MAP, (VOID *)&Lcd);
    if ( Info->DevStatus.NozzleStatus )
    {
        if ( Count % 2 )
            ShowPtr->Write((VOID *)NozzleHeatingMapData, sizeof(NozzleHeatingMapData));
        else
            ShowPtr->Write((VOID *)NozzleMapData, sizeof(NozzleMapData));
    }
    else
    {
        ShowPtr->Write((VOID *)NozzleMapData, sizeof(NozzleMapData));
    }

    if ( Info->SdStatus.Status )
    {
        Lcd.X = 40;
        Lcd.Y = 41;
        Lcd.Width = 10;
        Lcd.Height = 11;
        ShowPtr->Ioctl(LCD_WRITE_MAP, (VOID *)&Lcd);
        ShowPtr->Write((VOID *)SdCardMapData, sizeof(SdCardMapData));
//        if ( GetPrintStatus() == PRINT_READY || GetPrintStatus() == PRINT_STOP  )
       //     MainMenuArraySdCardStatusSetting(true);
    }
    else
    {
    //    if ( GetPrintStatus() == PRINT_READY || GetPrintStatus() == PRINT_STOP )
         //   MainMenuArraySdCardStatusSetting(false);
    }

    Lcd.X = 73;
    Lcd.Y = 8;
    Lcd.Width = BED_MAP_WIDTH;
    Lcd.Height = BED_MAP_HEIGHT;
    ShowPtr->Ioctl(LCD_WRITE_MAP, (VOID *)&Lcd);
    if ( Info->DevStatus.BedStatus )
    {
        if ( Count % 2 )
            ShowPtr->Write((VOID *)BedHeatingMapData, sizeof(BedHeatingMapData));
        else
            ShowPtr->Write((VOID *)BedMapData, sizeof(BedMapData));
    }
    else
    {
        ShowPtr->Write((VOID *)BedMapData, sizeof(BedMapData));
    }

    Lcd.X = 105;
    Lcd.Y = 0;
    Lcd.Width = FAN_MAP_WIDTH;
    Lcd.Height = FAN_MAP_HEIGHT;
    ShowPtr->Ioctl(LCD_WRITE_MAP, (VOID *)&Lcd);
    if ( ShowInfo.DevStatus.FanSpeed )
    {
        if ( Count % 2 )
        {
            ShowPtr->Write((VOID *)FanMapData, sizeof(FanMapData));
        }
        else
        {
            ShowPtr->Write((VOID *)FanMapData_1, sizeof(FanMapData_1));
        }
    }
    else
    {
        ShowPtr->Write((VOID *)FanMapData, sizeof(FanMapData));
    }

    Lcd.X = 105;
    Lcd.Y = 27;
    memset(Buff, 0, sizeof(Buff));
    sprintf(Buff, "%d%%", ShowInfo.DevStatus.FanSpeed);
    ShowPtr->WriteString(Buff, Lcd.Y, Lcd.X);

    Lcd.X = 3;
    Lcd.Y = 42;
    Lcd.Width = FR_MAP_WIDTH;
    Lcd.Height = FR_MAP_HEIGHT;
    ShowPtr->Ioctl(LCD_WRITE_MAP, (VOID *)&Lcd);
    ShowPtr->Write((VOID *)FrMapData, sizeof(FrMapData));


    //绘制进度条的外框
    Lcd.X = 52;
    Lcd.Y = 49;
    Lcd.Width = 50;
    Lcd.Height = 3;
    ShowPtr->Ioctl(LCD_WRITE_RECTANGLE, (VOID *)&Lcd);

    //绘制进度条的进度
    Lcd.X = 52;
    Lcd.Y = 50;
    Lcd.Width = (Lcd.Width * Info->Progress.Progress) / 100;
    ShowPtr->Ioctl(LCD_WRITE_HOR_LINE, &Lcd);

    ShowPtr->Ioctl(LCD_FLUSH, (VOID*)&CmdParam);

    return 1;
}


S32 ShowBootScreen(VOID *DataPtr)
{
    printf("******** Show BootScreen ********\n");
    if ( !ShowPtr )
        return 0;

    S32 CmdParam = 0;
    ShowPtr->Ioctl(LCD_CLEAN, (VOID*)&CmdParam);

    S32 Width = ShowPtr->GetPixelWidth();
    S32 Height = ShowPtr->GetPixelHeight();
    struct LcdCtrlParam_t Param;
    Param.X = 0;
    Param.Y = 0;
    Param.Height = Height;
    Param.Width = Width;
    ShowPtr->Ioctl(LCD_WRITE_MAP, (VOID *)&Param);
    ShowPtr->Write((VOID *)BootMapData, sizeof(BootMapData));  //直接写了

    //将内容刷到屏幕上
    ShowPtr->Ioctl(LCD_FLUSH, (VOID *)&CmdParam);

    return 1;
}





