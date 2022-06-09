#include "GcodeM155Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrHeaterManager.h"
#include "Common/Include/CrTimer.h"
#include "Common/Include/CrTime.h"
#include "Common/Include/CrCommon.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"


#define CMD_M155 "M155"

static CrTimer_t Timer = NULL;
static S64  NextReprotTime  = 0;
static BOOL AutoReport = false;
static S32 TimerSpace = 0;   //毫秒

static VOID AutoReportTemperCallBack(VOID * Param)
{
    if ( AutoReport )
    {
        if( (CrGetSystemTimeMilSecs() - NextReprotTime) < 0 )
             return;

        NextReprotTime = CrGetSystemTimeMilSecs() + TimerSpace;
        struct HeaterManagerAttr_t Attr, BedAttr;
        if ( 0 < (HOTENDS) )
        {
            if ( HotEndArr[0] )
            {
                struct ViewHeaterMsg_t handle;
                handle.Action = GET_HEATER_MANAGER_ATTR;
                HotEndArr[0]->HandleTemperMsg((S8 *)&handle, sizeof(handle));
                Attr = HotEndArr[0]->CrHeaterMagagerGetAttr();

                S8 Cur[20] = {0}, Target[20] = {0};
                PrintFloat(Attr.CurTemper, Cur);
                PrintFloat(Attr.TargetTemper, Target);
                printf("T:%s /%s ", Cur, Target);
                //printf("T:%f /%f ", HotEndArr[0]->CrHeaterManagerGetTemper(), HotEndArr[0]->CrHeaterMagagerGetAttr().TargetTemper);
            }
         }

         if ( BedHeater )
         {
             struct ViewHeaterMsg_t handle;
             handle.Action = GET_HEATER_MANAGER_ATTR;
             BedHeater->HandleTemperMsg((S8 *)&handle, sizeof(handle));
             BedAttr = BedHeater->CrHeaterMagagerGetAttr();
             S8 Cur[20] = {0}, Target[20] = {0};
             PrintFloat(BedAttr.CurTemper, Cur);
             PrintFloat(BedAttr.TargetTemper, Target);
             printf("B:%s /%s ", Cur, Target);
             //printf("B:%f /%f ", BedHeater->CrHeaterManagerGetTemper(), BedHeater->CrHeaterMagagerGetAttr().TargetTemper);
         }

         if ( 0 < (HOTENDS) )
         {
             if ( HotEndArr[0] )
             {
                 printf("@:%d ", HotEndArr[0]->CrHeaterMagagerGetAttr().HeaterAttr.Duty);
             }
         }

         if ( BedHeater )
         {
             printf("B@:%d", BedHeater->CrHeaterMagagerGetAttr().HeaterAttr.Duty);
         }

         printf("\n");
    }

    return ;
}

S32 GcodeM155Init()
{
    struct CrTimerAttr_t TimeAttr;
    sprintf(TimeAttr.Name,"AutoReport");
    TimeAttr.HardwareTimer = 0;
    TimeAttr.Mode = RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER;
    Timer = &TimeAttr;

    CrSignalEvent_t Sig;
    Sig.sigev_notify_function = (VOID (*) (union SigVal_t))AutoReportTemperCallBack;
    Sig.sigev_value.sival_ptr = NULL;
    CrClockid_t ClockId = 0;

    CrTimerCreate(ClockId, &Sig, &Timer);
    if ( Timer == NULL)
    {
        rt_kprintf("**** M155 Create timer failed! ****\n");
        return 0;
    }

    //最开始时因该是不启动定时器的
//    CrTimerSpec_t OldTimerSpac = {0};
//    CrTimerSpec_t NewTimerSpac = {{0,500000000}};   //一秒钟一次
//    CrTimerSetTime(Timer, 1, &NewTimerSpac, &OldTimerSpac);

    return 1;
}

VOID SetAutoReportTempInterval(S32 Sec)
{
    if ( Sec <= 0 )
    {
        AutoReport = false;
    }
    else
    {
        AutoReport = true;
    }

    if ( Timer )
    {
        CrTimerSpec_t OldTimerSpac = {0};
        CrTimerSpec_t NewTimerSpac = {{Sec, 0}};   //一秒钟一次
        CrTimerSetTime(Timer, 0, &NewTimerSpac, &OldTimerSpac);
    }

    return;
}

S32 GcodeM155Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是G
        return 0;

    //
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M155) != 0 )  //传进来的命令不是G0命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    S32 Offset = strlen(CMD_M155) + sizeof(S8); //加1是越过分隔符
    struct ArgM155_t *CmdArg = (struct ArgM155_t *)Arg;

    S8 Opt[20] = {0};
    S32 OptLen = 0;

    while (Offset < Len)
    {
        memset(Opt,0,sizeof(Opt));
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            //printf(" Cmd get opt failed  cmd = %s\n", CmdBuff);
            break;    //获取参数失败
        }
        OptLen = strlen(Opt);

        if ( OptLen <= 1 )
        {
            printf("Gcode G0 has no param opt = %s\n", CmdBuff);
        }

        switch (Opt[0])
        {
            case OPT_S:

                CmdArg->HasS = 1;

                if (OptLen > 1)
                {
                    CmdArg->S = atoi(Opt + 1);
                }

            break;
            default:
            break;
        }
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

S32 GcodeM155Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM155_t *CmdArg = (struct ArgM155_t *)ArgPtr;
    if ( !CmdArg->HasS || CmdArg->S < 0 )
        return 1;

    TimerSpace = CmdArg->S * 1000;
    //NextReprotTime =  + TimerSpace;
    SetAutoReportTempInterval(CmdArg->S);
//    if ( CmdArg->S <= 0 )
//    {
//        AutoReport = false;
//    }
//    else
//    {
//        AutoReport = true;
//    }
//
//    if ( Timer )
//    {
//        CrTimerSpec_t OldTimerSpac = {0};
//        CrTimerSpec_t NewTimerSpac = {{CmdArg->S,0}};   //一秒钟一次
//        CrTimerSetTime(Timer, 0, &NewTimerSpac, &OldTimerSpac);
//    }

    return 1;
}

S32 GcodeM155Reply(VOID *ReplyResult, S8 *Buff)
{

    return 0;
}
