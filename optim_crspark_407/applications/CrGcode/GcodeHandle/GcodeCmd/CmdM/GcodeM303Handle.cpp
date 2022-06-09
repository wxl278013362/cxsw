#include "GcodeM303Handle.h"
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "Common/Include/CrTime.h"
#include "CrModel/CrHeaterManager.h"
#include "CrHeater/Pid/CrPid.h"

#define STR_ON                              "ON"
#define STR_OFF                             "OFF"

#define CMD_M303           "M303"
static CrMsg_t LineMsgQueue = NULL;

#ifdef PID_DEBUG
  BOOL PidDebugFlag = 0;
#endif

// Did the temperature overshoot very far?
#ifndef MAX_OVERSHOOT_PID_AUTOTUNE
    #define MAX_OVERSHOOT_PID_AUTOTUNE 30
#endif

// Timeout after MAX_CYCLE_TIME_PID_AUTOTUNE minutes since the last undershoot/overshoot cycle
#ifndef MAX_CYCLE_TIME_PID_AUTOTUNE
    #define MAX_CYCLE_TIME_PID_AUTOTUNE 20L
#endif

#define OVERSAMPLENR 1
#define TEMP_TIMER_PRESCALE     1000 // prescaler for setting Temp timer, 72Khz
#define TEMP_TIMER_FREQUENCY    1000 // temperature interrupt frequency
#define MIN_ADC_ISR_LOOPS 10

#ifndef SensorsReady
#define SensorsReady (int(HOTENDS) + 1)
#endif

#define ACTUAL_ADC_SAMPLES (int(MIN_ADC_ISR_LOOPS) > int(SensorsReady) ?  int(MIN_ADC_ISR_LOOPS) : int(SensorsReady))

#define PID_K2 (1-float(PID_K1))
#define PID_dT ((OVERSAMPLENR * float(ACTUAL_ADC_SAMPLES)) / TEMP_TIMER_FREQUENCY)

// Apply the scale factors to the PID values
#define scalePID_i(i)   ( float(i) * PID_dT )
#define unscalePID_i(i) ( float(i) / PID_dT )
#define scalePID_d(d)   ( float(d) / PID_dT )
#define unscalePID_d(d) ( float(d) * PID_dT )


#ifndef M_PI
  #define M_PI 3.14159265358979323846f
#endif

static VOID PidAutoTune(const FLOAT Target, const S32 HeaterIndex, const S8 Cycles, const BOOL SetResult);
VOID DisableAllHeaters();
VOID checkExtruderAutoFans();

VOID TemperError(heater_ind_t Index, S8 *ErrMsg, S8 *LcdShowMsg);

