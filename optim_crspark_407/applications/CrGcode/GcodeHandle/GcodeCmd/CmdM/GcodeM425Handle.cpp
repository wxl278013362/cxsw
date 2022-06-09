#include "GcodeM425Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "CrMotion/StepperApi.h"

#define CMD_M425  "M425"

S32 GcodeM425Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M425) != 0 )  //传进来的命令不是M18或M84命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    S32 Offset = strlen(CMD_M425) + sizeof(S8); //加1是越过分隔符
    struct ArgM425_t *CmdArg = (struct ArgM425_t *)Arg;

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
            case OPT_F:
            {
                CmdArg->HasF = true;
                if ( OptLen > 1 ) CmdArg->F = atof( Opt + 1 );
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atof( Opt + 1 );

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
                if ( OptLen > 1 ) CmdArg->Y = atof(Opt + 1);

                break;
            }
            case OPT_Z:
            {
                if ( OptLen > 1 )
                {
                    CmdArg->HasZ = true;
                    CmdArg->Z = atof(Opt + 1);
                }
                else
                {
                    CmdArg->HasZM = true;
                }

                break;
            }
            default:
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8);
    }

    return 1;
}

auto axis_can_calibrate = [](const uint8_t a) {  //定义函数指针
  switch (a) {
    default:
    case X: return AXIS_CAN_CALIBRATE(X);   //case的X表示轴的序号真实值时0，宏函数的参数X不是轴的编号，就表示字符X
    case Y: return AXIS_CAN_CALIBRATE(Y);
    case Z: return AXIS_CAN_CALIBRATE(Z);
  }
};

S32 GcodeM425Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM425_t *Arg = (struct ArgM425_t *)ArgPtr;
    bool NoArgs = true;
#if 0
    if ( AXIS_CAN_CALIBRATE(X) && Arg->HasX )
    {
        //等待运动结束

        backlash.distance_mm[X] = Arg->X;
        if ( fabs(Arg->X) <= 0.000001 )
        {
            backlash.distance_mm[X] = backlash.get_measurement(AxisEnum(X));
        }
        NoArgs = false;
    }

    if ( AXIS_CAN_CALIBRATE(Y) && Arg->HasY )
    {
        //等待运动结束


        backlash.distance_mm[Y] = Arg->Y;
        if ( fabs(Arg->X) <= 0.000001 )
        {
            backlash.distance_mm[Y] = backlash.get_measurement(AxisEnum(Y));
        }
        NoArgs = false;
    }

    if ( AXIS_CAN_CALIBRATE(Z) && (Arg->HasZ || Arg->HasZM) )
    {
        //等待运动结束



        if ( Arg->HasZ )
        {
            backlash.distance_mm[Z] = Arg->Z;
        }

        if (Arg->HasZM)   //他和hasz是互斥的
        {
            backlash.distance_mm[Z] = backlash.get_measurement(AxisEnum(Z));
        }

        NoArgs = false;
    }

    if ( Arg->HasF )
    {
        //等待运动完成


        backlash.set_correction(Arg->F);
        NoArgs = false;
    }

    #ifdef BACKLASH_SMOOTHING_MM
      if (Arg->HasS)
      {
          //等待运动结束


          backlash.smoothing_mm = Arg->S;
          NoArgs = false;
      }
    #endif

    if ( NoArgs )
    {
      printf("Backlash Correction ");
      if ( !backlash.correction )
      {
          printf("inactive:");
      }
      else
      {
          printf("active:");
      }

      printf("  Correction Amount/Fade-out:     F%0.1f(F1.0 = full, F0.0 = none)", backlash.get_correction());
      printf("  Backlash Distance (mm):        ");
      if ( axis_can_calibrate(X) )   //X表示轴的序号真实值时0
          printf(" X%f\n", backlash.distance_mm[X]); //X表示轴的序号真实值时0

      if ( axis_can_calibrate(Y) )  //Y表示轴的序号真实值时1
          printf(" Y%f\n", backlash.distance_mm[Y]); //Y表示轴的序号真实值时1

      if ( axis_can_calibrate(Z) )  //Z表示轴的序号真实值时2
          printf(" Z%f\n", backlash.distance_mm[Z]);  //Z表示轴的序号真实值时2

      #ifdef BACKLASH_SMOOTHING_MM
        printf("  Smoothing (mm):                 S%f\n", backlash.smoothing_mm);
      #endif

      #ifdef MEASURE_BACKLASH_WHEN_PROBING
        printf("  Average measured backlash (mm):");
        if (backlash.has_any_measurement())
        {
          if ( axis_can_calibrate(X) && backlash.has_measurement(AxisEnum(X)) )  //X表示轴的序号真实值时0
              printf(" X%f\n", backlash.get_measurement(AxisEnum(X)));

          if ( axis_can_calibrate(Y) && backlash.has_measurement(AxisEnum(Y)) )  //Y表示轴的序号真实值时1
              printf(" X%f\n", backlash.get_measurement(AxisEnum(Y)));

          if ( axis_can_calibrate(Z) && backlash.has_measurement(AxisEnum(Z)) )  //Z表示轴的序号真实值时2
              printf(" X%f\n", backlash.get_measurement(AxisEnum(Z)));

        }
        else
          printf(" (Not yet measured)");
        printf("\n");
      #endif
    }
#endif

    return 1;
}

S32 GcodeM425Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
