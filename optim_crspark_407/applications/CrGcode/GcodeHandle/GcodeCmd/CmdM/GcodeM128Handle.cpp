#include "GcodeM128Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"

#if HAS_HEATER_1

#define CMD_M128 ("M128")

U8 baricuda_e_to_p_pressure = 0;

S32 GcodeM128Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M128, strlen(CMD_M128)) != 0 )
    {
        return 0;
    }

    struct ArgM128_t *CmdArg = (struct ArgM128_t *)Arg;
    S32 Offset = strlen(CMD_M128) + 1; //加1是越过分隔符
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen == 0 )
        {
            printf("Gcode M128 has no param opt = %s\n", CmdBuff);  //命令仅为M128,设置默认值255
            CmdArg->HasS = false;
            CmdArg->S = 255;

            return GCODE_EXEC_OK;
        }
        else if ( OptLen == 1 && Opt[0] != OPT_S)   //非M128 S,不合法
        {
            return 0;
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_S:
            {
                CmdArg->HasS = true;
                CmdArg->S = Constrain(atoi(Opt + 1), 0, 255);
                break;
            }
            default:
            {
                printf("Gcode M128 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return 1;
}

S32 GcodeM128Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return 0;
    }

    struct ArgM128_t *Param = (struct ArgM128_t *)ArgPtr;
    baricuda_e_to_p_pressure = (int)Param->S;

    return 1;
}

S32 GcodeM128Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

#endif

