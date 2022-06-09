#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "CrInc/CrMsgType.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "GcodeM5Handle.h"
#include "GcodeM3Handle.h"

#define CMD_M5 "M5"

/*******额外定义的变量和函数********/
/*******额外定义的变量和函数********/


S32 GcodeM5Parse(const S8 *CmdBuff, VOID *Arg)
{
    //1、判空
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    //2、判断Gcode命令长度是否大于1且是否为M命令
    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || CmdBuff[0] != GCODE_CMD_M )
    {
        return 0;
    }

    //3、获取命令
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )   //防止获取命令失败
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_M5) != 0 )    //判断是否是M5命令
    {
        return 0;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M5) + sizeof(S8);
    struct ArgM5_t *CmdArg = (struct ArgM5_t *)Arg;

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
            printf("Gcode M5 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_T:
            {
                CmdArg->HasT = true;
                if ( OptLen > 1 )
                {
                    CmdArg->T = !!atoi(Opt + 1);
                }
                else
                {
                    CmdArg->T = false;
                }
                break;
            }
            default:
                break;
        }
        Offset += strlen(Opt) + sizeof(S8);
    }
    return 1;
}

S32 GcodeM5Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM5_t *Param = (struct ArgM5_t *)ArgPtr;
    BOOL Flag = 0;
#ifdef LASER_POWER_INLINE
#ifdef LASER_POWER_INLINE_INVERT
    Flag = 0;
#else
    Flag = 1;
    if ( Param->HasT == Flag )
    {
        CutterSetInlineEnabled(false);
        return 0;
    }
    CutterInlineDisable();
#endif
#endif

    PlannerSynchronize();
    CutterSetEnabled(false);
    MenuPower = UnitPower;
    return 1;
}

S32 GcodeM5Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
