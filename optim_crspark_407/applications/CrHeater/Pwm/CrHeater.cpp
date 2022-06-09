#include "CrHeater.h"

CrHeater::CrHeater(S32 PwmFd, FuncType Func)
    : Pwm(PwmFd, Func)
{

}

CrHeater::~CrHeater()
{
    
}

VOID CrHeater::CrHeaterSetAttr(struct PwmControllerAttr_t &Attr)
{
    Pwm.CrPwmControllerSetAttr(Attr);
}

const struct PwmControllerAttr_t &CrHeater::CrHeaterGetAttr()
{
    return Pwm.CrPwmControllerGetAttr();
}

