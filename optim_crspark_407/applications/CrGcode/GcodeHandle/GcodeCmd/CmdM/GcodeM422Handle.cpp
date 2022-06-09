#include "GcodeM422Handle.h"
#include "CrModel/CrMotion.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include "CrInc/CrMsgType.h"
#include <stdio.h>
#include <stdlib.h>

#define CMD_M422 ("M422")
#define WITHIN(N,L,H)       ((N) >= (L) && (N) <= (H))

BOOL SetStepperZ_AlignStepperXY(struct MotionCoord_t* Coord)
{
    //TODO...
    return true;
}

struct MotionCoord_t* GetStepperZ_AlignStepperXY(VOID)
{
    //TODO...
    return NULL;
}

VOID ZstepperAlignResetDefault(VOID)
{
    //TOODO
}

S32 GcodeM422Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Offset = strlen(CMD_M422) + 1;
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M422, strlen(CMD_M422)) != 0 )
    {
        return CrErr;
    }

    struct ArgM422_t *CmdArg = (struct ArgM422_t *)Arg;
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M422 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_R:
                CmdArg->HasR = true;
                break;

            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen == 1 )
                {
                    CmdArg->S = 0;
                }
                else
                {
                    CmdArg->S = atoi(Opt + 1);
                }
                break;

            case OPT_W:
                CmdArg->HasW = true;
                if ( OptLen == 1 )
                {
                    CmdArg->W = 0;
                }
                else
                {
                    CmdArg->W = atoi(Opt + 1);
                }
                break;

            case OPT_X:
                CmdArg->HasX = true;
                if ( OptLen == 1 )
                {
                    CmdArg->X = 0.0f;
                }
                else
                {
                    CmdArg->X = atof(Opt + 1);
                }
                break;

            case OPT_Y:
                CmdArg->HasY = true;
                if ( OptLen == 1 )
                {
                    CmdArg->Y = 0.0f;
                }
                else
                {
                    CmdArg->Y = atof(Opt + 1);
                }
                break;

            default:
                printf("Gcode M422 has no define opt = %s\n", CmdBuff);
                break;
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM422Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    struct ArgM422_t *Param = (struct ArgM422_t *)ArgPtr;
    if ( Param->HasR )
    {
        ZstepperAlignResetDefault();

        return GCODE_EXEC_OK;
    }

#ifdef Z_STEPPER_ALIGN_KNOWN_STEPPER_POSITIONS
    if ( Param->HasS && Param->HasW )
    {
        printf("?(S) and (W) may not be combined!!!\n");
        return CrErr;
    }

    if ( !Param->HasS && !Param->HasW )
    {
        printf("(S) or (W) is required!!!\n");
        return CrErr;
    }
#endif

    S32 PositionIndex;
    if ( Param->HasS )
    {
        PositionIndex = Param->S - 1;
        if ( !WITHIN(PositionIndex, 0, NUM_Z_STEPPER_DRIVERS - 1) )
        {
            printf("?(S) Probe-position index invalid!!!\n");

            return CrErr;
        }
    }
    else
    {
    #ifdef Z_STEPPER_ALIGN_KNOWN_STEPPER_POSITIONS
        PositionIndex = Param->W - 1;
        if ( !WITHIN(PositionIndex, 0, NUM_Z_STEPPER_DRIVERS - 1) )
        {
            printf("?(W) Z-stepper index invalid!!!\n");
            return CrErr;
        }
    #endif
    }

    float X_Pos = Param->X;
    float Y_Pos = Param->Y;

    if ( (X_Pos < 0) || (Y_Pos < 0) || (X_Pos > X_MAX_POS) || (Y_Pos > Y_MAX_POS) )
    {
        printf("Position is invalid!!!\n");

        return CrErr;
    }

    MotionCoord_t *Pos = GetStepperZ_AlignStepperXY();
    Pos->CoorX = Param->X;
    Pos->CoorY = Param->Y;

    SetStepperZ_AlignStepperXY(Pos);

    return GCODE_EXEC_OK;
}

S32 GcodeM422Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

