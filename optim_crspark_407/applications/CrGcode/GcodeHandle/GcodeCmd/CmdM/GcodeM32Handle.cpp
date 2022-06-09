#include "GcodeM32Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "Common/Include/CrSleep.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrPrinter.h"
#include "CrModel/CrSdCard.h"

#define CMD_M32  "M32"
#define FILENAME_END_MODIFIER '#'
#define LENG_FILENAME "gcode"

S32 GcodeM32Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M32) != 0 )  //传进来的命令不是M18或M84命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    S32 Offset = strlen(CMD_M32) + sizeof(S8); //加1是越过分隔符
    struct ArgM32_t *CmdArg = (struct ArgM32_t *)Arg;

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
            case OPT_P:
            {
                CmdArg->HasP = true;
                if ( OptLen > 1 ) CmdArg->P = atoi( Opt + 1 );
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atoi( Opt + 1 );

                break;
            }
            case OPT_NON:
            {
                CmdArg->HasStr = true;
                if ( OptLen >2 )
                {
                    if( *(Opt + OptLen - 1) == FILENAME_END_MODIFIER  )
                    {
                        *(Opt + OptLen - 1) = '\0';
                    }

                    S8* Index = strrchr(Opt, '.');
                    if ( Index )
                    {
                        if ( strcmp(Index + 1, LENG_FILENAME) != 0 )
                        {
                            *Index = '\0';
                            sprintf(CmdArg->Str, "%s.%s", Opt + 1, LENG_FILENAME);
                        }
                        else
                        {
                            strcpy(CmdArg->Str, Opt + 1);
                        }
                    }
                }
                break;
            }
            default:   /*关闭所有电机*/
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    if ( !CmdArg->HasS )
    {
        CmdArg->HasS = true;
        CmdArg->S = 0;
    }

    return 1;
}

S32 GcodeM32Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !Printer || !MotionModel || !SdCardModel || !GcodeModel )
        return 0;

    struct ArgM32_t *Param = (struct ArgM32_t *)ArgPtr;
    if ( !(Param->HasS) || !(Param->HasStr) || (Param->S < 0) || (strlen(Param->Str) <= 0) )
        return 1;

    if ( (Printer->GetPrintStatus() == PRINTING) || (Printer->GetPrintStatus() == RESUME_PRINT)  )
    {
        GcodeModel->StartWaiting();

        //等待运动完成
        while ( MotionModel->IsMotionMoveFinished() )
        {
            CrM_Sleep(5);
        }

        Printer->StopPrint();
        GcodeModel->CrCmdQueueClear();
        CrM_Sleep(5);
        GcodeModel->CrCmdQueueClear();
        GcodeModel->StopWaiting();
    }

    if ( Printer->GetPrintStatus() != STOP_PRINT )
    {
        Printer->StopPrint();
        GcodeModel->StopWaiting();
        GcodeModel->CrCmdQueueClear();
    }

    return SdCardModel->SetPrintFile(Param->Str, Param->S);
}

S32 GcodeM32Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
