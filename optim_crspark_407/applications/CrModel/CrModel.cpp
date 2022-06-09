#include "CrModel.h"
#include "Common/Include/CrTime.h"

CrModel::CrModel()
{

}

CrModel::~CrModel()
{

}

S32 CrModel::SendMsg(CrMsgQueue_t &MsgQ, S8 *Buff, S32 Len, S32 Pri)
{
    if ( !IsValidMsgQueue(MsgQ) || !Buff || (Len <= 0) )
        return 0;

    CrTimeSpec_t Time;
    CrGetCurrentSystemTime(&Time);
    //Time.tv_sec += 1;
    Time.tv_nsec += 1000;
    S32 Res = CrMsgQueueTimedSend(MsgQ, Buff, Len, Pri, &Time);
//    S32 Res = CrMsgQueueSend(MsgQ, Buff, Len, Pri);

    return Res;
}

S32 CrModel::RecvMsg(CrMsgQueue_t &MsgQ, S8 *Buff, S32 Len, S32* Pri)
{
    if ( !IsValidMsgQueue(MsgQ) || !Buff || (Len <= 0) || !Pri)
        return 0;

    CrTimeSpec_t Time;
    CrGetCurrentSystemTime(&Time);
    Time.tv_nsec += 1000;
    S32 Res = CrMsgQueueTimedRecv(MsgQ, Buff, Len, (U32 *)Pri, &Time);

    return Res;
}
