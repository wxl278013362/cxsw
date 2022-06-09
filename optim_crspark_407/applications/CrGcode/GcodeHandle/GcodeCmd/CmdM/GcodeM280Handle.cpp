#include "GcodeM280Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include "CrModel/CrServo.h"

#if HAS_SERVOS

#define CMD_M280 ("M280")

S32 GcodeM280Parse(const S8 *CmdBuff, VOID *Arg)
{
    /*M280 P[Index] S[Pos] P不可省略，S若省略则读取索引伺服位置*/
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M280, strlen(CMD_M280)) != 0 )
    {
        return 0;
    }

    struct ArgM280_t *CmdArg = (struct ArgM280_t *)Arg;
    S32 Offset = strlen(CMD_M280) + 1; //加1是越过分隔符
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
            printf("Gcode M280 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_P:
            {
                CmdArg->HasS = true;
                if ( WITHIN(atoi(Opt + 1), 0, NUM_SERVOS - 1) )
                {
                    CmdArg->S = atoi(Opt + 1);
                }
                else
                {
                    return CrErr;
                }
                break;
            }

            case OPT_S:
            {
                CmdArg->HasS = true;
                CmdArg->S = atoi(Opt + 1);
                break;
            }

            default:
            {
                printf("Gcode M280 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM280Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    struct ArgM280_t *Param = (struct ArgM280_t *)ArgPtr;
    if ( !Param->HasP )
    {
        return CrErr;
    }
    if ( !Param->HasS )
    {
        return Servos[Param->P].Read();       //read
    }
    else if ( Param->S == -1 )
    {
        Servos[Param->P].Detach();     //detach
    }
    else
    {
        Servos[Param->P].Move(Param->S);   //move
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM280Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

#endif
