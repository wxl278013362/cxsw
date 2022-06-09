#include "GcodeG90Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrModel/CrMotion.h"

#define CMD_G90            "G90"

extern U8 IsRelativeMode;

S32 GcodeG90Parse(const S8 *CmdBuff, VOID *Arg)
{
    /*G90 没有操作 */

    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_G) )   //Gcode命令的长度大于1,且是G
        return 0;

    //
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_G90) != 0 )  //传进来的命令不是G0命令
    {
        //printf("current cmd is not G0 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    S32 Offset = strlen(CMD_G90) + sizeof(S8); //加1是越过分隔符
//    struct ArgG1AndG0_t *CmdArg = (struct ArgG1AndG0_t *)Arg;

    //printf("begin opt cmd = %s \n", CmdBuff);

    while ( Offset < Len )  //处理结束
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            //printf(" Cmd get opt failed  cmd = %s\n", CmdBuff);
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode G0 has no param opt = %s\n", CmdBuff);
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }    

    return 1;
}


S32 GcodeG90Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    //设置模式为绝对模式
    if(MotionModel)
    {
        MotionModel->SetAbsluteMotion();
    }
    return 1;
}

S32 GcodeG90Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


