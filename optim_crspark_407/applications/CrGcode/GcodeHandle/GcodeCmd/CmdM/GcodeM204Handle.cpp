#include "GcodeM204Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"

#define CMD_M204  "M204"

//static CrMotion *M204MotionModel = NULL;
//
//VOID GcodeM204Init(VOID *Model)
//{
//    M204MotionModel = (CrMotion *)Model;
//}

/* ����M204ָ��
   M204 [P<accel>] [R<accel>] [S<accel>] [T<accel>]
 */
S32 GcodeM204Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M204
    if ( strcmp(Cmd, CMD_M204) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M204) + sizeof(S8); // ��1��Խ���ָ���
    struct ArgM204_t *CmdArg = (struct ArgM204_t *)Arg;

    // [P<accel>] [R<accel>] [S<accel>] [T<accel>]
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

            case OPT_R:
                CmdArg->HasR = true;
                if ( OptLen > 1 ) CmdArg->R = atof(Opt + 1);
                break;

            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atof(Opt + 1);
                break;

            case OPT_T:
                CmdArg->HasT = true;
                if ( OptLen > 1 ) CmdArg->T = atof(Opt + 1);
                break;

            default:
                printf("Warn:unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //�˴���1�Ƿָ���
    }

    return 1;
}

S32 GcodeM204Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM204_t *Param = (struct ArgM204_t *)ArgPtr;

    /* [P<accel>] [R<accel>] [S<accel>] [T<accel>]
     */

    if ( Param->HasS )
    {
        struct StepperMotionAttr_t AttrXyze = MotionModel->GetSetpperMotion(StepperX);
        AttrXyze.Acceleration = Param->S;
        MotionModel->SetStepperMotion(AttrXyze, StepperX);

        struct FeedrateAttr_t FeedrateAttr = MotionModel->GetMotionFeedRateAttr();
        FeedrateAttr.TravelAccelXYZ = (S32)Param->T;
        MotionModel->SetMotionFeedRateAttr(FeedrateAttr);
    }

    if ( Param->HasP )
    {
        struct StepperMotionAttr_t AttrXyze = MotionModel->GetSetpperMotion(StepperX);
        AttrXyze.Acceleration = Param->P; // XYZE��ļ��ٶ�
        MotionModel->SetStepperMotion(AttrXyze, StepperX);
    }

    if ( Param->HasR )
    {
        struct FeedrateAttr_t FeedrateAttr = MotionModel->GetMotionFeedRateAttr();
        FeedrateAttr.RetractAccel = Param->R;
        MotionModel->SetMotionFeedRateAttr(FeedrateAttr);
    }

    if ( Param->HasT )
    {
        struct FeedrateAttr_t FeedrateAttr = MotionModel->GetMotionFeedRateAttr();
        FeedrateAttr.TravelAccelXYZ = (S32)Param->T;
        MotionModel->SetMotionFeedRateAttr(FeedrateAttr);
    }

    return 1;
}

S32 GcodeM204Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
