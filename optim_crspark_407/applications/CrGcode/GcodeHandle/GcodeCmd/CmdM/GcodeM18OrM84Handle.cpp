/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-28     cx2470       the first version
 */

#include "GcodeM18OrM84Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"

#define CMD_M18  "M18"
#define CMD_M84  "M84"

S32 GcodeM18OrM84Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( (strcmp(Cmd, CMD_M18) != 0) && (strcmp(Cmd, CMD_M84) != 0) )  //传进来的命令不是M18或M84命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    S32 Offset = strlen(CMD_M84) + sizeof(S8); //加1是越过分隔符
    struct ArgM18OrM84_t *CmdArg = (struct ArgM18OrM84_t *)Arg;

    while ( Offset < Len )  //处理结束
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            //printf("Gcode M18 Or M84 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_S:
            {
                CmdArg->HasS = true;
                CmdArg->S = atoi(Opt + 1);
                //rt_kprintf("Gcode M18 Or M84 Opt: %c Set timeout %d\n", OPT_S, CmdArg->S);
                break;
            }
            case OPT_E:
            {
                CmdArg->HasE = true;
                CmdArg->E = false;
                //rt_kprintf("Gcode M18 Or M84 Opt: %c Disenable %c motor\n", OPT_E, OPT_E);
                break;
            }
            case OPT_X:
            {
                CmdArg->HasX = true;
                CmdArg->X = false;
                //rt_kprintf("Gcode M18 Or M84 Opt: %c Disenable %c motor\n", OPT_X, OPT_X);
                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = true;
                CmdArg->Y = false;
                //rt_kprintf("Gcode M18 Or M84 Opt: %c Disenable %c motor\n", OPT_Y, OPT_Y);
                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = true;
                CmdArg->Z = false;
                //rt_kprintf("Gcode M18 Or M84 Opt: %c Disenable %c motor\n", OPT_Z, OPT_Z);
                break;
            }
            default:   /*关闭所有电机*/
            {
//                CmdArg->HasE = true;
//                CmdArg->HasX = true;
//                CmdArg->HasY = true;
//                CmdArg->HasZ = true;
//                CmdArg->E = false;
//                CmdArg->X = false;
//                CmdArg->Y = false;
//                CmdArg->Z = false;
                break;
            }
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    if ( (!CmdArg->HasE) && (!CmdArg->HasX) && (!CmdArg->HasY) && (!CmdArg->HasZ) && (!CmdArg->HasS) )  //如果M18/M84命令只有"M18"/"M84"，则表示关闭所有电机。
    {
        CmdArg->HasZ = true;
        CmdArg->Z = false;
        CmdArg->HasX = true;
        CmdArg->X = false;
        CmdArg->HasY = true;
        CmdArg->Y = false;
        CmdArg->HasE = true;
        CmdArg->E = false;
    }

    return 1;
}

S32 GcodeM18OrM84Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM18OrM84_t *ParamM18OrM84 = (struct ArgM18OrM84_t *)ArgPtr;

    if ( ParamM18OrM84->HasS )   //设置不活动超时
    {
        //FIXME:设置自动关闭超时时间


        return 1;
    }

    /*FIXME:阻塞等待队列中所有任务完成，清除队列计算器
          *队列中剩余的任务完成后才可以进行下面操作
     * */
    if ( (ParamM18OrM84->HasE) && (ParamM18OrM84->HasX) && ((ParamM18OrM84->HasY)) && ((ParamM18OrM84->HasZ)) )  //同时将所有电机都关闭
    {
        //清运动队列的内容
        if ( MotionModel )
        {
            //将所有电机失能
            MotionModel->DisableAllSteppers();
        }
        return 1;
    }

    //停止单个电机
    if ( ParamM18OrM84->HasE )
    {
        if ( MotionModel )
            MotionModel->DisableStepper(StepperE);  //因为Ender3v2的主板所有电机的失能是由同一个引脚来控制的
    }

    if ( ParamM18OrM84->HasX )
    {
        if ( MotionModel )
            MotionModel->DisableStepper(StepperX);
    }

    if ( ParamM18OrM84->HasY )
    {
        if ( MotionModel )
            MotionModel->DisableStepper(StepperY);
    }

    if ( ParamM18OrM84->HasZ )
    {
        if ( MotionModel )
            MotionModel->DisableStepper(StepperZ);
    }

    return 1;
}
S32 GcodeM18OrM84Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
