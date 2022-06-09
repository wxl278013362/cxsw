#include "GcodeM73Handle.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrMsgQueue/SharedQueue/CrSharedMsgQueue.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"


static CrMsg_t PrintInfoQueue = NULL;
#define CMD_M73  "M73"
#define PROGRESS_SCALE (100)

VOID GcodeM73Init(VOID *Model)
{
    PrintInfoQueue = ViewRecvQueueOpen((S8 *)PRINT_INFO_REPORT_QUEUE, sizeof(struct PrintInfoMsg_t));
}

S32 GcodeM73Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M710
    if ( strcmp(Cmd, CMD_M73) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M73) + sizeof(S8); // 加1是越过分隔符
    struct ArgM73_t *CmdArg = (struct ArgM73_t *)Arg;

    while ( Offset < Len )
    {
        S8 Opt[20] = {0}; // 操作参数
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) ) break;

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 ) // 没有操作码
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        // 解析出操作码
        switch ( Opt[0] ) // 操作类型
        {
            case OPT_P:
                CmdArg->HasP = true;
                if( OptLen > 1 )
                {
                    S8 *Index = strstr(Opt, "E");
                    S8 *IndexSub = strstr(Opt, "e");
                    if ( Index )
                    {
                        *Index = '\0';
                        FLOAT Progress = atof(Opt + 1) * PROGRESS_SCALE;
                        if ( Progress > 100 )
                            Progress /= PROGRESS_SCALE;
                        CmdArg->P = Progress;

                    }
                    else if ( IndexSub )
                    {
                        *IndexSub = '\0';
                        FLOAT Progress = atof(Opt + 1) * PROGRESS_SCALE;
                        if ( Progress > 100 )
                            Progress /= PROGRESS_SCALE;
                        CmdArg->P = Progress;
                    }
                    else
                    {
                        CmdArg->P = (int)atoi(Opt + 1);
                    }
                }
                break;

            default:
                printf("Unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}

S32 GcodeM73Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !PrintInfoQueue)
        return 0;

    struct PrintInfoMsg_t Info;
    struct ArgM73_t *CmdArg = (struct ArgM73_t *)ArgPtr;
    if ( CmdArg->HasP )
    {
        Info.Progress = CmdArg->P;
        Info.Status = PRINTING;
        Info.TimeElapsed = 0;
        CrMsgSend(PrintInfoQueue, (S8 *)&Info, sizeof(Info), 1);
    }

    return 1;
}

S32 GcodeM73Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
