#include "GcodeG21Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"

#define CMD_G21            "G21"

S32 GcodeG21Parse(const S8 *CmdBuff, VOID *Arg)
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

    // G21
    if ( strcmp(Cmd, CMD_G21) != 0 )
        return 0;

    S32 Offset = strlen(CMD_G21) + sizeof(S8); // 加1是越过分隔符

    // 确认是否无操作参数
    if (Len > Offset)
    {
        printf("Error:G21 has no parameter \n");
        return 0;
    }

    return 1;
}

S32 GcodeG21Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if (!MotionModel)
    {
        printf("Error:G21MotionModel not found \n");
        return 0;
    }

    //将单位设置为毫米。
    MotionModel->SetMotionUnitType(Millimeter);

    if (!MotionModel->IsMotionUnitTypeInch())
    {
        printf("Set Millimeter unit done \n");
    }

    return 1;
}

S32 GcodeG21Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


