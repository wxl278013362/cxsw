#include "GcodeM569Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"

#define CMD_M569 ("M569")

VOID SayTmcMode(S8 *AxisName, S8 TmcStealthChopStatus)    //获取TMC是否是StealthChopStatus
{
    if ( AxisName )
        printf("%s", AxisName);
    printf(" driver mode:\t");
    printf("%s\n", TmcStealthChopStatus ? "stealthChop" : "spreadCycle");
}

void TmcSetStealthChop(S32 TmcStepperFd, const BOOL Enable)
{
    //st.stored.stealthChop_enabled = enable;  //设置是否启动静音模式
    //st.refresh_stepping_mode();     //刷新电机模式

    return ;
}


BOOL IsTmcStepper(U32 Fd)
{

    return true;
}

BOOL GetTmcStepperStealthChopStatus(U32 Fd)
{
    if ( IsTmcStepper(Fd) )
        return false;

    //获取

    return true;
}

S32 GcodeM569Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M911
    if ( strcmp(Cmd, CMD_M569) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M569) + sizeof(S8); //加1是越过分隔符
    struct ArgM569_t *CmdArg = (struct ArgM569_t *)Arg;

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
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                {
                    CmdArg->S = atoi( Opt + 1 );
                }
                else
                {
                    CmdArg->S = 1;
                }

                break;
            }
            case OPT_I:
            {
                CmdArg->HasI = true;
                if ( OptLen > 1 ) CmdArg->I = atoi( Opt + 1 );
                break;
            }
            case OPT_T:
            {
                CmdArg->HasT = true;
                if ( OptLen > 1 ) CmdArg->T = atoi( Opt + 1 );
                break;
            }
            case OPT_E:
            {
                CmdArg->HasE = true;
                break;
            }
            case OPT_X:
            {
                CmdArg->HasX = true;
                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = true;
                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = true;
                break;
            }
            default:
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM569Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM569_t *CmdArg = (struct ArgM569_t *)ArgPtr;
    if ( CmdArg->HasS )
    {
        S32 Fd = 0;
        //设置电机的模式
        if ( CmdArg->HasX )
        {
            if ( CmdArg->I == 0 )
            {
                Fd = 0; //获取X轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
            else if ( CmdArg->I == 1 )
            {
                Fd = 0; //获取X2轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
        }

        if ( CmdArg->HasY )
        {
            if ( CmdArg->I == 0 )
            {
                Fd = 0; //获取Y轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
            else if ( CmdArg->I == 1 )
            {
                Fd = 0; //获取Y2轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
        }

        if ( CmdArg->HasZ )
        {
            if ( CmdArg->I == 0 )
            {
                Fd = 0; //获取Z轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
            else if ( CmdArg->I == 1 )
            {
                Fd = 0; //获取Z2轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
            else if ( CmdArg->I == 2 )
            {
                Fd = 0; //获取Z3轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
            else if ( CmdArg->I == 3 )
            {
                Fd = 0; //获取Z4轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
        }

        if ( CmdArg->HasE )
        {
            if ( CmdArg->T == 0 )
            {
                Fd = 0; //获取E轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
            else if ( CmdArg->T == 1 )
            {
                Fd = 0; //获取E1轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
            else if ( CmdArg->T == 2 )
            {
                Fd = 0; //获取E2轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
            else if ( CmdArg->T == 3 )
            {
                Fd = 0; //获取E3轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
            if ( CmdArg->T == 4 )
            {
                Fd = 0; //获取E4轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
            else if ( CmdArg->T == 5 )
            {
                Fd = 0; //获取E5轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
            else if ( CmdArg->T == 6 )
            {
                Fd = 0; //获取E6轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
            else if ( CmdArg->T == 7 )
            {
                Fd = 0; //获取E7轴电机的Fd
                TmcSetStealthChop(Fd, CmdArg->S);
            }
        }

    }
    else
    {
        //报告电机的模式
        S32 Fd = 0;   //获取X轴电机的Fd
        BOOL StealthChopStatus = false;   //是否是静音状态
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("X", StealthChopStatus);
        }

        Fd = 0;  //获取Z轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("Y", StealthChopStatus);
        }

        Fd = 0;  //获取E轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("Z", StealthChopStatus);
        }

        Fd = 0;  //获取X2轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("X2", StealthChopStatus);
        }

        Fd = 0;  //获取Y2轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("Y2", StealthChopStatus);
        }

        Fd = 0;  //获取Z2轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("Z2", StealthChopStatus);
        }
        Fd = 0;  //获取Z3轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("Z3", StealthChopStatus);
        }
        Fd = 0;  //获取Z4轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("Z4", StealthChopStatus);
        }

        Fd = 0;  //获取E轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("E", StealthChopStatus);
        }
        Fd = 0;  //获取E1轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("E1", StealthChopStatus);
        }
        Fd = 0;  //获取E2轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("E2", StealthChopStatus);
        }
        Fd = 0;  //获取E3轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("E3", StealthChopStatus);
        }
        Fd = 0;  //获取E4轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("E4", StealthChopStatus);
        }
        Fd = 0;  //获取E5轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("E5", StealthChopStatus);
        }
        Fd = 0;  //获取E6轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("E6", StealthChopStatus);
        }
        Fd = 0;  //获取E7轴电机的Fd
        if ( IsTmcStepper(Fd) )
        {
            StealthChopStatus = GetTmcStepperStealthChopStatus(Fd);
            SayTmcMode("E7", StealthChopStatus);
        }
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM569Reply(VOID *ReplyResult, S8 *Buff)
{

    return 0;
}
