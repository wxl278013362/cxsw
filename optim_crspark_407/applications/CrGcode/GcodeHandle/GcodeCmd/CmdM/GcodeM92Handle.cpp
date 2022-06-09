#include "GcodeM92Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "CrMotion/StepperApi.h"
#include "CrInc/CrMsgType.h"

#define CMD_M92  "M92"

S32 GcodeM92Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是M
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M92) != 0 )  //传进来的命令不是M18或M84命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    S32 Offset = strlen(CMD_M92) + sizeof(S8); //加1是越过分隔符
    struct ArgM92_t *CmdArg = (struct ArgM92_t *)Arg;

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
            case OPT_E:
            {
                CmdArg->HasE = true;
                if ( OptLen > 1 ) CmdArg->E = atof( Opt + 1 );
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
            case OPT_T:
            {
                CmdArg->HasT = true;
                if ( OptLen > 1 )
                {
                    CmdArg->T = atoi( Opt + 1 );
                }
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

S32 GcodeM92Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM92_t *Param = (struct ArgM92_t *)ArgPtr;

    if ( !Param->HasT || (Param->T < 0) )
        return 1;

    if ( !(Param->HasE) && !(Param->HasX) && !(Param->HasY) && !(Param->HasZ) )
    {
        struct StepperAttr_t Attr;
        printf("M92 ");
        if ( StepperGetAttr(&Attr, X) >= 0 )
            printf("X%d ", Attr.StepsPerMm);

        if ( StepperGetAttr(&Attr, Y) >= 0 )
            printf("Y%d ", Attr.StepsPerMm);

        if ( StepperGetAttr(&Attr, Z) >= 0 )
            printf("Z%d ", Attr.StepsPerMm);

        if ( StepperGetAttr(&Attr, E) >= 0 )
            printf("E%d ", Attr.StepsPerMm);

        printf("\n");

        return 1;
    }

    if ( Param->HasX )
    {
        struct StepperAttr_t Attr;
        if ( StepperGetAttr(&Attr, X) >= 0 )
        {
            Attr.StepsPerMm = Param->X;
            StepperSetAttr(&Attr, X);
        }
    }

    if ( Param->HasY )
    {
        struct StepperAttr_t Attr;
        if ( StepperGetAttr(&Attr, Y) >= 0 )
        {
            Attr.StepsPerMm = Param->Y;
            StepperSetAttr(&Attr, Y);
        }
    }

    if ( Param->HasZ )
    {
        struct StepperAttr_t Attr;
        if ( StepperGetAttr(&Attr, Z) >= 0 )
        {
            Attr.StepsPerMm = Param->Z;
            StepperSetAttr(&Attr, Z);
        }
    }

    if ( Param->HasE )
    {
        struct StepperAttr_t Attr;
        if ( StepperGetAttr(&Attr, E) >= 0 )
        {
            Attr.StepsPerMm = Param->E;
            StepperSetAttr(&Attr, E);
        }
    }

    return 1;
}

S32 GcodeM92Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
