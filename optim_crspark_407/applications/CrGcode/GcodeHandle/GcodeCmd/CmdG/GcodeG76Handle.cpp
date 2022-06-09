#include "GcodeG76Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../../GcodeHandle.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "Common/Include/CrSleep.h"
#include "Common/Include/CrTime.h"
#include "CrModel/CrHeaterManager.h"
#include "CrModel/CrMotion.h"

#define CMD_G76  "G76"

static FLOAT SavedFeedrate;
static S16 SavedFeedratePercentage;

enum TempSensorID : uint8_t {
  TSI_PROBE,
  TSI_BED,
//  #if ENABLED(USE_TEMP_EXT_COMPENSATION)
//    TSI_EXT,
//  #endif
  TSI_COUNT
};


static VOID PrintHeaterState(const FLOAT &CurTemper, const FLOAT &TargetTemper, const heater_ind_t e=INDEX_NONE)
{
  S8 k;
  switch (e) {
      case H_CHAMBER: k = 'C'; break;
      case H_PROBE: k = 'P'; break;
      case H_BED: k = 'B'; break;
      case H_REDUNDANT: k = 'R'; break;
      default: k = 'T'; break;
  }
  printf(" %c", k);
  if ( HOTENDS > 1 )
    if (e >= 0) printf("%c", '0' + e);

  printf(":%f/%f", CurTemper, TargetTemper);

  CrM_Sleep(2);
}

static VOID PrintHeaterStates(U32 ExtruderIndex, BOOL UseInc = false, S32 Inc = 0)
{
    //目标挤出机的温度状态
    if ( (ExtruderIndex > 0) && (ExtruderIndex < HOTENDS)  )
    {
        if ( HotEndArr[ExtruderIndex] )
        {
            struct HeaterManagerAttr_t Attr = HotEndArr[ExtruderIndex]->CrHeaterMagagerGetAttr();
            PrintHeaterState(Attr.CurTemper, Attr.TargetTemper, (heater_ind_t)(H_E0 + ExtruderIndex));
        }
    }

    //热床的温度状态
    if ( BedHeater )
    {
        struct HeaterManagerAttr_t Attr = BedHeater->CrHeaterMagagerGetAttr();
        PrintHeaterState(Attr.CurTemper, Attr.TargetTemper, H_BED);
    }

    //加热室的温度状态

    //探针的温度状态


    //多挤出机的温度状态
    if ( HOTENDS > 1 )
    {
        S32 i;
        for ( i = 0; i < HOTENDS ; i++ )
        {
            if ( HotEndArr[ExtruderIndex] )
            {
                struct HeaterManagerAttr_t Attr = HotEndArr[ExtruderIndex]->CrHeaterMagagerGetAttr();
                PrintHeaterState(Attr.CurTemper, Attr.TargetTemper, (heater_ind_t)(H_E0 + i));
            }
        }
    }


    //目标挤出机的功率
    if ( (ExtruderIndex > 0) && (ExtruderIndex < HOTENDS)  )
    {
        if ( HotEndArr[ExtruderIndex] )
        {
            struct HeaterManagerAttr_t Attr = HotEndArr[ExtruderIndex]->CrHeaterMagagerGetAttr();
            printf(" @:", Attr.HeaterAttr.Duty);
        }
    }

    //热床的功率
    if ( BedHeater )
    {
        struct HeaterManagerAttr_t Attr = BedHeater->CrHeaterMagagerGetAttr();
        printf(" B@:", Attr.HeaterAttr.Duty);
    }

    //加热室的功率

    //多挤出机的功率
    if ( HOTENDS > 1 )
    {
        S32 i;
        for ( i = 0; i < HOTENDS ; i++ )
        {
            if ( HotEndArr[ExtruderIndex] )
            {
                struct HeaterManagerAttr_t Attr = HotEndArr[ExtruderIndex]->CrHeaterMagagerGetAttr();
                printf(" @:", Attr.HeaterAttr.Duty);
            }
        }
    }

    printf("\n");

    return ;
}

static auto ReportTemps = [](U32 &Ntr, U32 Timeout=0) {  //打印当前温度和目标温度
//    idle_no_sleep();    //这个内容暂时不实现
    const U32 Ms = CrGetSystemTimeMilSecs();
    if ( Ms >= Ntr )
    {
        Ntr = Ms + 1000;
        PrintHeaterStates(0);    //应为现在没有活动挤出机索引，所以给出的是0
    }

    return (Timeout && (Ms >= Timeout ? 1 : 0));
};

