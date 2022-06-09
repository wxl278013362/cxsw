#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GcodeM871Handle.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"

#define CMD_M871 "M871"

/******额外定义的变量和函数***********/
// Probe temperature calibration constants
#define PTC_SAMPLE_COUNT    10U
#define PTC_SAMPLE_RES      5
#define PTC_SAMPLE_START    30
#define PTC_SAMPLE_END      ((PTC_SAMPLE_START) + (PTC_SAMPLE_COUNT) * (PTC_SAMPLE_RES))

// Bed temperature calibration constants
#define BTC_SAMPLE_COUNT 10U
#define BTC_SAMPLE_RES      5
#define BTC_SAMPLE_START    60
#define BTC_SAMPLE_END      ((BTC_SAMPLE_START) + (BTC_SAMPLE_COUNT) * (BTC_SAMPLE_RES))

enum TempSensorId
{
    TsiProbe,
    TsiBed,
#ifdef USE_TEMP_EXT_COMPENSATION
    TsiExt,
#endif
    TsiCount
};
struct TempCalib_t
{
    U8      Measurements;
    S16     TempRes;
    S16     StartTemp;
    S16     EndTemp;
};

U8 CalibIdx = 0;

const struct TempCalib_t CaliInfo[TsiCount] = {
        { PTC_SAMPLE_COUNT, PTC_SAMPLE_RES, PTC_SAMPLE_START, PTC_SAMPLE_END }, // Probe
        { BTC_SAMPLE_COUNT, BTC_SAMPLE_RES, BTC_SAMPLE_START, BTC_SAMPLE_END } // Bed
        #ifdef USE_TEMP_EXT_COMPENSATION
         , { 20,  5, 180, 180 +  5 * 20 }                                        // Extruder
        #endif
};
S16 ProbeOffsetsZ[PTC_SAMPLE_COUNT] = {0};
S16 BedOffsetsZ[BTC_SAMPLE_COUNT] = {0};
#ifdef USE_TEMP_EXT_COMPENSATION
S16 ExtOffsetsZ[CaliInfo[2].Measurements] = {0};
#endif

S16 *SensorZOffsets[TsiCount] = {ProbeOffsetsZ,BedOffsetsZ
#ifdef USE_TEMP_EXT_COMPENSATION
, ExtOffsetsZ
#endif
};
VOID ClearOffset(enum TempSensorId Tsi)
{
    for ( int i = 0; i < CaliInfo[Tsi].Measurements; i++ )
    {
        SensorZOffsets[Tsi][i] = 0;
    }
    CalibIdx = 0;
}

VOID ClearAllOffsets()
{
    ClearOffset(TsiBed);
    ClearOffset(TsiProbe);
#ifdef USE_TEMP_EXT_COMPENSATION
    ClearOffset(TsiExt);
#endif
}

BOOL SetOffset(const TempSensorId Tsi, const U8 Idx, S16 Offset)
{
    if ( Idx >= CaliInfo[Tsi].Measurements )
    {
        return false;
    }
    SensorZOffsets[Tsi][Idx] = Offset;
    return true;
}

VOID PrintOffsets()
{
    for( int s = 0; s < TsiCount; s++ )
    {
         S16 Temp = CaliInfo[s].StartTemp;
         for(int i = -1; i < CaliInfo[s].Measurements; i++ )
         {
             if ( s == TsiBed )
             {
                 printf("Bed");
             }
             else
             {
#ifdef USE_TEMP_EXT_COMPENSATION
              if ( s == TSI_EXT  )
              {
                  printf("Extruder");
              }
              else
#endif
                  printf("Probe");
             }

             printf(" temp %dC; Offset: %f um\n",Temp,i < 0 ? 0.0f :SensorZOffsets[s][i]);

             Temp += CaliInfo[s].TempRes;
         }
    }
}
/******额外定义的变量和函数***********/

S32 GcodeM871Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd,CMD_M871) != 0 )    //判断是否是M871命令
    {
        return 0;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M871) + sizeof(S8);
    struct ArgM871_t *CmdArg = (struct ArgM871_t *)Arg;

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
            printf("Gcode M871 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_B:
            {
                CmdArg->HasB = true;
                if ( OptLen > 1 )
                {
                    CmdArg->B = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->B = 0;
                }
                break;
            }
            case OPT_E:
            {
                CmdArg->HasE = true;
                if ( OptLen > 1 )
                {
                    CmdArg->E = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->E = 0;
                }
                break;
            }
            case OPT_I:
            {
                CmdArg->HasI = true;
                if ( OptLen > 1 )
                {
                    CmdArg->I = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->HasI = false;
                }
                break;
            }
            case OPT_P:
            {
                CmdArg->HasP = true;
                if ( OptLen > 1 )
                {
                    CmdArg->P = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->P = 0;
                }
                break;
            }
            case OPT_R:
            {
                CmdArg->HasR = true;
                if ( OptLen > 1 )
                {
                    CmdArg->R = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->R = 0;
                }
                break;
            }
            case OPT_V:
            {
                CmdArg->HasV = true;
                if ( OptLen > 1 )
                {
                    CmdArg->V = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->HasV = false;
                }
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }
    return 1;
}

S32 GcodeM871Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM871_t *Param = (struct ArgM871_t *)ArgPtr;
    if ( Param->HasR )
    {
        //如果有R将所有的偏移值重置为默认值
        ClearAllOffsets();
        printf("Offsets reset to default.\n");
    }
    else if ( Param->HasB || Param->HasP || Param->HasE )
    {
        //有BPE任何一个参数的前提下取V或者I参数进行调整
        if ( !Param->HasV )
        {
            return 0;
        }
        S16 OffsetVal = Param->V;

        if ( !Param->HasI )
        {
            return 0;
        }
        S16 Idx = Param->I;

        TempSensorId Mod;

        if ( Param->HasB )
        {
            Mod = TsiBed;
        }
        else
        {
#ifdef USE_TEMP_EXT_COMPENSATION
        if ( Param->HasE )
        {
            Mod = TsiExt;
        }
        else
#endif
            Mod = TsiProbe;
        }
        if ( Idx > 0 && SetOffset(Mod,Idx - 1,OffsetVal) )
        {
            printf("Set value: %d\n",OffsetVal);
        }
        else
        {
            printf("!Invalid index. Failed to set value (note: value at index 0 is constant).\n");
        }
    }
    else
    {
        //RBPE四个参数都没有打印当前的偏移值
        PrintOffsets();
    }

    return 1;
}

S32 GcodeM871Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
