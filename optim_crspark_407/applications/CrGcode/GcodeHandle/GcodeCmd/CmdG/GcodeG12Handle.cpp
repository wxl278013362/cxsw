#include "GcodeG12Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"

#define CMD_G12            "G12"

S32 GcodeG12Parse(const S8 *CmdBuff, VOID *Arg)
{
    /*G1 的操作有 E F X Y E */
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_G) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_G12) != 0 )  //传进来的命令不是G1命令
    {
        return 0;
    }
    
    S32 Offset = strlen(CMD_G12) + sizeof(S8); //加1是越过分隔符
    struct ArgG12_t *CmdArg = (struct ArgG12_t *)Arg;

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
            printf("Gcode G0 has no param opt = %s\n", CmdBuff);
        }
        
        switch ( Opt[0] )  //判断命令
        {
            case OPT_P:
             {
                 CmdArg->HasP = 1;
                 if ( OptLen > 1 )
                 {
                     CmdArg->P = atoi(Opt + 1);
                 }
                 break;
             }
             case OPT_S:
             {
                 CmdArg->HasS = 1;
                 if ( OptLen > 1 )
                 {
                     CmdArg->S = atoi(Opt + 1);
                 }
                 break;
             }
             case OPT_T:
             {
                 CmdArg->HasT = 1;
                 if ( OptLen > 1 )
                 {
                     CmdArg->T = atoi(Opt + 1);
                 }
                 break;
             }
             case OPT_R:
             {
                 CmdArg->HasR = 1;
                 if ( OptLen > 1 )
                 {
                     CmdArg->R = atof(Opt + 1);
                 }
                 break;
             }
            default:
            {
                printf("Gcode G12 has no define opt = %s\n", CmdBuff);
                break;
            }
        }
        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }
    return 1;
}

S32 GcodeG12Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult)
        return 0;
    
    struct ArgG12_t *Param = (struct ArgG12_t *)ArgPtr;
    


    return GCODE_EXEC_OK;
}

S32 GcodeG12Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


