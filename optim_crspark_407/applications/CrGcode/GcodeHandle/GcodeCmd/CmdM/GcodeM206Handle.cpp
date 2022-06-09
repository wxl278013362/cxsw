#include "GcodeM206Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"

#define CMD_M206  "M206"

S32 GcodeM206Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M205
    if ( strcmp(Cmd, CMD_M206) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M206) + sizeof(S8); // ��1��Խ���ָ���
    struct ArgM206_t *CmdArg = (struct ArgM206_t *)Arg;

    // [B<��s>] [E<jerk>] [J<deviation>] [S<units/s>] [T<units/s>] [X<jerk>] [Y<jerk>] [Z<jerk>]
    while ( Offset < Len )
    {
        S8 Opt[20] = {0}; // ��������
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) ) break;

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 ) // û�в�����
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        // ������������
        switch ( Opt[0] ) // ��������
        {
            case OPT_P:
                CmdArg->HasP = true;
                if ( OptLen > 1 ) CmdArg->P = atof(Opt + 1);
                break;

            case OPT_T:
                CmdArg->HasT = true;
                if ( OptLen > 1 ) CmdArg->T = atof(Opt + 1);
                break;

            case OPT_X:
                CmdArg->HasX = true;
                if ( OptLen > 1 ) CmdArg->X = atof(Opt + 1);
                break;

            case OPT_Y:
                CmdArg->HasY = true;
                if ( OptLen > 1 ) CmdArg->Y = atof(Opt + 1);
                break;

            case OPT_Z:
                CmdArg->HasZ = true;
                if ( OptLen > 1 ) CmdArg->Z = atof(Opt + 1);
                break;

            default:
                printf("Warn:unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //�˴���1�Ƿָ���
    }

    return 1;
}

S32 GcodeM206Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM206_t *Param = (struct ArgM206_t *)ArgPtr;
    if ( Param->HasP )
    {
        printf(">> P:%f \n", Param->P);
    }

    if ( Param->HasT )
    {
        printf(">> T:%f/100 \n", Param->T);
    }

    if ( Param->HasX )
    {
        printf(">> X:%f \n", Param->X);
        MotionModel->SetAxisHomeOffset(X, Param->X);
    }

    if ( Param->HasY )
    {
        printf(">> Y:%f \n", Param->Y);
        MotionModel->SetAxisHomeOffset(Y, Param->Y);
    }

    if ( Param->HasZ )
    {
        printf(">> Z:%f \n", Param->Z);
        MotionModel->SetAxisHomeOffset(Z, Param->Z);
    }


    struct MotionCoord_t Coord =  MotionModel->GetLatestCoord();
    printf("X:%f Y:%f Z:%f E:%f ", Coord.CoorX, Coord.CoorY, Coord.CoorZ, Coord.CoorE);   //逻辑位置

    struct StepperPos_t PosX = {0};
    struct StepperPos_t PosY = {0};
    struct StepperPos_t PosZ = {0};
    StepperGetPos(&PosX,X);
    StepperGetPos(&PosY,Y);
    StepperGetPos(&PosZ,Z);

    printf("Count X:%d Y:%d Z:%d\n", PosX.CurPosition, PosY.CurPosition, PosZ.CurPosition);  //部署统计


    return 1;
}

S32 GcodeM206Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
