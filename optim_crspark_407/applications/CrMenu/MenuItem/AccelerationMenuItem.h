/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-18     cx2470       the first version
 */
#ifndef __ACCELERATIONMENUITEM_H__
#define __ACCELERATIONMENUITEM_H__

#include "CrInc/CrType.h"

//enum AcceleOkAction_e
//{
//    ACCEL_OKACTION,
//    ARETRACT_OKACTION,
//    ATRAVEL_OKACTION,
//    AMAX_X_OKACTION,
//    AMAX_Y_OKACTION,
//    AMAX_Z_OKACTION,
//    AMAX_E_OKACTION
//};

typedef struct
{
    U16 Accel;
    U16 ARetract;
    U16 ATravel;
    U16 AmaxX;
    U16 AmaxY;
    U16 AmaxZ;
    U16 AmaxE;
}Accele_t;

#ifdef __cplusplus
extern "C" {
#endif

VOID AccelQueueInit();
//VOID *AccelerationOKAction(VOID *ArgOut, VOID *ArgIn, AcceleOkAction_e Flag);
/*=========================================================
*                  Accel Directory function
*=========================================================*/
S32   AccelespecialDisplay(VOID *Arg);
VOID *AcceleDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleOKAction(VOID *ArgOut, VOID *ArgIn);
S32   AcceleMenuShow(VOID *Arg);
/*=========================================================
*                  ARetract Directory function
*=========================================================*/
S32   AcceleARetractspecialDisplay(VOID *Arg);
VOID *AcceleARetractDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleARetractUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleARetractOKAction(VOID *ArgOut, VOID *ArgIn);
S32   AcceleARetractMenuShow(VOID *Arg);
/*=========================================================
*                  ATravel Directory function
*=========================================================*/
S32   AcceleATravelspecialDisplay(VOID *Arg);
VOID *AcceleATravelDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleATravelUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleATravelOKAction(VOID *ArgOut, VOID *ArgIn);
S32   AcceleATravelMenuShow(VOID *Arg);
/*=========================================================
*                  AmaxX Directory function
*=========================================================*/
S32   AcceleAmaxXspecialDisplay(VOID *Arg);
VOID *AcceleAmaxXDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleAmaxXUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleAmaxXOKAction(VOID *ArgOut, VOID *ArgIn);
S32   AcceleAmaxXMenuShow(VOID *Arg);
/*=========================================================
*                  AmaxY Directory function
*=========================================================*/
S32   AcceleAmaxYspecialDisplay(VOID *Arg);
VOID *AcceleAmaxYDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleAmaxYUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleAmaxYOKAction(VOID *ArgOut, VOID *ArgIn);
S32   AcceleAmaxYMenuShow(VOID *Arg);
/*=========================================================
*                  AmaxZ Directory function
*=========================================================*/
S32   AcceleAmaxZspecialDisplay(VOID *Arg);
VOID *AcceleAmaxZDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleAmaxZUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleAmaxZOKAction(VOID *ArgOut, VOID *ArgIn);
S32   AcceleAmaxZMenuShow(VOID *Arg);
/*=========================================================
*                  AmaxE Directory function
*=========================================================*/
S32   AcceleAmaxEspecialDisplay(VOID *Arg);
VOID *AcceleAmaxEDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleAmaxEUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *AcceleAmaxEOKAction(VOID *ArgOut, VOID *ArgIn);
S32   AcceleAmaxEMenuShow(VOID *Arg);

Accele_t *GetAcceleConfigInfo();

VOID AcceleConfigRecover();
VOID ClearAccleShowTime(VOID);

#ifdef __cplusplus
}
#endif

#endif /*__ACCELERATIONMENUITEM_H__*/
