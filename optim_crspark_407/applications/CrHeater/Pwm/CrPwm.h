#ifndef _CRPWM_H
#define _CRPWM_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"
#include "rtdef.h"
#include "Common/Include/CrTimer.h"

typedef VOID(*FuncType)(VOID *); 

#if 0
struct PwmControllerAttr_t
{
    S32 Freq;    //频率
    S32 Duty;    //占空
    S32 Times;   //if Times=0, the pwm will run forever
    S32 DefaultLevel;    //默认高低电平
};
#endif

//执行器
class CrPwmController
{
public:
    CrPwmController(S32 Fd ,FuncType Func);//执行周期是由外部控制，不是由执行器控制
    virtual ~CrPwmController();
    S32 CrPwmControllerSetAttr(struct PwmControllerAttr_t &Attr);
    const struct PwmControllerAttr_t &CrPwmControllerGetAttr();
    S32 CrPwmControllerGetPwmFd();
    VOID CrPwmControllerSetTimer(U32 Freq);  //

    FuncType ExecFunc;
    
protected:
    struct PwmControllerAttr_t  Attr;
    S32    PwmFd;

    CrTimer_t timer;
    static VOID TimeOut(VOID *PData);
    //是否加执行函数指针
    
    //static void Exec(VOID *PData);
};


#endif