S32 GcodeM303Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    if ( !LineMsgQueue )
        LineMsgQueue = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0] != GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M303) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M303) + sizeof(S8); // 加1是越过分隔符
    struct ArgM303_t *CmdArg = (struct ArgM303_t *)Arg;

    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) ) break;

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )
        {
            case OPT_C:
            {
                CmdArg->HasC = true;
                if ( OptLen > 1 )
                {
                    CmdArg->C = atoi( Opt + 1 );
                }

                break;
            }
            case OPT_D:
            {
                CmdArg->HasD = true;
//                if ( OptLen > 1 ) CmdArg->D = atoi( Opt + 1 );
//                break;
            }
            case OPT_E:
            {
                CmdArg->HasE = true;
                if ( OptLen > 1 ) CmdArg->E = atoi( Opt + 1 );
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atof( Opt + 1 );
                break;
            }
            case OPT_U:
            {
                CmdArg->HasU = true;
                if ( OptLen > 1 )
                {
                    CmdArg->U = atoi( Opt + 1 );
                }
                else
                {
                    CmdArg->U = 1;
                }
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    if ( !CmdArg->HasE )
    {
        CmdArg->HasE =true;
        CmdArg->E = 0;
    }

    if ( !CmdArg->HasU )
    {
        CmdArg->HasU = true;
        CmdArg->U = 0;
    }

    return 1;
}

S32 GcodeM303Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM303_t *Arg = (struct ArgM303_t *)ArgPtr;
#ifdef PID_DEBUG
    if ( Arg->HasD  )
    {
        PidDebugFlag != PidDebugFlag;
        printf("PID Debug ");
        printf("%s\n", PidDebugFlag ? STR_ON : STR_OFF);
        return 1;
    }
#endif

    S8 Index = Arg->E;  //索引碰头的索引为0到HOTENDS -1, 热床的索引是-1
    if ( (Index < -1) || (Index >= HOTENDS) )
    {
        printf("%s\n", STR_PID_BAD_EXTRUDER_NUM);

        //TERN_(EXTENSIBLE_UI, ExtUI::onPidTuning(ExtUI::result_t::PID_BAD_EXTRUDER_NUM));  //暂时不实现
        return 1;
    }

    FLOAT Temper = 0.0f;
    if (Arg->HasS)
    {
        Temper = Arg->S;
    }
    else
    {
        if( Index < 0 )
        {
            Temper = PREHEAT_1_TEMP_BED;
        }
        else
        {
            Temper = PREHEAT_1_TEMP_HOTEND;
        }
    }

    S32 Cycle = 5;
    if ( Arg->HasC )
        Cycle = Arg->C;

//    #if DISABLED(BUSY_WHILE_HEATING)
//        KEEPALIVE_STATE(NOT_BUSY);
//    #endif

    //将状态设置成自动调整状态
    if ( LineMsgQueue )
    {
        struct MsgLine_t LineMsg = {0};
        LineMsg.MsgType = SPECIAL_MSG;
        strcpy(LineMsg.Info, MSG_PID_AUTOTUNE);
        if ( LineMsgQueue )
            CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
    }

    PidAutoTune(Temper, Index, Cycle, Arg->U);

    //将状态恢复成原来的状态
    if ( LineMsgQueue )
    {
        struct MsgLine_t LineMsg = {0};
        LineMsg.MsgType = SPECIAL_MSG;
        LineMsg.Finished = 1;
        strcpy(LineMsg.Info, "");
        if ( LineMsgQueue )
            CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
    }

    return 1;
}

S32 GcodeM303Reply(VOID *ReplyResult, S8 *Buff)
{

    return 1;
}


/**
 * PID Autotuning (M303)
 *
 * Alternately heat and cool the nozzle, observing its behavior to
 * determine the best PID values to achieve a stable temperature.
 * Needs sufficient heater power to make some overshoot at target
 * temperature to succeed.
 */
