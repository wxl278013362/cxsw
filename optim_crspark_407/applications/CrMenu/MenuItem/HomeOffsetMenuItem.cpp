/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-04     cx2470       the first version
 */
#include "HomeOffsetMenuItem.h"
#include "CrInc/CrMsgStruct.h"
#include "../Show/ShowAction.h"
#include <stdio.h>
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

#define BASE_HOME_POS 0
#define WITHIN(N,L,H)       ((N) >= (L) && (N) <= (H))

S8 AxesNeedHoming()
{
    char AxesHomed = 0x07; /*0111 XYZ三轴 */

//    _READ_BIT(HomeIsSetting(), X ) ? (AxesHomed = CBI(AxesHomed, X)) : AxesHomed;
//    _READ_BIT(HomeIsSetting(), Y ) ? (AxesHomed = CBI(AxesHomed, Y)) : AxesHomed;
//    _READ_BIT(HomeIsSetting(), Z ) ? (AxesHomed = CBI(AxesHomed, Z)) : AxesHomed;

    return AxesHomed;
}

BOOL AxisUnhomedError()
{
    BOOL Ret = false;
    S8 LcdMsgBuf[20] = {0};
    S8 AxesNeedHome = AxesNeedHoming();

    if ( 0x00 != AxesNeedHome )
    {
//        sprintf(LcdMsgBuf , "Home Axes %s%s%s first!", _READ_BIT(AxesNeedHome, X ) ? "X":"",
//                                                       _READ_BIT(AxesNeedHome, Y ) ? "Y":"",
//                                                       _READ_BIT(AxesNeedHome, Z ) ? "Z":"");
//        printf("%s\n", LcdMsgBuf);
        Ret = true;
    }

    return Ret;
}

VOID *HomeOffsetMenuOkAction(VOID *ArgOut, VOID *ArgIn)
{
    if ( !ArgOut )
        return 0;

    CrBeepAppShortBeeps(); /* 短鸣 */

    S32 *Back = (S32 *)ArgOut;
    *Back = BACK_ROOT;

    if ( AxisUnhomedError() )
        return ArgOut;

    float Xpos = -1 ,Ypos = -1 ,Zpos = -1 ;
//    GetPositions(&Xpos, &Ypos, &Zpos);

    float XYZpos[3] = {Xpos, Ypos, Zpos};
    float Diff[3] = {-1};

    for ( U8 i = 0; i < 3; i++ )
    {
        Diff[i] = BASE_HOME_POS - XYZpos[i];
        if (!WITHIN(Diff[i], -20, 20))
        {
            printf("Err: Too far!");
            return ArgOut;
        }
    }

//    HomeSet();
    printf("Offsets applied");
    return ArgOut;
}
