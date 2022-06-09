#include "GcodeM205Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrModel/CrMotion.h"

#define CMD_M205  "M205"

//static CrMotion *M205MotionModel = NULL;

//VOID GcodeM205Init(VOID *Model)
//{
//    M205MotionModel = (CrMotion *)Model;
//}

/* ����M205ָ��
   M205 [B<��s>] [E<jerk>] [J<deviation>] [S<units/s>] [T<units/s>] [X<jerk>] [Y<jerk>] [Z<jerk>]
 */
S32 GcodeM205Parse(const S8 *CmdBuff, VOID *Arg)
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
    if ( strcmp(Cmd, CMD_M205) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M205) + sizeof(S8); // ��1��Խ���ָ���
    struct ArgM205_t *CmdArg = (struct ArgM205_t *)Arg;

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
            case OPT_B:
                CmdArg->HasB = true;
                if ( OptLen > 1 ) CmdArg->B = atoi(Opt + 1);
                break;

            case OPT_E:
                CmdArg->HasE = true;
                if ( OptLen > 1 ) CmdArg->E = atof(Opt + 1);
                break;

            case OPT_J:
                CmdArg->HasJ = true;
                if ( OptLen > 1 ) CmdArg->J = atof(Opt + 1);
                break;

            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atof(Opt + 1);
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

S32 GcodeM205Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM205_t *Param = (struct ArgM205_t *)ArgPtr;

    /* [B<��s>] [E<jerk>] [J<deviation>] [S<units/s>] [T<units/s>] [X<jerk>] [Y<jerk>] [Z<jerk>]
     */


    if ( Param->HasB )  //unused
    {
        // ������С��ʱ�䣨΢�룩
        // TODO
        //printf(">> B:%d \n", (S32)Param->B);
    }

#ifdef JUNCTION_DEVIATION
    if ( Param->HasJ )
    {
        // ���ý��ƫ��
        // TODO
    }
#else
    if ( Param->HasJ )  //unused
    {
        //printf("Warn: JUNCTION_DEVIATION no defined \n");
    }
#endif

    if ( Param->HasS )
    {
        // ���ô�ӡʱ��С�����ٶȣ���λ/�룩
        //printf(">> S:%d \n", (S32)Param->S);
        struct FeedrateAttr_t FeedrateAttr = MotionModel->GetMotionFeedRateAttr();
        FeedrateAttr.VelMinXYZ = (S32)Param->S;
        MotionModel->SetMotionFeedRateAttr(FeedrateAttr);
    }

    if ( Param->HasT )
    {
        // �����н�ʱ��С�����ٶȣ���λ/�룩
        //printf(">> T:%d \n", (S32)Param->T);
        struct FeedrateAttr_t FeedrateAttr = MotionModel->GetMotionFeedRateAttr();
        FeedrateAttr.TravelVelMinXYZ = (S32)Param->T;
        MotionModel->SetMotionFeedRateAttr(FeedrateAttr);
    }

    struct PlanLine_t *Line = &LineBuff[LineBuffTail];
    if ( Param->HasX )
    {
        // ����X�����Jerk�ٶȣ���λ/�룩
        // TODO
        //printf(">> X:%d \n", (S32)Param->X);
        //struct StepperMotionAttr_t AttrX = MotionModel->GetSetpperMotion(StepperX);
        //AttrX.Jerk = (S32)Param->X;
        //MotionModel->SetStepperMotion(AttrX, StepperX);
        Line->Axis[X].MaxJerk = (S32)Param->X;
    }

    if ( Param->HasY )
    {
        // ����Y�����Jerk�ٶȣ���λ/�룩
        //printf(">> Y:%d \n", (S32)Param->Y);
        //struct StepperMotionAttr_t AttrY = MotionModel->GetSetpperMotion(StepperY);
        //AttrY.Jerk = (S32)Param->Y;
        //MotionModel->SetStepperMotion(AttrY, StepperY);
        Line->Axis[Y].MaxJerk = (S32)Param->Y;
    }

    if ( Param->HasZ )
    {
        // ����Z�����Jerk�ٶȣ���λ/�룩
        //printf(">> Z:%d \n", (S32)Param->Z);
        //struct StepperMotionAttr_t AttrZ = MotionModel->GetSetpperMotion(StepperZ);
        //AttrZ.Jerk = (S32)Param->Z;
        //MotionModel->SetStepperMotion(AttrZ, StepperZ);
        Line->Axis[Z].MaxJerk = (S32)Param->Z;
    }

    if ( Param->HasE )
    {
        // ����E�����Jerk�ٶȣ���λ/�룩
        //printf(">> E:%d/100 \n", (S32)(Param->E*100));
        //struct StepperMotionAttr_t AttrE = MotionModel->GetSetpperMotion(StepperE);
        //AttrE.Jerk = (S32)Param->E;
        //MotionModel->SetStepperMotion(AttrE, StepperE);
        Line->Axis[E].MaxJerk = (S32)Param->E;
    }

    return 1;
}

S32 GcodeM205Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
