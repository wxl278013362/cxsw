#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "GcodeM4Handle.h"
#include "CrInc/CrMsgType.h"
#include "CrInc/CrType.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrMotion/CrMotionPlanner.h"
#include "CrModel/CrPrinter.h"
#include "Common/Include/CrSleep.h"
#include "CrGpio/CrGpio.h"


#define CMD_M4 "M4"
/*******额外定义的变量和函数********/
/*******额外定义的变量和函数********/


S32 GcodeM4Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd,CMD_M4) != 0 )    //判断是否是M4命令
    {
        return 0;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M4) + sizeof(S8);
    struct ArgM4_t *CmdArg = (struct ArgM4_t *)Arg;

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
            printf("Gcode M4 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_I:
            {
                CmdArg->HasI = true;
                break;
            }
            case OPT_O:
            {
                CmdArg->HasO = true;
                if ( OptLen > 1 )
                {
                    CmdArg->O = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->O = 0;
                }
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                {
                    CmdArg->O = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->HasS = false;
                }
            }
            default:
                break;
        }
        Offset += strlen(Opt) + sizeof(S8);
    }
    return 1;
}

S32 GcodeM4Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM4_t *Param = (struct ArgM4_t *)ArgPtr;
    BOOL Flag = 0;
#ifdef LASER_POWER_INLINE
#ifdef LASER_POWER_INLINE_INVERT
    Flag = 0;
#else
    Flag = 1;
#endif
    if ( Param->HasI == Flag )
    {
        CutterInlineDirection(true);
#ifdef SPINDLE_LASER_PWM
        if ( Param->HasO )
        {
            CutterUnitPower = CutterPowerToRange(Param->O,0);
            CutterInlineOcrPower(CutterUnitPower);
        }
        else
        {
            CutterInlinePower(UpowerToOcr(GetPowerS()));
        }
#else
        CutterSetInlineEnabled();

#endif
        return 0;
    }
    CutterInlineDisable();
#endif

    //改变电源前等待所有的运动命令执行完
    PlannerSynchronize();

    //
    CutterSetReverse(true);

#ifdef SPINDLE_LASER_PWM
    if ( Param->HasO )
    {
        UnitPower = CutterPowerToRange(Param->O, 0);
        CutterSetOcrPower(UnitPower);
    }
    else
    {
        CutterSetPower(UpowerToOcr(GetPowerS(Param)));
    }
#elif SPINDLE_SERVO
    CutterSetPower(GetPowerS(Param));
#else
    CutterSetEnabled(true);
#endif
    MenuPower = UnitPower;
    return 1;
}

S32 GcodeM4Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
