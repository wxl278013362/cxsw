#ifndef _CRMODEL_H
#define _CRMODEL_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrMsgQueue/CommonQueue/CrMsgQueue.h"

class CrModel
{
public:
    CrModel();
    virtual ~CrModel();

    virtual VOID Exec() = 0;     //将所有消息队列接收要接收的内容以及他们的处理放到次函数中

protected:
    S32 SendMsg(CrMsgQueue_t &Queue, S8 *Buff, S32 Len, S32 Pri);
    S32 RecvMsg(CrMsgQueue_t &Queue, S8 *Buff, S32 Len, S32* Pri);
};

#endif
