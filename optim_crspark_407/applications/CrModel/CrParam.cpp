#include "CrParam.h"
#include "CrInc/CrConfig.h"
#include "CrConfig/CrParamConfig.h"
#include "CrMsgQueue/QueueCommonFuns.h"

CrParam* ParamModel = NULL;

CrParam::CrParam()
    : CrModel()
{
    CrConfigLoad();

    RecvTemperSetQueue = InValidMsgQueue();
    RecvParamOptQueue = InValidMsgQueue();
    RecvMotionQueue = InValidMsgQueue();
    SendParamOptQueue = NULL;
    SendTemperSetQueue = NULL;
    SendMotionQueue = NULL;

    CrMsgQueueAttr_t Attr;
    Attr.mq_flags = O_NONBLOCK;
    Attr.mq_maxmsg = 2;
    Attr.mq_msgsize = sizeof(struct ViewParamOptActionMsg_t);
    RecvParamOptQueue = QueueOpen(PARAM_OPT_SET_QUEUE, O_CREAT | O_RDWR, 0x0777, &Attr);   //使用普通队列
    Attr.mq_msgsize = sizeof(struct ViewTargetTemperSetMsg_t);
    RecvTemperSetQueue = QueueOpen(PARAM_TEMPER_SET_QUEUE, O_CREAT | O_RDWR, 0x0777, &Attr);   //使用普通队列
    Attr.mq_msgsize = sizeof(struct ViewMotionMsg_t);
    RecvMotionQueue = QueueOpen(PARAM_MOTION_SET_QUEUE, O_CREAT | O_RDWR, 0x0777, &Attr);   //使用普通队列
    Attr.mq_msgsize = sizeof(struct ViewFanSpeedParamMsg_t);
    RecvFansParamQueue = QueueOpen((S8 *)PARAM_FANS_REQUEST_QUEUE, O_CREAT | O_RDWR, 0x0777, &Attr);   //使用普通队列

    Attr.mq_maxmsg = 2;
    Attr.mq_msgsize = sizeof(struct ViewParamOptActionMsg_t);
    SendParamOptQueue = SharedQueueOpen(PARAM_OPT_REPORT_QUEUE, O_CREAT | O_RDWR, 0x0777, &Attr);   //使用共享队列
    Attr.mq_msgsize = sizeof(struct ParamMaterialTemperSet_t);
    SendTemperSetQueue = SharedQueueOpen(PARAM_TEMPER_REPORT_QUEUE, O_CREAT | O_RDWR, 0x0777, &Attr);   //使用共享队列
    Attr.mq_msgsize = sizeof(struct ViewMotionMsg_t);
    SendMotionQueue = SharedQueueOpen(PARAM_MOTION_REPORT_QUEUE, O_CREAT | O_RDWR, 0x0777, &Attr);   //使用共享队列
    Attr.mq_msgsize = sizeof(struct ViewFanSpeedParamMsg_t);
    SendFansParamQueue = SharedQueueOpen((S8 *)PARAM_FANS_REPORT_QUEUE, O_CREAT | O_RDWR, 0x0777, &Attr);

    //参数赋值
    FansSpeedParam.Fan = *GetFansParam();
    TargetTemperSet = *GetTemperSet();
    MotionParam.MotionParam = *GetMotionParam();
    MotionParam.Diameter = *GetFilementConfig();
    MotionParam.FilamentPatternAction = GetFilamentVolumePattern();
}

CrParam::~CrParam()
{
    CrConfigSave();   //暂时的
}

VOID CrParam::Exec()
{
    TargetTemperSetMsgOpt();
    ParamOptMsgOpt();
    MotionMsgOpt();
    FanOptMsgOpt();

    return ;
}

VOID CrParam::FanOptMsgOpt()
{
    if ( !IsValidMsgQueue(RecvFansParamQueue) || !SendFansParamQueue )
        return ;

    S8 Data[sizeof(struct ViewFanSpeedParamMsg_t)] = {0};
    S32 Prime = 0;
    if ( RecvMsg(RecvFansParamQueue, Data, sizeof(Data), &Prime) <= 0 )
        return ;

    HandleFansMsg(Data, sizeof(Data));
    CrMsgSend(SendFansParamQueue, (S8 *)&FansSpeedParam, sizeof(FansSpeedParam), 1);

    return ;
}