static auto WaitForTemps = [&](const FLOAT Tb, const FLOAT Tp, U32 &Ntr, const U32 Timeout=0)
{   //timeout是超时的具体时间点
    printf("Waiting for bed and probe temperature.\n");
    if ( !BedHeater || !ProbeHeater )
        return false;


    while (fabs(BedHeater->CrHeaterManagerGetTemper() - Tb) > 0.1f || ProbeHeater->CrHeaterManagerGetTemper() > Tp)   //床没有达到预热温度或探针的温度大于预热温度
        if (ReportTemps(Ntr, Timeout)) return true;

    return false;
};

static auto G76Probe = [](const S32 Sid, U16 &Targ, const MotionCoord_t &Nozpos)
{
    FLOAT MeasuredZ = 0.1;
#if 0       //此部分的内容暂时缺失，接口实现延后
    do_z_clearance(5.0); // Raise nozzle before probing
    const float measured_z = probe.probe_at_point(nozpos, PROBE_PT_STOW, 0, false);  // verbose=0, probe_relative=false, 获取探针的ZOffset.
    if (isnan(measured_z))
      SERIAL_ECHOLNPGM("!Received NAN. Aborting.");
    else {
      SERIAL_ECHOLNPAIR_F("Measured: ", measured_z);
      if (targ == cali_info_init[sid].start_temp)      //目标温度是否和传感器的开始测量温度相同
        temp_comp.prepare_new_calibration(measured_z);  //开启新的测量时的探针的
      else
        temp_comp.push_back_new_measurement(sid, measured_z);
      targ += cali_info_init[sid].temp_res;   //获取下一次的预热温度
    }
#endif

    return MeasuredZ;
};

static auto ReportTargets = [&](const U16 Tb, const U16 Tp)
{
      printf("Target Bed:%f Probe:%f\n", Tb, Tp);
};

S32 GcodeG76Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_G76) != 0 )  //传进来的命令不是M18或M84命令
    {
        return 0;
    }

    S32 Offset = strlen(CMD_G76) + sizeof(S8); //加1是越过分隔符
    struct ArgG76_t *ArgPtr = (struct ArgG76_t *)Arg;
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
            case OPT_B:
           {
                ArgPtr->HasB = true;
                break;
            }
            case OPT_P:
            {
                ArgPtr->HasP = true;
                break;
            }
            default:
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}

