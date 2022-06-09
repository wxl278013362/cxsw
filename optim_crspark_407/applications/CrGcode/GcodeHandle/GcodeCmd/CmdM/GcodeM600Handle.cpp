#include "GcodeM600Handle.h"
#include <stdlib.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "Common/Include/CrSleep.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrPrinter.h"

#define CMD_M600           "M600"

BOOL IsWait = false;
VOID ReleaseM600Wait()
{
    IsWait = false;
}


S32 GcodeM600Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0] != GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M600) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M600) + sizeof(S8); //加1是越过分隔符
    struct ArgM600_t *CmdArg = (struct ArgM600_t *)Arg;

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
                CmdArg->HasB = true;
                if ( OptLen > 1 ) CmdArg->B = atoi( Opt + 1 );
                break;
            }
            case OPT_E:
            {
                CmdArg->HasE = true;
                if ( OptLen > 1 ) CmdArg->E = atof( Opt + 1 );

                break;
            }
            case OPT_T:
            {
                CmdArg->HasT = true;
                if ( OptLen > 1 ) CmdArg->T = atoi( Opt + 1 );

                break;
            }
            case OPT_L:
            {
                CmdArg->HasL = true;
                if ( OptLen > 1 ) CmdArg->L = atof(Opt + 1);

                break;
            }
            case OPT_U:
            {
                CmdArg->HasU = true;
                if ( OptLen > 1 ) CmdArg->U = atof( Opt + 1 );
                break;
            }
            case OPT_X:
            {
                CmdArg->HasX = true;
                if ( OptLen > 1 ) CmdArg->X = atof( Opt + 1 );

                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = true;
                if ( OptLen > 1 ) CmdArg->Y = atof( Opt + 1 );

                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = true;
                if ( OptLen > 1 ) CmdArg->Z = atof(Opt + 1);

                break;
            }
            case OPT_R:
            {
                CmdArg->HasR = true;
                if ( OptLen > 1 ) CmdArg->R = atof(Opt + 1);

                break;
            }
            default:   /*关闭所有电机*/
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    if ( !(CmdArg->HasB) )
    {
        CmdArg->HasB = true;
        CmdArg->B = -1;
#ifdef FILAMENT_CHANGE_ALERT_BEEPS
        CmdArg->B = FILAMENT_CHANGE_ALERT_BEEPS;
#endif
    }

    return 1;
}


S32 GcodeM600Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel || !Printer )
        return 0;

    struct ArgM600_t *CmdArg = (struct ArgM600_t *)ArgPtr;
    S8 ExtruderIndex = 0;   //当前活动的挤出机
    S8 LastExtruderIndex = 0;  //在切换前保存当前活动的挤出机索引
    if ( CmdArg->HasT )
    {
        if ( (CmdArg->T >= 0) && (CmdArg->T < (AxisNum - E)) )
        {
            ExtruderIndex = CmdArg->T;
        }
        else
        {
            printf("%s %s %d", CMD_M600,  "Invalid extruder", CmdArg->T);
        }
    }

    if ( ExtruderIndex < 0 )
        return 0;


    //暂时没有考虑混合挤出机
#ifdef  DUAL_X_CARRIAGE
    //此处的功能暂时不实现

#endif

#ifdef HOME_BEFORE_FILAMENT_CHANGE
    GcodeHandle("G28", strlen("G28"), NULL);
#endif

#if EXTRUDERS > 1
    //切换工具， 暂时不实现
    if ( LastExtruderIndex != ExtruderIndex )
        ToolChange(LastExtruderIndex, false);   //此内容暂时未实现
#endif

    FLOAT Rectract = PAUSE_PARK_RETRACT_LENGTH;
    if ( CmdArg->HasE )
    {
        Rectract = CmdArg->E;
        if ( Rectract < 0 )
            Rectract = 0 - Rectract;
    }

    struct XYZFloat_t ParkPos = NOZZLE_PARK_POINT;
    if ( CmdArg->HasX )
        ParkPos.X = CmdArg->X;

    if ( CmdArg->HasY )
        ParkPos.Y = CmdArg->Y;

    if ( CmdArg->HasZ )
        ParkPos.Z = CmdArg->Z;

#if HAS_HOTEND_OFFSET
#ifndef DUAL_X_CARRIAGE
#ifndef DELTA
    //加上喷头的偏移量，暂时未实现（因为没有hotend offset）
    ParkPos += HotendOffset[ExtruderIndex];
#endif
#endif
#endif

    FLOAT UnloadLength = 0.0f, SlowLoadLength = 0.0f, FastLoadLength = 0.0f;
#ifdef MMU2_MENUS
    UnloadLength = 0.5f;
#else

    // Unload filament
    UnloadLength = 0.0;    //获取配置的参数 fc_settings[active_extruder].unload_length);
    if ( CmdArg->HasU )
    {
        UnloadLength = CmdArg->U;
        if ( UnloadLength < 0 )
            UnloadLength = 0 - UnloadLength;
    }

    // Slow load filament
    SlowLoadLength = FILAMENT_CHANGE_SLOW_LOAD_LENGTH;

    // Fast load filament
    FastLoadLength = 0.0f;   //获取配置的参数 fc_settings[active_extruder].load_length);
    if ( CmdArg->HasL )
    {
        FastLoadLength = CmdArg->L;
        if ( FastLoadLength < 0 )
            FastLoadLength = 0 - FastLoadLength;
    }
#endif

    S32 TemperCelsius = 0;
    if ( CmdArg->HasR )
        TemperCelsius = CmdArg->R;



    struct MotionCoord_t Coor = MotionModel->GetLatestCoord();
    struct FeedrateAttr_t Feedrate = MotionModel->GetMotionFeedRateAttr();
    struct MotionCoord_t DestCoor = Coor;
    DestCoor.CoorE -= Rectract;
    DestCoor.CoorX = ParkPos.X;
    DestCoor.CoorY = ParkPos.Y;
    DestCoor.CoorZ = ParkPos.Z;
    struct MotionGcode_t Pos;
    Pos.Coord = DestCoor;
    Pos.FeedRate = Feedrate.Feedrate;
    Pos.RetractFlag = true;
    MotionModel->PutMotionCoord(Pos);
    Printer->PauseHandle();   //暂停

#ifdef DUAL_X_CARRIAGE
    {

#ifdef MMU2_MENUS
        IsWait = true;
        while ( IsWait )
        {
            CrM_Sleep(5);
        }

        //恢复打印的内容

#else
        //执行等待并通知界面显示

        // 执行恢复操作
#endif
    }
#else

    {
#ifdef MMU2_MENUS
        IsWait = true;
        while ( IsWait )
        {
            CrM_Sleep(5);
        }

        //执行恢复打印的操作
#else
      //执行等待并通知界面显示

      // 执行恢复操作
#endif
    }
#endif

#if EXTRUDERS > 1
  // Restore toolhead if it was changed  (恢复切换前的)
  if (LastExtruderIndex != ExtruderIndex)
      ToolChange(LastExtruderIndex, false);
#endif

    //混合挤出机要恢复旧工具

    return 1;
}

S32 GcodeM600Reply(VOID *ReplyResult, S8 *Buff)
{

    return 1;
}