VOID CrParam::TargetTemperSetMsgOpt()
{
    if ( !IsValidMsgQueue(RecvTemperSetQueue) || !SendTemperSetQueue )
        return;

    S8 Data[sizeof(struct ViewTargetTemperSetMsg_t)] = {0};
    S32 Prime = 0;
    if ( RecvMsg(RecvTemperSetQueue, Data, sizeof(Data), &Prime) <= 0 )
        return;

    HandleTargetTemperSetMsg(Data, sizeof(Data));
    CrMsgSend(SendTemperSetQueue, (S8 *)&TargetTemperSet, sizeof(TargetTemperSet), 1);
}

VOID CrParam::ParamOptMsgOpt()
{
    if ( IsValidMsgQueue(RecvParamOptQueue) )
    {
        S8 Data[sizeof(struct ViewParamOptActionMsg_t)] = {0};
        S32 Prime = 0;
        if ( RecvMsg(RecvParamOptQueue, Data, sizeof(Data), &Prime) > 0 )
        {
            HandleParamOptMsg(Data, sizeof(Data));
        }
    }
}

VOID CrParam::MotionMsgOpt()
{
    if ( IsValidMsgQueue(RecvMotionQueue) )
    {
        S8 Data[sizeof(struct ViewMotionMsg_t)] = {0};
        S32 Prime = 0;
        if ( RecvMsg(RecvMotionQueue, Data, sizeof(Data), &Prime) > 0 )
        {
            HandleMotionMsg(Data, sizeof(Data));
            if ( SendMotionQueue )
            {
                CrMsgSend(SendMotionQueue, (S8 *)&MotionParam, sizeof(MotionParam), 1);
            }
        }
    }
}

