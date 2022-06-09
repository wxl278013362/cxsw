#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrHeaterManager.h"
#include "GcodeM141Handle.h"

#define CMD_M141        "M141"

VOID AutoJobCheckTimer(const BOOL CanStart,const BOOL CanStop)
{
    //if CanStart 开始打印任务计时器

    //if CanStop  停止打印任务计时器，并重置屏幕状态栏
}

S32 GcodeM141Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }
    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1并且是M命令
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M141) != 0 )  //传进来的命令不是M141命令
    {
        return 0;
    }

    S32 Offset = strlen(CMD_M141) + sizeof(S8); //加1是越过分隔符
    struct ArgM141_t *CmdArg = (struct ArgM141_t *)Arg;

    while ( Offset < Len  )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;
        }
        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M141 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                {
                    CmdArg->S = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->S = 0;
                }
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt + sizeof(S8));
    }
    return 1;
}
S32 GcodeM141Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !ChamberHeater)
    {
        return 0;
    }

    struct ArgM141_t *Param = (struct ArgM141_t *)ArgPtr;

    if ( Param->HasS )
    {
        //设置目标温度
        struct HeaterManagerAttr_t Attr = {0};
        Attr = ChamberHeater->CrHeaterMagagerGetAttr();
        Attr.TargetTemper = Param->S;

        //打印计时器停止，屏幕重置状态栏消息
        AutoJobCheckTimer(false,true);
    }
    else
    {
        printf("M141 has no target temperature.\n");
    }
    return 1;
}
S32 GcodeM141Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
