/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     cx2470       the first version
 */

#include "GcodeM119Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "CrMotion/StepperApi.h"

#define CMD_M119  "M119"

#define STR_X_MIN                           "x_min"
#define STR_X_MAX                           "x_max"
#define STR_Y_MIN                           "y_min"
#define STR_Y_MAX                           "y_max"
#define STR_Z_MIN                           "z_min"
#define STR_Z_MAX                           "z_max"
#define STR_Z_PROBE                         "z_probe"
#define STR_FILAMENT_RUNOUT_SENSOR          "filament"

#define STR_ENDSTOP_HIT                     "TRIGGERED"
#define STR_ENDSTOP_OPEN                    "open"

static VOID PrintEsState(const BOOL IsHit,  const S8* Label)
{
  if (Label)
      printf(Label);

  printf(": ");
  printf("%s\n", IsHit ? STR_ENDSTOP_HIT : STR_ENDSTOP_OPEN);

  return ;
}

S32 GcodeM119Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
     if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
         return 0;

     S8 Cmd[CMD_BUFF_LEN] = {0};
     if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
         return 0;

     if ( strcmp(Cmd, CMD_M119) != 0 )
         return 0;

     return 1;
 }

S32 GcodeM119Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    printf("Reporting endstop status\n");
    PrintEsState(MotionModel->EndStopStatus(X), STR_X_MIN);
    PrintEsState(MotionModel->EndStopStatus(Y), STR_X_MIN);
    PrintEsState(MotionModel->EndStopStatus(Z), STR_X_MIN);

    //还缺少的有断料检测器的状态和探针的状态

    return 1;
}

S32 GcodeM119Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
