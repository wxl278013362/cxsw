/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-19     cx2470       the first version
 */
#ifndef _STEPSMENUITEM_H_
#define _STEPSMENUITEM_H_

#include "CrInc/CrType.h"

//enum StepsOKAction_e
//{
//    X_STEPS_OK_ACTION,
//    Y_STEPS_OK_ACTION,
//    Z_STEPS_OK_ACTION,
//    E_STEPS_OK_ACTION,
//};

typedef struct
{
    float Xsteps;
    float Ysteps;
    float Zsteps;
    float Esteps;
}Steps_t;

#ifdef __cplusplus
extern "C"
{
#endif

VOID StepQueueInit();

//VOID *StepsOKAction(VOID *ArgOut, VOID *ArgIn, StepsOKAction_e Flag);
/*=========================================================
 *                  X Steps Directory function
 *=========================================================*/
S32   XstepsSpecialDisplay(VOID *Arg);
VOID *XstepsDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *XstepsUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *XstepsOKAction(VOID *ArgOut, VOID *ArgIn);
S32   XstepsMenuShow(VOID *Arg);
/*=========================================================
*                   Y Steps  Directory function
*=========================================================*/
S32   YstepsSpecialDisplay(VOID *Arg);
VOID *YstepsDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *YstepsUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *YstepsOKAction(VOID *ArgOut, VOID *ArgIn);
S32   YstepsMenuShow(VOID *Arg);
/*=========================================================
*                   Z Steps Directory function
*=========================================================*/
S32   ZstepsSpecialDisplay(VOID *Arg);
VOID *ZstepsDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *ZstepsUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *ZstepsOKAction(VOID *ArgOut, VOID *ArgIn);
S32   ZstepsMenuShow(VOID *Arg);
/*=========================================================
*                   E Steps  Directory function
*=========================================================*/
S32   EstepsSpecialDisplay(VOID *Arg);
VOID *EstepsDownAction(VOID *ArgOut, VOID *ArgIn);
VOID *EstepsUpAction(VOID *ArgOut, VOID *ArgIn);
VOID *EstepsOKAction(VOID *ArgOut, VOID *ArgIn);
S32   EstepsMenuShow(VOID *Arg);

Steps_t *GetStepsConfigInfo();
VOID StepsConfigRecover();
VOID ClearStepsShowTime(VOID);

#ifdef __cplusplus
}
#endif


#endif /* _STEPSMENUITEM_H_ */
