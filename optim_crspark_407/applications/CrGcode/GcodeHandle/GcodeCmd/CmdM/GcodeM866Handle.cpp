#include "GcodeM866Handle.h"
#include "CrModel/CrMotion.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include "CrInc/CrMsgType.h"
#include <stdio.h>
#include <stdlib.h>

#define CMD_M866 ("M866")

S32 GcodeM866Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Offset = strlen(CMD_M866) + 1;
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M866, strlen(CMD_M866)) != 0 )
    {
        return CrErr;
    }

    struct ArgM866_t *CmdArg = (struct ArgM866_t *)Arg;
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M866 has no param opt = %s\n", Opt);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_A:
                CmdArg->Common->HasA = true;
                if ( OptLen == 1 )
                {
                    CmdArg->Common->HasValueA = false;
                }
                else
                {
                    CmdArg->Common->HasValueA = false;
                    CmdArg->Common->A = atoi(Opt + 1);
                }
                break;

            case OPT_I:
                CmdArg->Common->HasI = true;
                if ( OptLen == 1 )
                {
                    CmdArg->Common->HasValueI = false;
                }
                else
                {
                    CmdArg->Common->HasValueI = true;
                    CmdArg->Common->I = atoi(Opt + 1);
                }
                break;

            case OPT_R:
                CmdArg->HasR = true;
                if ( OptLen == 1 )
                {
                    CmdArg->R = 0;
                }
                else
                {
                    CmdArg->R = atoi(Opt + 1);
                }
                break;

            case OPT_X:
                CmdArg->Common->HasX = true;
                if ( OptLen == 1 )
                {
                    CmdArg->Common->X = 0;
                }
                else
                {
                    CmdArg->Common->X = atoi(Opt + 1);
                }
                break;

            case OPT_Y:
                CmdArg->Common->HasY = true;
                if ( OptLen == 1 )
                {
                    CmdArg->Common->Y = 0;
                }
                else
                {
                    CmdArg->Common->Y = atoi(Opt + 1);
                }
                break;

            case OPT_Z:
                CmdArg->Common->HasZ = true;
                if ( OptLen == 1 )
                {
                    CmdArg->Common->Z = 0;
                }
                else
                {
                    CmdArg->Common->Z = atoi(Opt + 1);
                }
                break;

            case OPT_E:
                CmdArg->Common->HasE = true;
                if ( OptLen == 1 )
                {
                    CmdArg->Common->E = 0;
                }
                else
                {
                    CmdArg->Common->E = atoi(Opt + 1);
                }
                break;

            default:
                printf("Gcode M866 has no define opt = %s\n", CmdBuff);
                break;
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}


S32 GcodeM866Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    struct ArgM866_t *Param = (struct ArgM866_t *)ArgPtr;
    if ( Parse(Param->Common) )
    {
        return CrErr;
    }

    BOOL hasR = Param->HasR;
    if ( I2CPEIdx == 0xFF )
    {
        for ( int i = X; i <= E; i++ )
        {
            BOOL HasAxis = false;
            switch( i )
            {
                case X:
                    if ( Param->Common->HasX )
                    {
                        HasAxis = true;
                    }
                    break;

                case Y:
                    if ( Param->Common->HasY )
                    {
                        HasAxis = true;
                    }
                    break;

                case Z:
                    if ( Param->Common->HasZ )
                    {
                        HasAxis = true;
                    }
                    break;

                case E:
                    if ( Param->Common->HasE )
                    {
                        HasAxis = true;
                    }
                    break;

                default:
                    break;
            }
            if( !I2CPEAnyaxis || HasAxis )
            {
                const U8 Idx = IdxFromAxis((enum Axis_t)i);
                if ( (S8)Idx >= 0 )
                {
                    if ( hasR )
                    {
                        ResetErrorCount(Idx, (enum Axis_t)i);
                    }
                    else
                    {
                        ReportErrorCount(Idx, (enum Axis_t)i);
                    }
                }
            }
        }
    }
    else if ( hasR )
    {
        ResetErrorCount(I2CPEIdx, EncoderGetAxis(I2CPEIdx));
    }
    else
    {
        ReportErrorCount(I2CPEIdx, EncoderGetAxis(I2CPEIdx));
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM866Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


