
/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     GcodeG20Handle.cpp
  Author :        FuMin
  Version:        1.0
  Description:    G20 GCODE Command
  Other:
  Modify History:
          <author>        <time>      <version>         <desc>
          FuMin         2021-01-22     V1.0.0.1
****************************************************************************/

#include "GcodeG20Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"

#define CMD_G20  "G20"

static CrMotion *G20MotionModel = NULL;

VOID GcodeG20Init(VOID *Model)
{
    G20MotionModel = (CrMotion *)Model;
}

/* 解析G20指令
   G20
      无操作参数
 */
S32 GcodeG20Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    //printf("[%s] %s \n", __FUNCTION__, CmdBuff);

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_G) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // G20
    if ( strcmp(Cmd, CMD_G20) != 0 )
        return 0;

    S32 Offset = strlen(CMD_G20) + sizeof(S8); // 加1是越过分隔符

    // 确认是否无操作参数
    if (Len > Offset)
    {
        printf("Error:G20 has no parameter \n");
        return 0;
    }

    return 1;
 }

S32 GcodeG20Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if (!G20MotionModel)
    {
        printf("Error:G20MotionModel not found \n");
        return 0;
    }

    //将单位设置为英寸。
    G20MotionModel->SetMotionUnitType(Inch);

    if (G20MotionModel->IsMotionUnitTypeInch())
    {
        printf("Set Inch unit done \n");
    }

    return 1;
}

S32 GcodeG20Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


