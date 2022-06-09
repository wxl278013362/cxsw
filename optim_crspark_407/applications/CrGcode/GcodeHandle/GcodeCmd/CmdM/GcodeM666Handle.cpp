#include "GcodeM666Handle.h"
#include <stdlib.h>
#include <string.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"

#define CMD_M666           "M666"


FLOAT DeltaEndstopAdj[AxisNum] = {0.0f};

S32 GcodeM666Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len < 1 || CmdBuff[0] != GCODE_CMD_M )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M666) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M666) + sizeof(S8); //加1是越过分隔符
    union ArgM666_t *CmdArg = (union ArgM666_t *)Arg;

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
            case OPT_X:
            {
                CmdArg->EndStop.HasX = true;
                if ( OptLen > 1 ) CmdArg->EndStop.X = atof( Opt + 1 );

                break;
            }
            case OPT_Y:
            {
                CmdArg->EndStop.HasY = true;
                if ( OptLen > 1 ) CmdArg->EndStop.Y = atof( Opt + 1 );

                break;
            }
            case OPT_Z:
            {
                CmdArg->EndStop.HasZ = true;
                if ( OptLen > 1 ) CmdArg->EndStop.Z = atof(Opt + 1);

                break;
            }
            case OPT_S:
            {
                if ( OptLen > 1 ) CmdArg->EndStop.S = atoi(Opt + 1);

                break;
            }
            default:   /*关闭所有电机*/
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    return 1;
}


S32 GcodeM666Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    union ArgM666_t *CmdArg = (union ArgM666_t *)ArgPtr;
#if DELTA
    printf(">>> M666\n");
    if ( CmdArg->Delta.HasX )
    {
        if (CmdArg->Delta.X * Z_HOME_DIR <= 0)
            DeltaEndstopAdj[X] = CmdArg->Delta.X;
        printf("delta_endstop_adj[%d] = %f\n", X, DeltaEndstopAdj[X]);
    }

    if ( CmdArg->Delta.HasY )
    {
        if (CmdArg->Delta.Y * Z_HOME_DIR <= 0)
            DeltaEndstopAdj[Y] = CmdArg->Delta.Y;
        printf("delta_endstop_adj[%d] = %f\n", Y, DeltaEndstopAdj[Y]);
    }

    if ( CmdArg->Delta.HasZ )
    {
        if (CmdArg->Delta.Z * Z_HOME_DIR <= 0)
            DeltaEndstopAdj[Z] = CmdArg->Delta.Z;
        printf("delta_endstop_adj[%d] = %f\n", Z, DeltaEndstopAdj[Z]);
    }
    printf("<<< M666\n");

#elif HAS_EXTRA_ENDSTOPS
    #if X_DUAL_ENDSTOPS
        if ( CmdArg->EndStop.HasX )
            X2EndstopAdj = CmdArg->EndStop.X;
    #endif

    #if Y_DUAL_ENDSTOPS
        if ( CmdArg->EndStop.HasY )
            Y2EndstopAdj = CmdArg->EndStop.Y;
    #endif

    #if Z_TRIPLE_ENDSTOPS
        if ( CmdArg->EndStop.HasZ )
        {
            if ( !CmdArg->EndStop.S || (CmdArg->EndStop.S == 2) )
                Z2EndstopAdj = CmdArg->EndStop.Z;

            if ( !CmdArg->EndStop.S || (CmdArg->EndStop.S == 3) )
                Z3EndstopAdj = CmdArg->EndStop.Z;
        }
    #elif Z_MULTI_ENDSTOPS
        if ( CmdArg->EndStop.HasZ )
            Z2EndstopAdj = CmdArg->EndStop.Z;
    #endif

    if (  (!CmdArg->EndStop.HasX) && (!CmdArg->EndStop.HasY) && (!CmdArg->EndStop.HasZ) )
    {
        printf("Dual Endstop Adjustment (mm): ");
        #if X_DUAL_ENDSTOPS
          printf(" X2:%.2f", X2EndstopAdj);
        #endif
        #if Y_DUAL_ENDSTOPS
          printf(" Y2:%.2f", Y2EndstopAdj);
        #endif
        #if Z_MULTI_ENDSTOPS
          printf(" Z2:%.2f", Z2EndstopAdj);
        #endif
        #if Z_TRIPLE_ENDSTOPS
          printf(" Z3:%.2f", Z3EndstopAdj);
        #endif
        printf("\n");
    }

#endif

    return 1;
}

S32 GcodeM666Reply(VOID *ReplyResult, S8 *Buff)
{

    return 1;
}
