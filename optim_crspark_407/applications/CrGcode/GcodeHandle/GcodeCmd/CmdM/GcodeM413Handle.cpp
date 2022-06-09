#include "GcodeM413Handle.h"
#include <stdlib.h>
#include <string.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "Common/Include/CrSleep.h"

#define CMD_M413           "M413"

BOOL RecoverEnable = false;
BOOL RecoverValid = false;
BOOL RecoverExist = false;


S32 GcodeM413Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0] != GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M413) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M413) + sizeof(S8); //加1是越过分隔符
    struct ArgM413_t *CmdArg = (struct ArgM413_t *)Arg;

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
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atoi( Opt + 1 );
                break;
            }
            case OPT_R:
            {
                CmdArg->HasR = true;
                break;
            }
            case OPT_W:
            {
                CmdArg->HasW = true;
                break;
            }
            case OPT_P:
            {
                CmdArg->HasP = true;
                break;
            }
            case OPT_L:
            {
                CmdArg->HasL = true;
                break;
            }
            case OPT_O:
            {
                CmdArg->HasO = true;
                break;
            }
            case OPT_E:
            {
                CmdArg->HasE = true;
                break;
            }
            case OPT_V:
            {
                CmdArg->HasV = true;
                break;
            }
            default:
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}


S32 GcodeM413Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM413_t *CmdArg = (struct ArgM413_t *)ArgPtr;
    if ( CmdArg->HasS )
    {
        RecoverEnable = CmdArg->S;
    }
    else
    {
        printf("Power-loss recovery %s\n", RecoverEnable ? "ON" : "OFF");
    }

#ifdef  DEBUG_POWER_LOSS_RECOVERY
    if ( CmdArg->HasR || CmdArg->HasL )
    {
        //recovery.load();    //获取断电恢复的信息
    }

    if ( CmdArg->HasW )
    {
        //recovery.save(true);  //保存断电恢复的信息

    }

    if ( CmdArg->HasP )
    {
        //recovery.purge();   //清除已保存的断电恢复信息


    }

#ifdef   POWER_LOSS
    #if POWER_LOSS_PIN
        if ( CmdArg->HasO )
        {
            //recovery._outage();

        }
    #endif
#endif

    if ( CmdArg->HasE )
    {
        printf("%s\n", RecoverExist ? "PLR Exists" : "No PLR");
    }

    if ( CmdArg->HasV )
    {
        printf("%s\n", RecoverValid ? "Valid" : "Invalid");
    }
#endif

    return 1;
}

S32 GcodeM413Reply(VOID *ReplyResult, S8 *Buff)
{

    return 1;
}
