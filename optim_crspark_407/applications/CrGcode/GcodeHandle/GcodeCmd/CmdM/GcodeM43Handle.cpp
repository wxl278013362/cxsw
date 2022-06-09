#include "GcodeM43Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"

#ifndef CMD_M43
#define CMD_M43 ("M43")
#endif

S32 GcodeM43Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S8 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M43) != 0 )  //传进来的命令不是G0命令
        return 0;

    S8 Offset = strlen(CMD_M43) + sizeof(S8); //加1是越过分隔符
    struct ArgM43_t *CmdArg = (struct ArgM43_t *)Arg;

    S8 Opt[20] = {0};
    S8 OptLen = 0;

    while ( Offset < Len )
    {
        memset(Opt, 0, sizeof(Opt));
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
            break;    //获取参数失败

        OptLen = strlen(Opt);

        switch ( Opt[0] )
        {
            case OPT_T:
                CmdArg->HasT = true;
            break;

            case OPT_E:

                CmdArg->HasE = true;

                if (OptLen > 1)
                    CmdArg->E = (BOOL)atoi(Opt + 1);
            break;

            case OPT_I:
                CmdArg->HasI = true;
            break;

            case OPT_W:

                CmdArg->HasP = true;

                if (OptLen > 1)
                    CmdArg->P = atoi(Opt + 1);
            break;

            case OPT_P:

                CmdArg->HasP = true;

                if (OptLen > 1)
                    CmdArg->P = atoi(Opt + 1);
            break;

            case OPT_S:

                CmdArg->HasS = true;

                if (OptLen > 1)
                {
                    CmdArg->S = atoi(Opt + 1);
                }
            break;

            case OPT_R:

                CmdArg->HasR = true;

                if (OptLen > 1)
                    CmdArg->R = atoi(Opt + 1);
            break;
            case OPT_L:

                CmdArg->HasL = true;

                if (OptLen > 1)
                    CmdArg->L = atoi(Opt + 1);
            break;

        }
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

/*
const bool ignore_protection = parser.boolval('I');
const int repeat = parser.intval('R', 1),
          start = PARSED_PIN_INDEX('S', 0),
          end = PARSED_PIN_INDEX('L', NUM_DIGITAL_PINS - 1),
          wait = parser.intval('W', 500);
*/

static inline S32 TogglePins(BOOL I, const S32 Repeat, const S32 Start, const S32 End, const S32 WaitTime)
{

    for ( S32 Var = Start; Var < End; ++Var )
    {

    }

    return 1;
}

S32 GcodeM43Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM43_t *Param = (struct ArgM43_t *)ArgPtr;
    if ( Param->HasT ) return TogglePins(((Param->HasI) ? true: false), \
                              (Param->HasR) ? Param->R : 1, \
                              (Param->HasS) ? Param->S : 0, \
                              (Param->HasL) ? Param->L : 255,\
                              (Param->HasW) ? Param->W : 500);  /*default delay */

    if ( Param->E )
    {
        /**
         * enable or disable Endstop and return
         * */
        goto __exit;
    }

    if ( Param->S )
    {
        /**
         * Run servo probe test and return
         * */
        goto __exit;
    }

    if ( Param->HasW )
    {
        /**
         * Watch until click, M108, or reset
         * */
    }
    else
    {
        /**
         * Report current state of selected pin(s)
         * */
    }

__exit:
    return 1;
}

S32 GcodeM43Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
