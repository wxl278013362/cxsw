#include "GcodeM861Handle.h"
#include "../../GcodeType.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define CMD_M861  "M861"

BOOL PassesTest(const BOOL Report,I2CPositionEncoder_t *Encoder)
{
    if ( !Encoder )
    {
        return false;
    }

    if ( Report )
    {
        if ( Encoder->H != I2CPE_MAG_SIG_GOOD )
        {
            printf("Warning. ");
        }
        if ( Encoder->EncoderAxis <= 3 )
        {
            printf("%c",AxisCodes[Encoder->EncoderAxis]);
        }

        //serial_ternary
        S8 S1[] = "magnetic strip ";
        S8 S2[] = "encoder ";
        printf(" axis %s",(Encoder->H == I2CPE_MAG_SIG_BAD) ? S1 : S2);

        switch(Encoder->H)
        {
            case I2CPE_MAG_SIG_GOOD:
            case I2CPE_MAG_SIG_MID:
            {
                S8 S3[] = "good";
                S8 S4[] = "fair";
                printf("passes test; field strength %s.\n",(Encoder->H == I2CPE_MAG_SIG_GOOD) ? S3 : S4);
                break;
            }
            default:
                printf("not detected!\n");
        }
    }
    return (Encoder->H == I2CPE_MAG_SIG_GOOD || Encoder->H == I2CPE_MAG_SIG_MID);
}

VOID ReportStatus(const S8 Idx)
{
    if ( !(Idx >= 0 && Idx <= I2CPE_ENCODER_CNT - 1) )
    {
        return;
    }

    printf("Encoder %d: \n",Idx);
    GetRawCount(Encoders[Idx]);
    PassesTest(true,&Encoders[Idx]);
}

S32 GcodeM861Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd,CMD_M861) != 0 )    //判断是否是M861命令
    {
        return 0;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M861) + sizeof(S8);
    struct ArgM861_t *CmdArg = (struct ArgM861_t *)Arg;

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
            printf("Gcode M861 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_A:
            {
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
            }
            case OPT_E:
            {
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
            }
            case OPT_I:
            {
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
            }
            case OPT_X:
            {
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
            }
            case OPT_Y:
            {
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
            }
            case OPT_Z:
            {
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
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

S32 GcodeM861Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return CrErr;
    }

    struct ArgM861_t *Param = (struct ArgM861_t *)ArgPtr;
    if ( Parse(Param->Common) )
    {
        return CrErr;
    }

    if ( I2CPEIdx == 0xFF )
    {
        for ( int i = X; i <= E; i++ )   //从0到3分别代表X,Y,Z,E四个轴
        {
            BOOL HasAxis = false;
            switch( i )
            {
                case X:
                {
                    if ( Param->Common->HasX )
                        HasAxis = true;
                    break;
                }
                case Y:
                {
                    if ( Param->Common->HasY )
                        HasAxis = true;
                    break;
                }
                case Z:
                {
                    if ( Param->Common->HasZ )
                        HasAxis = true;
                    break;
                }
                case E:
                {
                    if ( Param->Common->HasE )
                        HasAxis = true;
                    break;
                }
                default:
                    break;
            }
            if( !I2CPEAnyaxis || HasAxis )
            {
                const U8 Idx = IdxFromAxis((enum Axis_t)i);
                if ( (S8)Idx >= 0 )
                {
                    ReportStatus(Idx);
                }
            }
        }
    }
    else
    {
        ReportStatus(I2CPEIdx);
    }
    return GCODE_EXEC_OK;
}
S32 GcodeM861Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

