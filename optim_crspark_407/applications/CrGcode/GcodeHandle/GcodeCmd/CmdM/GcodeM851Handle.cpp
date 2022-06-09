#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "GcodeM851Handle.h"
#include "CrInc/CrMsgType.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "CrMotion/CrMotionPlanner.h"
#include "Common/Include/CrGlobal.h"
#include "Common/Include/CrTime.h"

#define CMD_M851  "M851"

/******额外定义的变量和函数***********/
#define HAS_PROBE_XY_OFFSET 1
#define Z_PROBE_OFFSET_RANGE_MIN -20
#define Z_PROBE_OFFSET_RANGE_MAX 20

XYFloat_t OffsetXY = {0};                //Probe的XY差值
XYZFloat_t Probeoffset = {0};            //
/******额外定义的变量和函数***********/


S32 GcodeM851Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd,CMD_M851) != 0 )    //判断是否是M851命令
    {
        return 0;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M851) + sizeof(S8);
    struct ArgM851_t *CmdArg = (struct ArgM851_t *)Arg;

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
            printf("Gcode M851 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_X:
            {
                CmdArg->HasX = true;
                if ( OptLen > 1 )
                {
                    CmdArg->X = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->HasX = false;
                }
                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Y = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->HasY = false;
                }
                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Z = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->HasZ = false;
                }
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }
    return 1;
}
S32 GcodeM851Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM851_t *Param = (struct ArgM851_t *)ArgPtr;
    //1.不带参数的流程
    if ( Param->HasX && Param->HasY && Param->HasZ )
    {
#if HAS_PROBE_XY_OFFSET
        printf("Probe Offset X%f Y%f Z%f\n",OffsetXY.X,OffsetXY.Y,Probeoffset.Z);
#else
        printf("Probe Offset X0 Y0 Z%f\n",ProbeOffset.Z);
#endif
        return 0;
    }

    //2.开始修改
    XYZFloat_t Offs = Probeoffset;
    BOOL Ok = true;

    if ( Param->HasX )
    {
        FLOAT X = Param->X;
#if HAS_PROBE_XY_OFFSET
        if ( X >= -(BED_LENGTH) && X <= BED_LENGTH )
        {
            Offs.X = X;
        }
        else
        {
            printf("?X out of range (-%d to %d).\n",BED_LENGTH,BED_LENGTH);
            Ok = false;
        }
#else
        if ( X )
        {
            printf("?X must be 0 (NOZZLE_AS_PROBE).\n");
        }
#endif
    }

    if ( Param->HasY )
    {
        FLOAT Y = Param->Y;
#if HAS_PROBE_XY_OFFSET
        if ( Y >= -(BED_WIDTH) && Y <= BED_WIDTH )
        {
            Offs.Y = Y;
        }
        else
        {
            printf("?Y out of range (-%d to %d).\n",BED_WIDTH,BED_WIDTH);
            Ok = false;
        }
#else
        if ( Y )
        {
            printf("?Y must be 0 (NOZZLE_AS_PROBE).\n");
        }
#endif
    }

    if ( Param->HasZ )
    {
        FLOAT Z = Param->Z;
        if ( Z >= Z_PROBE_OFFSET_RANGE_MIN && Z <= Z_PROBE_OFFSET_RANGE_MAX )
        {
            Offs.Z = Param->Z;
        }
        else
        {
            printf("?Z out of range (-%d to %d).\n",Z_PROBE_OFFSET_RANGE_MIN,Z_PROBE_OFFSET_RANGE_MAX);
            Ok = false;
        }
    }

    //3.保存
    if ( Ok )
    {
        Probeoffset = Offs;
    }

    return 1;
}
S32 GcodeM851Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
