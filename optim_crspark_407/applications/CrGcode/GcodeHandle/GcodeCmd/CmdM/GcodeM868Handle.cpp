#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "GcodeM860Handle.h"
#include "GcodeM868Handle.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrModel/CrMotion.h"

#define CMD_M868  "M868"

VOID SetEcThreshold(const S8 Idx, const BOOL NewThreshold, const enum Axis_t Axis)
{
    if ( !WITHIN(Idx, 0, I2CPE_ENCODER_CNT - 1) )
        return;

    Encoders[Idx].EcThreshold = NewThreshold;
    printf("Error correct threshold for %c axis set to %f mm.\n",AxisCodes[Axis],NewThreshold);
}

VOID GetEcThreshold(const S8 Idx, const enum Axis_t Axis)
{
    if ( !WITHIN(Idx, 0, I2CPE_ENCODER_CNT - 1) )
        return;

    const FLOAT Threshold = Encoders[Idx].EcThreshold;
    printf("Error correct threshold for %c axis is %f mm.\n",AxisCodes[Axis],Threshold);
}
/*********额外定义的变量和函数************/
S32 GcodeM868Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd,CMD_M868) != 0 )    //判断是否是M868命令
    {
        return 0;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M868) + sizeof(S8);
    struct ArgM868_t *CmdArg = (struct ArgM868_t *)Arg;

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
            printf("Gcode M868 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_A:
                CmdArg->Common->HasA = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Common->A = atoi(Opt + 1);
                    CmdArg->Common->HasValueA = true;
                }
                else
                {
                    CmdArg->Common->HasValueA = false;
                }
                break;

            case OPT_E:
                CmdArg->Common->HasE = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Common->E = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->Common->E = 0;
                }
                break;

            case OPT_I:
                CmdArg->Common->HasI = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Common->I = atoi(Opt + 1);
                    CmdArg->Common->HasValueI = true;
                }
                else
                {
                    CmdArg->Common->HasValueI = false;
                }
                break;

            case OPT_T:
                CmdArg->HasT = true;
                if ( OptLen > 1 )
                {
                    CmdArg->T = atof(Opt + 1);
                    CmdArg->HasValueT = true;
                }
                else
                {
                    CmdArg->HasValueT = false;
                }

            case OPT_X:
                CmdArg->Common->HasX = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Common->X = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->Common->X = 0;
                }
                break;

            case OPT_Y:
                CmdArg->Common->HasY = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Common->Y = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->Common->Y = 0;
                }
                break;

            case OPT_Z:
                CmdArg->Common->HasZ = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Common->Z = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->Common->Z = 0;
                }
                break;

            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

S32 GcodeM868Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return CrErr;
    }

    struct ArgM868_t *Param = (struct ArgM868_t *)ArgPtr;
    if ( Parse(Param->Common) )
    {
        return CrErr;
    }

    FLOAT NewThreshold = 0;
    if ( Param->HasT && Param->HasValueT )
    {
        NewThreshold = Param->T;
    }
    else
    {
        NewThreshold = -9999;
    }

    if ( I2CPEIdx == 0xFF )
    {
        for ( int i = X; i <= E; i++ )
        {
            BOOL HasAxis = false;
            switch( i )
            {
                case X:
                    if ( Param->Common->HasX )
                        HasAxis = true;
                    break;

                case Y:
                    if ( Param->Common->HasY )
                        HasAxis = true;
                    break;

                case Z:
                    if ( Param->Common->HasZ )
                        HasAxis = true;
                    break;

                case E:
                    if ( Param->Common->HasE )
                        HasAxis = true;
                    break;

                default:
                    break;
            }
            if ( !I2CPEAnyaxis || HasAxis )
            {
                U8 Idx = IdxFromAxis((enum Axis_t)i);
                if ( (S8)Idx >= 0 )
                {
                    if ( NewThreshold != -9999 )
                    {
                        SetEcThreshold(Idx, NewThreshold, Encoders[Idx].EncoderAxis);
                    }
                    else
                    {
                        GetEcThreshold(Idx, Encoders[Idx].EncoderAxis);
                    }
                }
            }
        }
    }
    else if ( NewThreshold != -9999 )
    {
        SetEcThreshold(I2CPEIdx,NewThreshold,Encoders[I2CPEIdx].EncoderAxis);
    }
    else
    {
        GetEcThreshold(I2CPEIdx, Encoders[I2CPEIdx].EncoderAxis);
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM868Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
