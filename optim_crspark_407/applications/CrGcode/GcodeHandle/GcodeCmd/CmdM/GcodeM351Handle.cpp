#include "GcodeM351Handle.h"
#include <stdlib.h>
#include <string.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "GcodeM350Handle.h"
#include "CrMotion/StepperApi.h"

#define CMD_M351           "M351"

S32 GcodeM351Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0] != GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M351) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M351) + sizeof(S8); // 加1是越过分隔符
    struct ArgM351_t *CmdArg = (struct ArgM351_t *)Arg;
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) ) break;

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )
        {
            case OPT_B:
            {
                CmdArg->HasB = true;
                if ( OptLen > 1 ) CmdArg->B = atoi( Opt + 1 );
                break;
            }
            case OPT_E:
            {
                CmdArg->HasE = true;
                if ( OptLen > 1 ) CmdArg->E = atoi( Opt + 1 );
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atoi( Opt + 1 );
                break;
            }
            case OPT_X:
            {
                CmdArg->HasX = true;
                if ( OptLen > 1 ) CmdArg->X = atof( Opt + 1 );
                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = true;
                if ( OptLen > 1 ) CmdArg->Y = atof( Opt + 1 );
                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = true;
                if ( OptLen > 1 ) CmdArg->Z = atof( Opt + 1 );
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}


S32 GcodeM351Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM351_t *Arg = (struct ArgM351_t *)ArgPtr;

    if (Arg->HasS)
    {
        switch (Arg->S)
        {
          case 1:
          {
              if ( Arg->HasX )
                  MicrostepMs(X, Arg->X, -1, -1);

              if ( Arg->HasY )
                  MicrostepMs(Y, Arg->Y, -1, -1);

              if ( Arg->HasZ )
                  MicrostepMs(Z, Arg->Z, -1, -1);

              if ( Arg->HasE )
                  MicrostepMs(E, Arg->E, -1, -1);

              if ( Arg->HasB )
              {
                  if ( AxisNum > 5 )
                      MicrostepMs((Axis_t)4, Arg->B, -1, -1);
              }

              break;
          }
          case 2:
          {
             if ( Arg->HasX )
                MicrostepMs(X, -1, Arg->X, -1);

             if ( Arg->HasY )
                MicrostepMs(Y, -1, Arg->Y, -1);

             if ( Arg->HasZ )
                MicrostepMs(Z, -1, Arg->Z, -1);

             if ( Arg->HasE )
                MicrostepMs(E, -1, Arg->E, -1);

             if ( Arg->HasB )
             {
                if ( AxisNum > 5 )
                    MicrostepMs((Axis_t)4, -1, Arg->B, -1);
             }

             break;
          }
          case 3:
          {
            if ( Arg->HasX )
               MicrostepMs(X, -1, -1, Arg->X);

            if ( Arg->HasY )
               MicrostepMs(Y, -1, -1, Arg->Y);

            if ( Arg->HasZ )
               MicrostepMs(Z, -1, -1, Arg->Z);

            if ( Arg->HasE )
               MicrostepMs(E, -1, -1, Arg->E);

            if ( Arg->HasB )
            {
               if ( AxisNum > 5 )
                   MicrostepMs((Axis_t)4, -1, -1, Arg->B);
            }

            break;
          }
       }
    }

    SteppersMicrostepReadings();


    return 1;
}

S32 GcodeM351Reply(VOID *ReplyResult, S8 *Buff)
{
    return 1;
}
