/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-19     cx2470       the first version
 */
#ifndef _JERKMENUITEM_H_
#define _JERKMENUITEM_H_

#include "CrInc/CrType.h"

enum JerkOKAction_e
{
    VX_JERK_OK_ACTION,
    VY_JERK_OK_ACTION,
    VZ_JERK_OK_ACTION,
    VE_JERK_OK_ACTION,
};

typedef struct
{
    U16 VxJerk;
    U16 VyJerk;
    float VzJerk;
    float VeJerk;
}Jerk_t;

#ifdef __cplusplus
extern "C"
{
#endif

VOID JerkQueueInit();

//VOID *JerkOKAction(VOID *ArgOut, VOID *ArgIn, JerkOKAction_e Flag);
/*=========================================================
 *                  VX Jerk Directory function
 *=========================================================*/
S32   VxJerkspecialDisplay(VOID *Arg);
VOID *VxJerkDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *VxJerkUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *VxJerkOKAction(VOID *ArgOut, VOID *ArgIn);
S32   VxJerkMenuShow(VOID *Arg);
/*=========================================================
*                   VY Jerk Directory function
*=========================================================*/
S32   VyJerkspecialDisplay(VOID *Arg);
VOID *VyJerkDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *VyJerkUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *VyJerkOKAction(VOID *ArgOut, VOID *ArgIn);
S32   VyJerkMenuShow(VOID *Arg);
/*=========================================================
*                   VZ JerkDirectory function
*=========================================================*/
S32   VzJerkspecialDisplay(VOID *Arg);
VOID *VzJerkDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *VzJerkUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *VzJerkOKAction(VOID *ArgOut, VOID *ArgIn);
S32   VzJerkMenuShow(VOID *Arg);
/*=========================================================
*                   VE Jerk Directory function
*=========================================================*/
S32   VeJerkspecialDisplay(VOID *Arg);
VOID *VeJerkDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *VeJerkUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *VeJerkOKAction(VOID *ArgOut, VOID *ArgIn);
S32   VeJerkMenuShow(VOID *Arg);

Jerk_t *GetJerkConfigInfo();
VOID JerkConfigRecover();

VOID ClearJerkShowTime(VOID);

#ifdef __cplusplus
}
#endif
#endif /* _JERKMENUITEM_H_ */