VOID PidAutoTune(const FLOAT Target, const S32 HeaterIndex, const S8 Cycles, const BOOL SetResult)
{
    if ( HeaterIndex < -1 || HeaterIndex >= HOTENDS )
        return;

    const BOOL IsBed = (HeaterIndex == H_BED);
    CrHeaterManager* Heater = NULL;
    if ( IsBed )
    {
        Heater = BedHeater;
    }
    else
    {
        Heater = HotEndArr[HeaterIndex];
    }

    if ( !Heater )
    {
        printf("PID auto tune failed\n");
        return ;
    }

    FLOAT CurTemp = 0.0;
    S32  CycleTemp = 0;
    BOOL Heating = true;

    S64 NextTempMs = CrGetSystemTimeMilSecs(), T1 = NextTempMs, T2 = NextTempMs;
    S64 HighT = 0, LowT = 0;

    S64 Bias, D;
    struct PidAttr_t TunePid = { 0, 0, 0 };
    FLOAT MaxT = 0, MinT = 10000;

    U16 WatchTempPeriod = IsBed ? WATCH_BED_TEMP_PERIOD : WATCH_TEMP_PERIOD;
    U8 WatchTempIncrease = IsBed ? WATCH_BED_TEMP_INCREASE : WATCH_TEMP_INCREASE;
    U8 Hysteresis = IsBed ? TEMP_BED_HYSTERESIS : TEMP_HYSTERESIS;
    FLOAT WatchTempTarget = Target - float(WatchTempIncrease + Hysteresis + 1);
    U64 TempChangeMs = NextTempMs + WatchTempPeriod * 1000;
    FLOAT NextWatchTemp = 0.0;
    BOOL Heated = false;   //已经加热

    //TERN_(HAS_AUTO_FAN, next_auto_fan_check_ms = next_temp_ms + 2500UL);  //暂时不使用

    S16 HotendMaxTemper = HEATER_0_MAXTEMP; //获取索引所指向的加热头的最大温度
    U16 MaxTargetTemper = IsBed ? U16(BED_MAX_TARGET) : (U16(HEATER_0_MAXTEMP) - U16(HOTEND_OVERSHOOT));

    if (Target > MaxTargetTemper )
    {
        printf("%s\n", STR_PID_TEMP_TOO_HIGH);
        //TERN_(EXTENSIBLE_UI, ExtUI::onPidTuning(ExtUI::result_t::PID_TEMP_TOO_HIGH));
        return;
    }

    printf("%s\n", STR_PID_AUTOTUNE_START);

    //关闭所有的加热器
    DisableAllHeaters();

    //SHV(bias = d = (MAX_BED_POWER) >> 1, bias = d = (PID_MAX) >> 1);   //
    //设置占空比开始加热
    if ( IsBed )
    {
        Bias = D = (MAX_BED_POWER) >> 1;
    }
    else
    {
        Bias = D = (PID_MAX) >> 1;
    }

    //设置加热器的占空比
    struct HeaterManagerAttr_t Attr = Heater->CrHeaterMagagerGetAttr();
    Attr.HeaterAttr.Duty = Bias;
    Heater->CrHeaterMagagerSetAttr(Attr);


//  #if ENABLED(PRINTER_EVENT_LEDS)
//    const float start_temp = GHV(temp_bed.celsius, temp_hotend[heater].celsius);
//    LEDColor color = ONHEATINGSTART();
//  #endif

//  TERN_(NO_FAN_SLOWING_IN_PID_TUNING, adaptive_fan_slowing = false);   //此处和热保护有关

    // PID Tuning loop
    BOOL wait_for_heatup = true; // Can be interrupted with M108 (设置再带等待加热)
    while (wait_for_heatup)
    {
        U64 Ms = CrGetSystemTimeMilSecs();

        // Get the current temperature and constrain it
        struct HeaterManagerAttr_t Attr = Heater->CrHeaterMagagerGetAttr();
        CurTemp = Attr.CurTemper; //GHV(temp_bed.celsius, temp_hotend[heater].celsius);
        if ( MaxT < CurTemp )
            MaxT = CurTemp;

        if ( MinT > CurTemp )
            MinT = CurTemp;

        #if HAS_AUTO_FAN
            if ( Ms >= next_auto_fan_check_ms )    //每2.5s进行一次检查
            {
                checkExtruderAutoFans();
                next_auto_fan_check_ms = Ms + 2500UL;
            }
        #endif

        if (Heating && CurTemp > Target)   //加热到超过目标温度
        {
            if ( Ms >= (T2 + 5000UL) )
            {
                Heating = false;
                //设置占空比
                struct HeaterManagerAttr_t Attr = Heater->CrHeaterMagagerGetAttr();
                Attr.HeaterAttr.Duty = (Bias - D) >> 1;
                Heater->CrHeaterMagagerSetAttr(Attr);
                T1 = Ms;
                HighT = T1 - T2;
                MaxT = Target;
            }
        }

        if (!Heating && CurTemp < Target)   //不加热直到温度小于目标温度
        {
            if ( Ms >= (T1 + 5000UL) )
            {
                Heating = true;
                T2 = Ms;
                LowT = T2 - T1;
                if (CycleTemp > 0)
                {
                    const long max_pow =  IsBed ? MAX_BED_POWER : PID_MAX;
                    Bias += (D * (HighT - LowT)) / (LowT + HighT);
                    LIMIT(Bias, 20, max_pow - 20);
                    D = (Bias > max_pow >> 1) ? max_pow - 1 - Bias : Bias;

                    printf("%s: %d %s: %d %s: %.2f %s:%.2f" ,STR_BIAS, Bias, STR_D_COLON, D, STR_T_MIN, MinT, STR_T_MAX, MaxT);
                    if (CycleTemp > 2)
                    {
                        const float Ku = (4.0f * D) / (float(M_PI) * (MaxT - MinT) * 0.5f),
                                    Tu = float(LowT + HighT) * 0.001f,
                                    pf = IsBed ? 0.2f : 0.6f,
                                    df = IsBed ? 1.0f / 3.0f : 1.0f / 8.0f;

                        printf(" %s: %.2f %s: %.2f", STR_KU, Ku, STR_TU, Tu);
                        if ( IsBed )
                        {
                                // Do not remove this otherwise PID autotune won't work right for the bed!
                              TunePid.Kp = Ku * 0.2f;
                              TunePid.Ki = 2 * TunePid.Kp / Tu;
                              TunePid.Kd = TunePid.Kp * Tu / 3;
                              printf("\n%s", " No overshoot"); // Works far better for the bed. Classic and some have bad ringing.
                              printf(" %s: %.2f %s: %.2f %s: %.2f\n", STR_KP, TunePid.Kp, STR_KI, TunePid.Ki, STR_KD, TunePid.Kd);
                        }
                        else
                        {
                              TunePid.Kp = Ku * pf;
                              TunePid.Kd = TunePid.Kp * Tu * df;
                              TunePid.Ki = 2 * TunePid.Kp / Tu;
                              printf("\n%s", STR_CLASSIC_PID);
                              printf(" %s: %.2f %s: %.2f %s: %.2f\n", STR_KP, TunePid.Kp, STR_KI, TunePid.Ki, STR_KD, TunePid.Kd);
                        }
                    }
                }

                struct HeaterManagerAttr_t Attr = Heater->CrHeaterMagagerGetAttr();
                Attr.HeaterAttr.Duty = (Bias + D) >> 1;
                Heater->CrHeaterMagagerSetAttr(Attr);

                CycleTemp++;
                MinT = Target;
            }
        }

        if ( CurTemp > (Target + MAX_OVERSHOOT_PID_AUTOTUNE) )
        {
              printf("%s\n", STR_PID_TEMP_TOO_HIGH);
              //TERN_(EXTENSIBLE_UI, ExtUI::onPidTuning(ExtUI::result_t::PID_TEMP_TOO_HIGH));
              break;
        }

        // Report heater states every 2 seconds
        if ( Ms >= NextTempMs )
        {
            //print_heater_states(IsBed ? active_extruder : heater);
            if ( IsBed )
            {
                struct HeaterManagerAttr_t Attr = Heater->CrHeaterMagagerGetAttr();
                if ( HotEndArr[0] )
                {
                    struct HeaterManagerAttr_t Attr1 = Heater->CrHeaterMagagerGetAttr();
                    printf("T: %.2f /%.2f B: %.2f /%.2f @:%d @B:%d\n", Attr1.CurTemper, Attr1.TargetTemper
                            , Attr.CurTemper, Attr.TargetTemper, Attr1.HeaterAttr.Duty, Attr.HeaterAttr.Duty);
                }
                else
                {
                    printf("T: %.2f /%.2f B: %.2f /%.2f @:%d @B:%d\n", 0.0, 0.0
                                                , Attr.CurTemper, Attr.TargetTemper, 0, Attr.HeaterAttr.Duty);
                }
            }
            else
            {
                struct HeaterManagerAttr_t Attr = Heater->CrHeaterMagagerGetAttr();
                if ( HotEndArr[0] )
                {
                    struct HeaterManagerAttr_t Attr1 = Heater->CrHeaterMagagerGetAttr();
                    printf("T: %.2f /%.2f B: %.2f /%.2f @:%d @B:%d\n", Attr1.CurTemper, Attr1.TargetTemper
                        , Attr.CurTemper, Attr.TargetTemper, Attr1.HeaterAttr.Duty, Attr.HeaterAttr.Duty);
                }
                else
                {
                    printf("T: %.2f /%.2f B: %.2f /%.2f @:%d @B:%d\n", 0.0, 0.0
                        , Attr.CurTemper, Attr.TargetTemper, 0, Attr.HeaterAttr.Duty);
                }
            }

            NextTempMs = Ms + 2000UL;

            // Make sure heating is actually working
            if (!Heated)
            {
                if (CurTemp > NextWatchTemp)
                {                   // Over the watch temp?
                    NextWatchTemp = CurTemp + WatchTempIncrease; // - set the next temp to watch for
                    TempChangeMs = Ms + WatchTempPeriod * 1000;     // - move the expiration timer up
                    if (CurTemp > WatchTempTarget)
                        Heated = true;  // - Flag if target temperature reached
//                    else if ( Ms >= TempChangeMs)                   // Watch timer expired
//                        _temp_error(heater, str_t_heating_failed, GET_TEXT(MSG_HEATING_FAILED_LCD));
                }
//                else if ( CurTemp < (Target - (MAX_OVERSHOOT_PID_AUTOTUNE)) ) // Heated, then temperature fell too far?
//                    _temp_error(heater, str_t_thermal_runaway, GET_TEXT(MSG_THERMAL_RUNAWAY));
            }
        } // every 2 seconds


        if ( (Ms - ( T1 < T2 ? T1 : T2)) > (MAX_CYCLE_TIME_PID_AUTOTUNE * 60L * 1000L))
        {
              //TERN_(DWIN_CREALITY_LCD, Popup_Window_Temperature(0));
              //TERN_(EXTENSIBLE_UI, ExtUI::onPidTuning(ExtUI::result_t::PID_TUNING_TIMEOUT));
              printf("%s\n", STR_PID_TIMEOUT);
              break;
        }

        if (CycleTemp > Cycles && CycleTemp > 2)
        {
              printf("%s\n", STR_PID_AUTOTUNE_FINISHED);
              if ( IsBed )
              {
                  printf("#define DEFAULT_bedKp %.2f\n", TunePid.Kp);
                  printf("#define DEFAULT_bedKi %.2f\n", TunePid.Ki);
                  printf("#define DEFAULT_bedKd %.2f\n", TunePid.Kd);
              }
              else
              {
                  printf("#define DEFAULT_Kp %.2f\n", TunePid.Kp);
                  printf("#define DEFAULT_Ki %.2f\n", TunePid.Ki);
                  printf("#define DEFAULT_Kd %.2f\n", TunePid.Kd);
              }

              // Use the result? (As with "M303 U1")
              if ( SetResult )
              {
                  struct HeaterManagerAttr_t Attr = Heater->CrHeaterMagagerGetAttr();
                  Attr.PidAttr.Kp = TunePid.Kp;
                  Attr.PidAttr.Kd = scalePID_d(TunePid.Kd);
                  Attr.PidAttr.Ki = scalePID_i(TunePid.Ki);
                  Heater->CrHeaterMagagerSetAttr(Attr);
              }
        }
    }

    DisableAllHeaters();

    return;
}


VOID DisableAllHeaters()
{
    if ( BedHeater )
    {
        struct HeaterManagerAttr_t Attr = BedHeater->CrHeaterMagagerGetAttr();
        Attr.HeaterAttr.Duty = 0;
        BedHeater->CrHeaterMagagerSetAttr(Attr);
    }

    S8 i;
    for ( i = 0; i < (HOTENDS); i++ )
    {
        if ( !HotEndArr[i] )
            continue;

        struct HeaterManagerAttr_t Attr = HotEndArr[i]->CrHeaterMagagerGetAttr();
        Attr.HeaterAttr.Duty = 0;
        HotEndArr[i]->CrHeaterMagagerSetAttr(Attr);
    }

    return;
}

VOID checkExtruderAutoFans()
{

}

VOID TemperError(heater_ind_t Index, S8 *ErrMsg, S8 *LcdShowMsg)
{
    if ( Index < -1 || Index >= HOTENDS || !ErrMsg || !LcdShowMsg )
        return ;

    printf("Error:%s", ErrMsg);
    if ( Index >= 0 )
    {
        printf("%d\n", Index);
    }
    else
    {
        printf("%s\n", STR_HEATER_BED);
    }

    DisableAllHeaters();

    //在界面上显示


    return;
}