S32 GcodeG76Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult  || !MotionModel )
        return 0;

    //检查是否有可用的加热床，并用探针进行Z轴归零，如果有任意一项不满足就退出


    //如果探针可用就进行警告释放和探针回缩


    struct ArgG76_t *Arg = (struct ArgG76_t *)ArgPtr;
    if ( !(Arg->HasB) && !(Arg->HasP) )
    {
        Arg->HasB = Arg->HasP = true;
    }

    //等待所有的运动完成
    while ( !MotionModel->IsMotionMoveFinished() )
    {
        CrM_Sleep(10);
    }

    //获取停靠位置，探针位置(比探测点高0.5mm)和喷头的位置
    struct MotionCoord_t ParkCoord;
    struct MotionCoord_t ProbeCoord;
    struct MotionCoord_t NozCoord;

    if ( Arg->HasB || Arg->HasP )
    {
        BOOL ParkReachAble = false, ProbeReachAble = false;
        //判断停靠点的XYZ坐标是否可以到达并赋值给ParkReachAble

        //判断探针位置的XY坐标是否可以到达并赋值给ProbeReachAble

        if ( !ParkReachAble )
        {
            printf("!Park  position unreachable - aborting.\n");
            return 1;
        }
        else
        {
            if ( !ProbeReachAble )
            {
                printf("!Probe  position unreachable - aborting.\n");
                return 1;
            }
        }

        S8 Replay[20] = {0};
        GcodeHandle("G28", strlen("G28"), Replay);
    }

    struct FeedrateAttr_t FeedRate = MotionModel->GetMotionFeedRateAttr();
    SavedFeedrate = FeedRate.Feedrate;
    SavedFeedratePercentage = FeedRate.FeedrateRatio;
    FeedRate.FeedrateRatio = 100;
    MotionModel->SetMotionFeedRateAttr(FeedRate);

    //计算下一个报告温度的时间（每一秒中报告一次）
    U32 NextReportTime = CrGetSystemTimeMilSecs() + 1000;

    //开始探测热床的温度
    if ( Arg->HasB )
    {
        U16 TargetBed = 0, //床校准时的热床的开始温度（即目标温度）
            TargetProbe = 0;   //校准床温时探针的目标温度

        printf("Waiting for cooling.\n");
        if ( BedHeater && ProbeHeater )
        {
            while ( (BedHeater->CrHeaterManagerGetTemper() > TargetBed)
                    || (ProbeHeater->CrHeaterManagerGetTemper() > TargetProbe) )
                ReportTemps(NextReportTime, 0);

        }

        //关闭调平

        //开始加热
        for ( ;; )
        {
            if ( BedHeater )
            {
                struct HeaterManagerAttr_t Attr = BedHeater->CrHeaterMagagerGetAttr();
                Attr.TargetTemper = TargetBed;
                BedHeater->CrHeaterMagagerSetAttr(Attr);
            }

            ReportTargets(TargetBed, TargetProbe);

            //喷头运动到停泊点

            //等待温度
            if ( WaitForTemps(TargetBed, TargetProbe, NextReportTime, CrGetSystemTimeMilSecs() + 15 * 60000) )   //等待时间是15分钟
            {
                printf("!Bed heating timeout.\n");   //到了时间后热床无法到达预热温度，探针无法降到预热温度之下
                break;
            }

            //将喷头运动到预定位置

            //在探针温度到达预定温度前隔一段时间报告一次温度
            if ( ProbeHeater )
            {
                while (ProbeHeater->CrHeaterManagerGetTemper() < TargetProbe)  //等待探针到达预热温度
                       ReportTemps(NextReportTime, 0);  //上报温度并形成下一次的上报时间， 报告热床的温度
            }


            //获取探针的Z轴位置
            FLOAT MeasuredZ = 0;
            //判断
            if (isnan(MeasuredZ) || TargetBed > BED_MAX_TARGET) break;   //有效的ZOffset和目标温度
        }

        //
        S32 Index = 0;

        //获取校准的次数

        //打印输出
        printf("Retrieved measurements: %d\n", Index);  //打印测试的次数
        BOOL Finish = false;
        //获取是否校准完成

        //输出提示
        if (Finish)    //所有的目标温度都测试了一遍
            printf("Successfully calibrated bed.\n");
        else
            printf("!Failed to calibrate bed. Values reset.\n");

        // Cleanup
        if ( BedHeater )
        {
            struct HeaterManagerAttr_t Attr = BedHeater->CrHeaterMagagerGetAttr();
            Attr.TargetTemper = 0;
            BedHeater->CrHeaterMagagerSetAttr(Attr);
        }

        //使能调平


    }


    //开始探测探针的温度
    if ( Arg->HasP )
    {
        //运动到停泊点

        //获取热床和喷头的目标温度
        U16 TargetBed = 0, //探针校准时的热床的目标温度
            TargetProbe = 0;   //探针校准时探针的目标温度

        if ( BedHeater )
        {
            struct HeaterManagerAttr_t Attr = BedHeater->CrHeaterMagagerGetAttr();
            Attr.TargetTemper = TargetBed;
            BedHeater->CrHeaterMagagerSetAttr(Attr);
        }

        ReportTargets(TargetBed, TargetProbe);

        WaitForTemps(TargetBed, TargetProbe, NextReportTime, 0);

        //关闭调平


        BOOL Timeout = false;
        for ( ;; )
        {
             //将喷头运动到预定位置

             //打印输出
             printf("Waiting for probe heating. Bed:%d Probe:%d", TargetBed, TargetProbe);
             const U32 ProbeTimeout = CrGetSystemTimeMilSecs() + 900UL * 1000UL;   //15分钟
             if ( ProbeHeater )
             {
                 while (ProbeHeater->CrHeaterManagerGetTemper() < TargetProbe)
                 {
                    if (ReportTemps(NextReportTime, ProbeTimeout))
                    {
                      printf("!Probe heating timed out.\n");
                      Timeout = true;
                      break;
                    }
                 }
             }
             if (Timeout) break;

             //获取Z轴的位置
             FLOAT MeasuredZ = 0;
             //获取校准探针的结束温度
             S32 EndTemp = 0;
             if (isnan(MeasuredZ) || TargetProbe > EndTemp) break;
        }

        //
        S32 Index = 0;

        //获取校准的次数

        //打印输出
        printf("Retrieved measurements: %d\n", Index);  //打印测试的次数
        BOOL Finish = false;
        //获取是否校准完成

        //输出提示
        if (Finish)    //所有的目标温度都测试了一遍
            printf("Successfully calibrated probe.\n");
        else
            printf("!Failed to calibrate probe.\n");

        if ( BedHeater )
        {
            struct HeaterManagerAttr_t Attr = BedHeater->CrHeaterMagagerGetAttr();
            Attr.TargetTemper = 0;
            BedHeater->CrHeaterMagagerSetAttr(Attr);
        }

        //使能调平


        //
        printf("Final compensation values:");
        //打印校准位置的偏移



    }

    //恢复速度
    FeedRate.Feedrate = SavedFeedrate;
    FeedRate.FeedrateRatio = SavedFeedratePercentage;
    MotionModel->SetMotionFeedRateAttr(FeedRate);

    return 1;
}

S32 GcodeG76Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
