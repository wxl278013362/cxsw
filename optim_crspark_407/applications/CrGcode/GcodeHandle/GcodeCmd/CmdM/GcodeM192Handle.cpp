#include "GcodeM192Handle.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrHeaterManager.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrMsgQueue/sharedQueue/CrSharedMsgQueue.h"
#include "CrInc/CrMsgType.h"
#include "CrModel/CrGcode.h"
#include "Common/Include/CrSleep.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"

#define CMD_M192        "M192"

// MARK:变量应该在控制模块添加，后期融合代码前需要将变量定义迁过去
U8 DryRun = 0;

/**
 *
 * 解析M192指令
 *
 * @param CmdBuff 上位机下发的命令字符串
 * @param Arg 保存命令参数的缓冲区
 * @return 解析成功返回1，否则返回0
 */
S32 GcodeM192Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || CmdBuff[0] != GCODE_CMD_M )
    {
        return 0;
    }

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_M192) != 0 )
    {
        return 0;
    }

    S32 Offset = strlen(CMD_M192) + sizeof(S8);
    struct ArgM192_t *CmdArg = (struct ArgM192_t *)Arg;
    memset((char *)CmdArg, 0, sizeof(struct ArgM192_t));

    S32 OptLen = 0;
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;
        }

        OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M192 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_R:
            {
                CmdArg->HasR = true;
                if ( OptLen > 1 )
                {
                    CmdArg->R = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->R = 0;
                }
                break;
            }
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
            {
                break;
            }
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

/**
 *
 * 执行M192
 *
 * @param ArgPtr M192指令的参数
 * @param ReplyResult 保存执行后需要返回的结果
 * @return 执行成功返回1，否则返回0
 */
S32 GcodeM192Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !ProbeHeater)
    {
        return 0;
    }

    if ( DryRun )
    {
        return 1;
    }

    struct ArgM192_t *Param = (struct ArgM192_t *)ArgPtr;
    struct HeaterManagerAttr_t Attr;
    BOOL NoWaitForCooling = false;
    BOOL HasTargetTemperature = false;

    if ( Param->HasS && Param->S != 0 )
    {
        NoWaitForCooling = true;
    }

    if ( Param->HasR && Param->R != 0 )
    {
        HasTargetTemperature = true;
        Attr = ProbeHeater->CrHeaterMagagerGetAttr();
        Attr.TargetTemper = Param->R;
        ProbeHeater->CrHeaterMagagerSetAttr(Attr);
    }

    if ( !NoWaitForCooling && !HasTargetTemperature )
    {
        printf("No target temperature set.");                   //打印错误信息
        return 0;
    }

    if ( ProbeHeater->CrHeaterManagerGetTemper() > Param->R )   //与设定温度比较
    {
        printf("Refroid. Probe...");                            //显示降温
    }
    else
    {
        printf("Probe en chauffe...");                          //显示升温
    }

    WaitForProbe(Param->R, NoWaitForCooling);

    return 1;
}

S32 GcodeM192Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
