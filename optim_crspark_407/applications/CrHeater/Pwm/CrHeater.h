#ifndef _CRHEATER_H
#define _CRHEATER_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrPwm.h"

class CrHeater
{
public:
    CrHeater(S32 PwmFd, FuncType Func);
    virtual ~CrHeater();
    VOID CrHeaterSetAttr(struct PwmControllerAttr_t &Attr);
    const struct PwmControllerAttr_t &CrHeaterGetAttr();

private:
    CrPwmController Pwm;
};

#endif
