#include "GcodeM504Handle.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../GcodeHandle.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "Common/Include/CrEepromApi.h"
#include "Common/Include/CrSleep.h"

#define CMD_M504 ("M504")

S32 GcodeM504Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M504, strlen(CMD_M504)) != 0)
        return 0;

    S32 Offset = strlen(CMD_M504) + sizeof(S8); //加1是越过分隔符
    struct ArgM504_t *CmdArg = (struct ArgM504_t *)Arg;

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
            case OPT_W:
            {
                CmdArg->HasW = true;
                if ( OptLen > 1 ) CmdArg->W = atoi( Opt + 1 );
                break;
            }
            case OPT_R:
            {
                CmdArg->HasR = true;
                if ( OptLen > 1 ) CmdArg->R = atoi( Opt + 1 );
                break;
            }
            case OPT_T:
            {
                CmdArg->HasT = true;
                if ( OptLen > 1 ) CmdArg->T = atoi( Opt + 1 );
                break;
            }
            case OPT_V:
            {
                CmdArg->HasV = true;
                if ( OptLen > 1 ) CmdArg->V = atoi( Opt + 1 );
                break;
            }
            default:
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM504Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;


#ifdef MARLIN_DEV_MODE
    struct ArgM504_t *CmdArg = (struct ArgM504_t *)ArgPtr;
    const BOOL DoWrite = CmdArg->HasW;
    if ( DoWrite || CmdArg->HasR )
    {
        U8 Val = 0;
        if ( DoWrite )
        {
            S32 Addr = CmdArg->W;
        }
        else
        {
            S32 Addr = CmdArg->R;
        }

        if (DoWrite)
        {

            if ( CmdArg->HasV )
                Val = CmdArg->V;

            EepromStart();
            EepromSkip(Addr);
            Success = EepromWrite((S8 *)&Val, sizeof(Val));
            EepromFinish();

            printf("Wrote address %d with %d\n", Addr, int(Val));
        }
        else
        {
            if ( CmdArg->HasT  )
            {
                const int EndAddr = CmdArg->T;
                EepromStart();
                EepromSkip(Addr);
                while ( Addr <= EndAddr )
                {
                    EepromRead((S8 *)&Val, sizeof(Val));
                    printf("0x%x:%x\n", Addr,Val);
                    EepromSkip(sizeof(Val));
                    Addr += sizeof(Val);
                    CrM_Sleep(10);
                }
                EepromFinish();

                printf("\n");
            }
            else
            {
                EepromStart();
                EepromSkip(Addr);
                EepromRead((S8 *)&Val, sizeof(Val));
                EepromFinish();
                printf("Read address %d and got %d\n", Addr, Val);
            }

        }
        return;
      }
#endif

    if ( EepromValidate() )
        printf("EEPROM OK\n");

    return GCODE_EXEC_OK;
}

S32 GcodeM504Reply(VOID *ReplyResult, S8 *Buff)
{

    return 0;
}
