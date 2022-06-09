#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "GcodeM404Handle.h"
#include "Common/Include/CrTime.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrGcode/GcodeHandle/GcodeType.h"


#define CMD_M404 "M404"

/******额外定义的变量和函数***********/

FLOAT NominalFialmentWidth = 0;     //标准料丝宽度(M104)
FLOAT NominalVolumetricArea = 0;    //标准横截面积
/******额外定义的变量和函数***********/

S32 GcodeM404Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd,CMD_M404) != 0 )    //判断是否是M404命令
    {
        return 0;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M404) + sizeof(S8);
    struct ArgM404_t *CmdArg = (struct ArgM404_t *)Arg;

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
            printf("Gcode M404 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_W:
            {
                CmdArg->HasW = true;
                if ( OptLen > 1 )
                {
                    CmdArg->W = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->HasW = false;
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

S32 GcodeM404Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM404_t *Param = (struct ArgM404_t *)ReplyResult;
    if ( Param->HasW )
    {
        NominalFialmentWidth = Param->W;
        NominalVolumetricArea = M_PI * powf(NominalFialmentWidth * 0.5, 2);
    }
    else
    {
        printf("Filament dia (nominal mm):%f\n",NominalFialmentWidth);
    }
    return 1;
}

S32 GcodeM404Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