S32 CrParam::HandleParamOptMsg(S8 *Buff, S32 Len)
{
    if ( !Buff || (Len < (S32)sizeof(struct ViewParamOptActionMsg_t)) )
        return 0;

    struct ViewParamOptActionMsg_t *Msg = (struct ViewParamOptActionMsg_t *)Buff;
    switch ( Msg->Action )
    {
        case LOAD_ALL_FORM_FILE:
        {
            CrConfigLoad();

            TargetTemperSet = *GetTemperSet();
            FansSpeedParam.Fan = *GetFansParam();
            MotionParam.MotionParam = *GetMotionParam();
            MotionParam.FilamentPatternAction = GetFilamentVolumePattern();
            MotionParam.Diameter = *GetFilementConfig();

            //Fixme: 将参数设置到系统中 主要是运动！

            CrMsgSend(SendTemperSetQueue, (S8 *)&TargetTemperSet, sizeof(TargetTemperSet), 1);
            CrMsgSend(SendFansParamQueue, (S8 *)&FansSpeedParam, sizeof(FansSpeedParam), 1);
            CrMsgSend(SendMotionQueue, (S8 *)&MotionParam, sizeof(MotionParam), 1);
            break;
        }
        case SAVE_ALL_TO_FILE:
        {
            CrConfigSave();
            break;
        }
        case LOAD_ALL_DEFAULT_PARAM:
        {
            CrConfigRecover();
            TargetTemperSet = *GetTemperSet();
            FansSpeedParam.Fan = *GetFansParam();
            MotionParam.MotionParam = *GetMotionParam();
            MotionParam.FilamentPatternAction = GetFilamentVolumePattern();
            MotionParam.Diameter = *GetFilementConfig();

            //Fixme: 将参数设置到系统中 主要是运动！

            CrMsgSend(SendTemperSetQueue, (S8 *)&TargetTemperSet, sizeof(TargetTemperSet), 1);
            CrMsgSend(SendFansParamQueue, (S8 *)&FansSpeedParam, sizeof(FansSpeedParam), 1);
            CrMsgSend(SendMotionQueue, (S8 *)&MotionParam, sizeof(MotionParam), 1);
            break;
        }
        case LOAD_PLA_PARAM:
        {
            CrConfigPlaLoad();

            FansSpeedParam.Fan = *GetFansParam();
            TargetTemperSet = *GetTemperSet();

            CrMsgSend(SendTemperSetQueue, (S8 *)&TargetTemperSet, sizeof(TargetTemperSet), 1);
            CrMsgSend(SendFansParamQueue, (S8 *)&FansSpeedParam, sizeof(FansSpeedParam), 1);

            break;
        }
        case SAVE_PLA_PARAM:
        {
            CrConfigPlaSave();
            break;
        }
        case LOAD_PLA_DEFAULT_PARAM:
        {
            CrConfigPlaRecover();
            TargetTemperSet = *GetTemperSet();
            FansSpeedParam.Fan = *GetFansParam();

            CrMsgSend(SendTemperSetQueue, (S8 *)&TargetTemperSet, sizeof(TargetTemperSet), 1);
            CrMsgSend(SendFansParamQueue, (S8 *)&FansSpeedParam, sizeof(FansSpeedParam), 1);

            break;
        }
        case LOAD_ABS_PARAM:
        {
            CrConfigAbsLoad();
            TargetTemperSet = *GetTemperSet();
            FansSpeedParam.Fan = *GetFansParam();

            CrMsgSend(SendTemperSetQueue, (S8 *)&TargetTemperSet, sizeof(TargetTemperSet), 1);
            CrMsgSend(SendFansParamQueue, (S8 *)&FansSpeedParam, sizeof(FansSpeedParam), 1);

            break;
        }
        case SAVE_ABS_PARAM:
        {
            CrConfigAbsSave();
            break;
        }
        case LOAD_ABS_DEFAULT_PARAM:
        {
            CrConfigAbsRecover();
            TargetTemperSet = *GetTemperSet();
            FansSpeedParam.Fan = *GetFansParam();
            CrMsgSend(SendTemperSetQueue, (S8 *)&TargetTemperSet, sizeof(TargetTemperSet), 1);
            CrMsgSend(SendFansParamQueue, (S8 *)&FansSpeedParam, sizeof(FansSpeedParam), 1);
            break;
        }
        default:
        {
            break;
        }
    }

    return 1;
}

S32 CrParam::HandleTargetTemperSetMsg(S8 *Buff, S32 Len)
{
    if ( !Buff || (Len < (S32)sizeof(struct ViewTargetTemperSetMsg_t)) )
        return 0;

    struct ViewTargetTemperSetMsg_t *Msg = (struct ViewTargetTemperSetMsg_t *)Buff;
    if ( Msg->Action  == SET_TEMPER_MSG )
    {
        TargetTemperSet = Msg->Set;
        SetTemperSet(&TargetTemperSet);
    }

    return 1;
}

S32 CrParam::HandleMotionMsg(S8 *Buff, S32 Len)
{
    if ( !Buff || (Len < (S32)sizeof(struct ViewMotionMsg_t)) )
        return 0;

    struct ViewMotionMsg_t *Msg = (struct ViewMotionMsg_t *)Buff;
    if ( Msg->Action == SET_MOTION_MSG )
    {
        MotionParam = *Msg;
        SetMotionParam(&MotionParam.MotionParam);
        SetFilamentVolumePattern(MotionParam.FilamentPatternAction);
        SetFilementConfig(MotionParam.Diameter);
    }

    return 1;
}

S32 CrParam::HandleFansMsg(S8 *Buff, S32 Len)
{
    if ( !Buff || Len < (S32)sizeof(struct ViewFanSpeedParamMsg_t) )
        return 0;

    struct ViewFanSpeedParamMsg_t *Msg = (struct ViewFanSpeedParamMsg_t *)Buff;
    if ( Msg->Action == FAN_SPEED_SET )
    {
        FansSpeedParam.Fan = Msg->Fan;
        SetFansParam(&FansSpeedParam.Fan);
    }

    return 1;
}

