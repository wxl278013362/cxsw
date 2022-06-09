#include "GcodeM24Handle.h"
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrPrinter.h"
#include "CrModel/CrGcode.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrInc/CrMsgType.h"

static CrMsg_t LineMsgQueue = NULL;

#define CMD_M24 ("M24")

VOID GcodeM24Init()
{
    LineMsgQueue = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));

    return ;
}

S32 GcodeM24Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M24) != 0 )  //传进来的命令不是M24命令
        return 0;

    rt_kprintf("%s\n", CmdBuff);

    return 1;
}
#if 0
#include <stdlib.h>
const double eps = 1e-3;
VOID FloatStr(S8 *Str, double Num, S32 Size)
{
    memset(Str, 0, Size);
//
//    S32 Temp = Num;
//    S32 SubTemp = abs((Num - Temp) * 10000);
//    if ( (SubTemp % 10) == 9 )
//    {
//        SubTemp += (10 - (SubTemp % 10));
//    }
//
//    SubTemp /= 10;
//    sprintf(Str, "%d.%03d", Temp, SubTemp);
     S32  High;//float_整数部分
     double Low;//float_小数部分
     char *Start = Str;
     int n = 0, i;
     char Ch[20];

     High = (int)Num;
     Low = Num - High;

     while ( High > 0 )
     {
         Ch[n++] = '0' + High % 10;
         High = High / 10;
     }

     for( i = n - 1; i >= 0; i--){
         *Str++ = Ch[i];
     }

     Num -= (S32)Num;
     FLOAT Tp = 0.1;
     *Str++ = '.';

     while ( Num > eps )
     {//精度限制
         Num -= Tp * (int)(Low * 10);
         Tp /= 10;
         *Str++ ='0'+ (int)(Low * 10);
         Low = Low * 10.0 - (S32)(Low * 10);
     }
     *Str = '\0';
     Str = Start;
}
VOID FloatToString(S8 *Str, FLOAT Num, S32 Size)
{
    if ( !Str || Size < 1)
    {
        return;
    }
    S32 Temp = (S32)Num ;
    memset(Str, 0, sizeof(Str));
    char StrTemp[10] = {0};
    itoa(Temp,StrTemp,10);
    strcpy(Str,StrTemp);
    Temp = (S32)(Num * 1000 - (S32)(Num * 1000));
    Str[strlen(Str)] = '.';
    memset(StrTemp, 0, sizeof(StrTemp));
    itoa(Temp,StrTemp,10);

    if ( Temp < 10 )
    {
        Str[strlen(Str)] = '0';
        Str[strlen(Str)] = '0';
    }
    if(Temp < 100 && Temp > 10)
    {
        Str[strlen(Str)] = '0';
    }
    strcat(Str,StrTemp);
    rt_kprintf("%s\n",Str);

}
#endif

VOID WaitTemperatureRecover()
{
    if ( !BedHeater )
        return ;

    struct HeaterManagerAttr_t BedAttr = BedHeater->CrHeaterMagagerGetAttr();
    BedAttr.TargetTemper = Printer->GetBedTemp();
    /*Set target Temperature of Bed*/
    BedHeater->CrHeaterMagagerSetAttr(BedAttr);

    if ( GcodeModel )
        GcodeModel->StartWaiting();

    if ( (HOTENDS > 1) && (HotEndArr[0])  )
    {
        struct HeaterManagerAttr_t NozAttr = HotEndArr[0]->CrHeaterMagagerGetAttr();
        NozAttr.TargetTemper = Printer->GetNozTemp();
        /*Set target Temperature of Nozzle*/
        HotEndArr[0]->CrHeaterMagagerSetAttr(NozAttr);

        while ( 1 )
        {
            if ( GcodeModel )
                if( !GcodeModel->IsWaiting() )
                    break;

            if ( HotEndArr[0]->ReachedTargetTemper() )
            {
                S32 i = 10;
                do
                {
                    if ( GcodeModel )
                        if( !GcodeModel->IsWaiting() )
                            break;

                    rt_thread_delay(300);

                }while ( i-- && HotEndArr[0]->ReachedTargetTemper() );

                if (i <= 0 )
                    break;
            }
            rt_thread_delay(500);
        }

    }

    rt_kprintf("M24 stop waiting heatting");

    if ( GcodeModel )
        GcodeModel->StopWaiting();
}

S32 GcodeM24Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !Printer )
        return 0;

    /*Recover Temperature*/
    Printer->ResumeHandle();
    struct MsgLine_t LineMsg = {0};
    LineMsg.MsgType = SPECIAL_MSG;
    strcpy(LineMsg.Info, "Nozzle Heating");
    if ( LineMsgQueue )
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
    //rt_kprintf("++++++++++++++++++++++line msg : %s\n", LineMsg.Info);
    WaitTemperatureRecover();
//    rt_thread_delay(3000);
    LineMsg.MsgType = SPECIAL_MSG;
    strcpy(LineMsg.Info, "");
    LineMsg.Finished = 1;
    if ( LineMsgQueue )
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);

//    if ( Printer->GetPrintStatus() == PRINTING )
        /*Recover motion*/
     MotionModel->MotionResume();
    /*Update state of print*/


    return 1;
}

S32 GcodeM24Reply(VOID *ReplyResult, S8 *Buff)
{

    return 0;
}
