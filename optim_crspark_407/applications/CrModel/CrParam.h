#ifndef _CRPARAMMODEL_H
#define _CRPARAMMODEL_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrModel.h"
#include "CrMsgQueue/CommonQueue/CrMsgQueue.h"
#include "CrInc/CrMsgType.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"

class CrParam : public CrModel
{
public:
    CrParam();    //直接进行加载
    ~CrParam();

    VOID Exec();
    struct ViewMotionMsg_t MotionParmGet(){return MotionParam; }
    struct ViewFanSpeedParamMsg_t FanSpeedParmGet(){return FansSpeedParam;}
    struct ParamMaterialTemperSet_t TargetTemperParmGet(){return TargetTemperSet;}
protected:
    S32 HandleParamOptMsg(S8 *Buff, S32 Len);
    S32 HandleTargetTemperSetMsg(S8 *Buff, S32 Len);
    S32 HandleMotionMsg(S8 *Buff, S32 Len);
	S32 HandleFansMsg(S8 *Buff, S32 Len);

    VOID TargetTemperSetMsgOpt();
    VOID ParamOptMsgOpt();
    VOID MotionMsgOpt();
    VOID FanOptMsgOpt();

private:
    CrMsgQueue_t RecvParamOptQueue;   //使用普通队列
    CrMsgQueue_t RecvTemperSetQueue;   //使用普通队列
    CrMsgQueue_t RecvMotionQueue;   //使用普通队列
    CrMsgQueue_t RecvFansParamQueue;

    CrMsg_t SendParamOptQueue;   //使用共享队列
    CrMsg_t SendTemperSetQueue;   //使用共享队列
    CrMsg_t SendMotionQueue;   //使用共享队列
    CrMsg_t SendFansParamQueue;

    struct ViewFanSpeedParamMsg_t FansSpeedParam;
    struct ParamMaterialTemperSet_t TargetTemperSet;
//    struct ParamMaterialTemperSet_t DefaultTargetTemperSet;
    struct ViewMotionMsg_t MotionParam;
//    struct ViewMotionMsg_t DefaultMotionParam;
};

extern CrParam* ParamModel;

#ifdef TEST_PARAM_MODEL
VOID TestParamModel();
#endif

#endif
