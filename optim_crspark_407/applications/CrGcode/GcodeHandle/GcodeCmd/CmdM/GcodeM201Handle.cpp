#include "GcodeM201Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"

#define CMD_M201  "M201"

/* M201ָ
   M201 [E<accel>] [T<index>] [X<accel>] [Y<accel>] [Z<accel>]
 */
S32 GcodeM201Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return CrErr;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return CrErr;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return CrErr;

    // M201
    if ( strcmp(Cmd, CMD_M201) != 0 )
        return CrErr;

    S32 Offset = strlen(CMD_M201) + sizeof(S8); // ��1��Խ���ָ���
    struct ArgM201_t *CmdArg = (struct ArgM201_t *)Arg;

    // [E<accel>] [T<index>] [X<accel>] [Y<accel>] [Z<accel>]
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

        Offset += strlen(Opt) + sizeof(S8);
    }

    return GCODE_EXEC_OK;
 }


S32 GcodeM201Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return CrErr;

    struct ArgM201_t *Param = (struct ArgM201_t *)ArgPtr;

    /* [E<accel>] [T<index>] [X<accel>] [Y<accel>] [Z<accel>]
     */

    if ( Param->HasT )
    {
        if ( Param->T < 0 || Param->T >= EXTRUDERS )
        {
            rt_kprintf("Invalid extruder\r\n");
            return CrErr;
        }
    }

    if ( Param->HasX )
    {
        struct StepperMotionAttr_t AttrX = MotionModel->GetSetpperMotion(StepperX);
        AttrX.Acceleration = (S32)Param->X;
        MotionModel->SetStepperMotion(AttrX, StepperX);
    }

    if ( Param->HasY )
    {
        struct StepperMotionAttr_t AttrY = MotionModel->GetSetpperMotion(StepperY);
        AttrY.Acceleration = (S32)Param->Y;
        MotionModel->SetStepperMotion(AttrY, StepperY);
    }

    if ( Param->HasZ )
    {
        struct StepperMotionAttr_t AttrZ = MotionModel->GetSetpperMotion(StepperZ);
        AttrZ.Acceleration = (S32)Param->Z;
        MotionModel->SetStepperMotion(AttrZ, StepperZ);
    }

    if ( Param->HasE )
    {
        struct StepperMotionAttr_t AttrE = MotionModel->GetSetpperMotion(StepperE);
        AttrE.Acceleration = (S32)Param->E;
        MotionModel->SetStepperMotion(AttrE, StepperE);
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM201Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

