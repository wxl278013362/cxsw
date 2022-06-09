#include "GcodeM77Handle.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"

#define CMD_M77 "M77"

S32 GcodeM77Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || CmdBuff[0] != GCODE_CMD_M ) //Gcode命令长度需要大于1且是M命令
    {
        return 0;
    }

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_M77) != 0 )
    {
        return 0;
    }

    return 1;
}
S32 GcodeM77Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }
    //M77原生处理流程
    //1、判断当前所处模式是否是RUNNING或者是PAUSED
    //2、两者都不是则返回false
    //3、处于两者任意一种模式下，先停止页面的打印时间、将打印状态设置为STOPPED、记录下当前的时间作为停止时间戳，然后返回true
    //因为目前Spark暂时无打印计时器，所以暂不做实现
    return 1;
}
S32 GcodeM77Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
