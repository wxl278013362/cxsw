#include "GcodeM864Handle.h"
#include "CrModel/CrMotion.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include "CrInc/CrMsgType.h"
#include <stdio.h>
#include <stdlib.h>

#define CMD_M864 ("M864")

S32 GcodeM864Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Offset = strlen(CMD_M864) + 1;
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M864, strlen(CMD_M864)) != 0 )
    {
        return CrErr;
    }

    struct ArgM864_t *CmdArg = (struct ArgM864_t *)Arg;
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
            printf("Gcode M864 has no param opt = %s\n", Opt);
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

            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueS = false;
                }
                else
                {
                    CmdArg->HasValueS = true;
                    CmdArg->S = atoi(Opt + 1);
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
                printf("Gcode M864 has no define opt = %s\n", CmdBuff);
                break;
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return 0;
}


S32 GcodeM864Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    struct ArgM864_t *Param = (struct ArgM864_t *)ArgPtr;
    if ( Parse(Param->Common) )
    {
        return CrErr;
    }

    U8 newAddress;
    if ( !I2CPEAddr )
    {
        I2CPEAddr = I2CPE_PRESET_ADDR_X;
    }

    if ( Param->HasS )
    {
        if ( !Param->HasValueS )
        {
            printf("?S seen, but no address specified! [30-200]");
            return CrErr;
        }

        newAddress = Param->S;
        if ( !WITHIN(newAddress, 30, 200) )
        {
            printf("?New address out of range. [30-200]");
            return CrErr;
        }
    }
    else if ( !I2CPEAnyaxis )
    {
        printf("?You must specify S or [XYZE].");
        return CrErr;
    }
    else
    {
        if ( Param->Common->HasX )
        {
            newAddress = I2CPE_PRESET_ADDR_X;
        }
        else if ( Param->Common->HasY )
        {
            newAddress = I2CPE_PRESET_ADDR_Y;
        }
        else if ( Param->Common->HasZ )
        {
            newAddress = I2CPE_PRESET_ADDR_Z;
        }
        else if ( Param->Common->HasE )
        {
            newAddress = I2CPE_PRESET_ADDR_E;
        }
        else
        {
            return CrErr;
        }
    }

    printf("Changing module at address:%d  to address 0x%x\n", I2CPEAddr, newAddress);

    ChangeModuleAddress(I2CPEAddr, newAddress);

    return GCODE_EXEC_OK;
}

S32 GcodeM864Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

