#include "GcodeM203Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"

#define CMD_M203  "M203"

static CrMotion *M203MotionModel = NULL;

VOID GcodeM203Init(VOID *Model)
{
    M203MotionModel = (CrMotion *)Model;
}

/* ����M203ָ��
   M203 [E<units/s>] [T<index>] [X<units/s>] [Y<units/s>] [Z<units/s>]
 */
S32 GcodeM203Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M203
    if ( strcmp(Cmd, CMD_M203) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M203) + sizeof(S8); // ��1��Խ���ָ���
    struct ArgM203_t *CmdArg = (struct ArgM203_t *)Arg;

    // [E<units/s>] [T<index>] [X<units/s>] [Y<units/s>] [Z<units/s>]
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
            case OPT_E:
                CmdArg->HasE = true;
                if ( OptLen > 1 ) CmdArg->E = atof(Opt + 1);
                break;

            case OPT_T:
                CmdArg->HasT = true;
                if ( OptLen > 1 ) CmdArg->T = atoi(Opt + 1);
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


S32 GcodeM203Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel)
        return 0;

    struct ArgM203_t *Param = (struct ArgM203_t *)ArgPtr;

    struct FeedrateAttr_t FeedrateAttr = MotionModel->GetMotionFeedRateAttr();

    /* [E<units/s>] [T<index>] [X<units/s>] [Y<units/s>] [Z<units/s>]
     */
    if ( Param->HasT )
    {
        if ( Param->T < 0 || Param->T > EXTRUDERS )
        {
            rt_kprintf("Invalid extruder\r\n");
            return 0;
        }
    }

    if ( Param->HasX )
    {
        // ����X���������ٶ�
        //printf(">> X:%d \n", (S32)Param->X);
        FeedrateAttr.Feedrate = Param->X;
        MotionModel->SetMotionFeedRateAttr(FeedrateAttr);
    }

    if ( Param->HasY )
    {
        // ����Y���������ٶ�
        //printf(">> Y:%d \n", (S32)Param->Y);
        FeedrateAttr.Feedrate = Param->Y;
        MotionModel->SetMotionFeedRateAttr(FeedrateAttr);
    }

    if ( Param->HasZ )
    {
        // ����Z���������ٶ�
        //printf(">> Z:%d \n", (S32)Param->Z);
        FeedrateAttr.Feedrate = Param->Z;
        MotionModel->SetMotionFeedRateAttr(FeedrateAttr);
    }

    if ( Param->HasE )
    {
        // ����E���������ٶ�
        //printf(">> E:%d \n", (S32)Param->E);
        FeedrateAttr.Feedrate = Param->E;
        MotionModel->SetMotionFeedRateAttr(FeedrateAttr);
    }

    return 1;
}

S32 GcodeM203Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

