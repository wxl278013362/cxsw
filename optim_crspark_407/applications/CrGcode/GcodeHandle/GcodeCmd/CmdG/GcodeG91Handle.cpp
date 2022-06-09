#include "GcodeG91Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrModel/CrMotion.h"

#define CMD_G91            "G91"

S32 GcodeG91Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_G) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_G91) != 0 )  //传进来的命令不是G0命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    return 1;
}

S32 GcodeG91Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    //设置模式为相对模式
    if ( MotionModel )
    {
        MotionModel->SetRelativeMotion();
    }
    return 1;
}

S32 GcodeG91Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


