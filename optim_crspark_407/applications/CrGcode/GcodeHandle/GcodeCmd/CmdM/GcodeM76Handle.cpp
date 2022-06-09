#include "GcodeM76Handle.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"



#define CMD_M76 "M76"

S32 GcodeM76Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd,CMD_M76) != 0 )
    {
        return 0;
    }

    return 1;
}

S32 GcodeM76Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }
    //原生M76的处理流程:暂停打印计时器的计时
    //1、判断目前的模式是否处在RUNNING模式(共有STOPPED、RUNNING、PAUSED三种模式)
    //2、不处于RUNNING模式下则返回false
    //3、处于RUNNING模式下，暂停打印时间、将打印状态置为PAUSED、同时记录下当前的系统时间作为停止时间的时间戳，返回true
    //因为目前Spark暂时无打印计时器，所以暂不做实现

    return 1;
}

S32 GcodeM76Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
