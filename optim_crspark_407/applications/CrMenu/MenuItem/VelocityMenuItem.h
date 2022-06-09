/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-18     cx2470       the first version
 */
#ifndef _VELOCITYMENUITEM_H_
#define _VELOCITYMENUITEM_H_

#include "CrInc/CrType.h"

typedef struct
{
    U16 VmaxX;
    U16 VmaxY;
    U16 VmaxZ;
    U16 VmaxE;
    U16 Vmin;
    U16 VTravMin;
}Velocity_t;

#ifdef __cplusplus
extern "C"
{
#endif

VOID VelocityQueueInit();

/*=========================================================
*                  Vmax X Directory function
*=========================================================*/
S32   VelocityVmaxXspecialDisplay(VOID *Arg);
VOID *VelocityVmaxXDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *VelocityVmaxXUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *VelocityVmaxXOKAction(VOID *ArgOut, VOID *ArgIn);
S32   VelocityVmaxXMenuShow(VOID *Arg);
/*=========================================================
*                  Vmax Y Directory function
*=========================================================*/
S32   VelocityVmaxYspecialDisplay(VOID *Arg);
VOID *VelocityVmaxYDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *VelocityVmaxYUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *VelocityVmaxYOKAction(VOID *ArgOut, VOID *ArgIn);
S32   VelocityVmaxYMenuShow(VOID *Arg);
/*=========================================================
*                  Vmax Z Directory function
*=========================================================*/
S32   VelocityVmaxZspecialDisplay(VOID *Arg);
VOID *VelocityVmaxZDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *VelocityVmaxZUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *VelocityVmaxZOKAction(VOID *ArgOut, VOID *ArgIn);
S32   VelocityVmaxZMenuShow(VOID *Arg);
/*=========================================================
*                  Vmax E Directory function
*=========================================================*/
S32   VelocityVmaxEspecialDisplay(VOID *Arg);
VOID *VelocityVmaxEDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *VelocityVmaxEUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *VelocityVmaxEOKAction(VOID *ArgOut, VOID *ArgIn);
S32   VelocityVmaxEMenuShow(VOID *Arg);
/*=========================================================
*                  Vmin Directory function
*=========================================================*/
S32   VelocityVminspecialDisplay(VOID *Arg);
VOID *VelocityVminDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *VelocityVminUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *VelocityVminOKAction(VOID *ArgOut, VOID *ArgIn);
S32   VelocityVminMenuShow(VOID *Arg);
/*=========================================================
*                  VTrav Min Directory function
*=========================================================*/
S32   VelocityVTravMinspecialDisplay(VOID *Arg);
VOID *VelocityVTravMinDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *VelocityVTravMinUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *VelocityVTravMinOKAction(VOID *ArgOut, VOID *ArgIn);
S32   VelocityVTravMinMenuShow(VOID *Arg);

//Velocity_t* GetVelocityConfigInfo();
VOID VelocityConfigRecover();
VOID ClearVelocityShowTime(VOID);

#ifdef __cplusplus
}
#endif


#endif /* _VELOCITYMENUITEM_H_ */
