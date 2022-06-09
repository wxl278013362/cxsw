#include "GcodeM17Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"

#define CMD_M17  "M17"
#if 0
static CrMotion *MotionModel = NULL;

VOID GcodeM17Init(VOID *Motion)
{
    if ( MotionModel )
        MotionModel = (CrMotion *)Motion;

    return;
}
#endif

S32 GcodeM17Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M17) != 0 )  //传进来的命令不是M18或M84命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    S32 Offset = strlen(CMD_M17) + sizeof(S8); //加1是越过分隔符
    struct ArgM17_t *CmdArg = (struct ArgM17_t *)Arg;

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
            //printf("Gcode M18 Or M84 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_E:
            {
                CmdArg->HasE = true;
                CmdArg->E = true;
                break;
            }
            case OPT_X:
            {
                CmdArg->HasX = true;
                CmdArg->X = true;
                //rt_kprintf("Gcode M18 Or M84 Opt: %c Disenable %c motor\n", OPT_X, OPT_X);
                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = true;
                CmdArg->Y = true;
                //rt_kprintf("Gcode M18 Or M84 Opt: %c Disenable %c motor\n", OPT_Y, OPT_Y);
                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = true;
                CmdArg->Z = true;
                break;
            }
            default:   /*关闭所有电机*/
            {
                break;
            }
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    if ( (!CmdArg->HasE) && (!CmdArg->HasX) && (!CmdArg->HasY) && (!CmdArg->HasZ) )  //如果M17命令只有"M17"，则表示打开所有电机。
    {
        CmdArg->HasZ = true;
        CmdArg->Z = true;
        CmdArg->HasX = true;
        CmdArg->X = true;
        CmdArg->HasY = true;
        CmdArg->Y = true;
        CmdArg->HasE = true;
        CmdArg->E = true;
    }

    return 1;
}

S32 GcodeM17Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM17_t *ParamM17 = (struct ArgM17_t *)ArgPtr;
    //停止单个电机
    if ( ParamM17->HasE )
    {
        if ( MotionModel )
            MotionModel->EnableStepper(StepperE);  //因为Ender3v2的主板所有电机的失能是由同一个引脚来控制的
    }

    if ( ParamM17->HasX )
    {
        if ( MotionModel )
            MotionModel->EnableStepper(StepperX);
    }

    if ( ParamM17->HasY )
    {
        if ( MotionModel )
            MotionModel->EnableStepper(StepperY);
    }

    if ( ParamM17->HasZ )
    {
        if ( MotionModel )
            MotionModel->EnableStepper(StepperZ);
    }

    return 1;
}

S32 GcodeM17Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
