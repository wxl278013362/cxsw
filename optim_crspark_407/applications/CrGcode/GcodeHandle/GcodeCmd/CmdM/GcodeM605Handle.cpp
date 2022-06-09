#include "GcodeM605Handle.h"
#include "CrModel/CrMotion.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CMD_M605 ("M605")
#define _BV(n) (1<<(n))
#define TEST(n,b) (!!((n)&_BV(b)))

#define CONSTRAIN(value, arg_min, arg_max) ((value) < (arg_min) ? (arg_min) :((value) > (arg_max) ? (arg_max) : (value)))
#define DEFAULT_DUPLICATION_X_OFFSET 100

#define DEFAULT_DUAL_X_CARRIAGE_MODE DXC_AUTO_PARK_MODE
#define X1_MIN_POS      X_MIN_POS   // Set to X_MIN_POS
#define X1_MAX_POS      BED_LENGTH  // Set a maximum so the first X-carriage can't hit the parked second X-carriage
#define X2_MIN_POS      80       // Set a minimum to ensure the  second X-carriage can't hit the parked first X-carriage
#define X2_MAX_POS      353       // Set this to the distance between toolheads when both heads are homed

DualXMode   dualXcarriageMode = DEFAULT_DUAL_X_CARRIAGE_MODE;
FLOAT       DuplicateExtruderXoffset = DEFAULT_DUPLICATION_X_OFFSET;
S16         DuplicateExtruderTempOffset = 0;
S8          ActiveExtruder = 0;
BOOL        ExtruderDuplicationEnabled = false;
FLOAT       InActiveExtruderX = X2_MAX_POS;
U32         DelayedMoveTime = 0;
BOOL        ActiveExtruderParked = false;
U8          DuplicationEmask = 0;

S32 GcodeM605Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M605, strlen(CMD_M605)) != 0 )
    {
        return 0;
    }

    struct ArgM605_t *CmdArg = (struct ArgM605_t *)Arg;
    S32 Offset = strlen(CMD_M605) + 1; //加1是越过分隔符
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
            printf("Gcode M605 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_P:
                CmdArg->HasP = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueP = false;
                }
                else
                {
                    CmdArg->HasValueP = true;
                    CmdArg->P = atoi(Opt + 1);
                }
                break;

            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueS = false;
                }
                else
                {
                    CmdArg->HasValueS = true;
                    CmdArg->S = atoi(Opt + 1);
                }
                break;

            case OPT_E:
                CmdArg->HasE = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueE = false;
                }
                else
                {
                    CmdArg->HasValueE = true;
                    CmdArg->E = atoi(Opt + 1);
                }
                break;

            case OPT_X:
                CmdArg->HasX = true;
                if ( OptLen == 1 )
                {
                    CmdArg->X = 0;
                }
                else
                {
                    CmdArg->X = atoi(Opt + 1);
                }
                break;

            case OPT_R:
                CmdArg->HasS = true;
                if ( OptLen == 1 )
                {
                    CmdArg->R = 0;
                }
                else
                {
                    CmdArg->R = atoi(Opt + 1);
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

            default:
            {
                printf("Gcode M605 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}


S32 GcodeM605Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    struct ArgM605_t *Param = (struct ArgM605_t *)ArgPtr;
#ifdef DUAL_X_CARRIAGE
    while(!IsPlanLineBuffEmpty(LineBuff));

    if ( Param->HasS )
    {
        const DualXMode PreviousMode = dualXcarriageMode;
        dualXcarriageMode = Param->HasValueS ? (CONSTRAIN(Param->S, 0, 255)) : 0;
        //idex_set_mirrored_mode(false);

        if ( dualXcarriageMode == DXC_MIRRORED_MODE)
        {
            if ( PreviousMode != DXC_DUPLICATION_MODE )
            {
                printf("Printer must be in DXC_DUPLICATION_MODE prior to ");
                printf("specifying DXC_MIRRORED_MODE.");
                dualXcarriageMode = DEFAULT_DUAL_X_CARRIAGE_MODE;
                return CrErr;
            }

            //idex_set_mirrored_mode(true);
            struct MotionPositionMsg_t Pos = {0};
            if ( !MotionModel )
            {
                return CrErr;
            }
            Pos = MotionModel->GetCurPosition();    //当前位置
            struct Coordinate3d_t Cur = {0};
            Cur.X = Pos.PosX;
            Cur.Y = Pos.PosY;
            Cur.Z = Pos.PosZ;
            Cur.E = Pos.PosE;

            struct Coordinate3d_t Tar = {0};
            Tar.X = Cur.X - 0.1;
            Tar.Y = Cur.Y;
            Tar.Z = Cur.Z;
            Tar.Z = Cur.Z;

            for ( U8 i = 2; --i;)
            {
                PlanLine(LineBuff, &Cur, &Tar);
                Tar.X += 0.1;
            }
            return CrErr;
        }

        switch ( dualXcarriageMode )
        {
            case DXC_FULL_CONTROL_MODE:
            case DXC_AUTO_PARK_MODE:
                break;

            case DXC_DUPLICATION_MODE:
                if ( Param->HasS )
                {
                    DuplicateExtruderXoffset = MAX((Param->HasValueS ? Param->S : 0), (X2_MIN_POS) - (X1_MIN_POS));
                }
                if ( Param->HasR )
                {
                    DuplicateExtruderTempOffset = Param->R;
                }
                if ( ActiveExtruder != 0 )
                {
                    //tool_change(0);
                }
                break;

            default:
                dualXcarriageMode = DEFAULT_DUAL_X_CARRIAGE_MODE;
                break;
        }

        //idex_set_parked(false);
        ExtruderDuplicationEnabled = false;
    }
    else if ( Param->HasW )
    {
        dualXcarriageMode = DEFAULT_DUAL_X_CARRIAGE_MODE;
    }

#ifdef DEBUG_DXC_MODE

    if ( Param->HasW )
    {
        printf("Dual X Carriage Mode ");
        switch (dualXcarriageMode) 
        {
            case DXC_FULL_CONTROL_MODE:
                printf("FULL_CONTROL");
                break;
            case DXC_AUTO_PARK_MODE:
                printf("AUTO_PARK");
                break;
            case DXC_DUPLICATION_MODE:
                printf("DUPLICATION");
                break;
            case DXC_MIRRORED_MODE:
                printf("MIRRORED");
                break;
            default:
                break;;
        }
        struct MotionPositionMsg_t Pos = {0};
        if ( !MotionModel )
        {
            return CrErr;
        }
        Pos = MotionModel->GetCurPosition();    //当前位置
        printf("\nActive Ext: ", ActiveExtruder);
        if ( !ActiveExtruderParked )
        {
            printf(" NOT ");
        }
        printf(" parked.");
        printf("\n active_extruder_x_pos: %4f", Pos.PosX);
        printf("\n inactive_extruder_x: %d", InActiveExtruderX);
        printf("\nextruder_duplication_enabled: ", ExtruderDuplicationEnabled);
        printf("\nduplicate_extruder_x_offset: ", DuplicateExtruderXoffset);
        printf("\nduplicate_extruder_temp_offset: ", DuplicateExtruderTempOffset);
        printf("\ndelayed_move_time: ", DelayedMoveTime);
        printf("\nX1 Home X:  %4f, X1_MIN_POS= %d, X1_MAX_POS = %d", SettingData.HomeOffset.X, X1_MIN_POS, X1_MAX_POS);
        printf("\nX2_MIN_POS = %d, X2_MAX_POS = %d", X2_MIN_POS, X2_MAX_POS);
        printf("\nDEFAULT_DUAL_X_CARRIAGE_MODE= %d", STRINGIFY(DEFAULT_DUAL_X_CARRIAGE_MODE));
        //printf("\toolchange_settings.z_raise=", toolchange_settings.z_raise);
        printf("\nDEFAULT_DUPLICATION_X_OFFSET = %d", DEFAULT_DUPLICATION_X_OFFSET);

    #endif // DEBUG_DXC_MODE
    }
#else if defined MULTI_NOZZLE_DUPLICATION
    BOOL Ena = false;
    if ( Param->HasE || Param->HasP || Param->HasS )
    {
        while(!IsPlanLineBuffEmpty(LineBuff));

        if ( Param->HasP && Param->HasValueP )
        {
            DuplicationEmask = Param->P;
        }
        else if ( Param->HasE && Param->HasValueE )
        {
            DuplicationEmask = pow(2, Param->E + 1) - 1;
        }

        Ena = (2 == ((Param->HasS && Param->HasValueS) ? Param->S : (ExtruderDuplicationEnabled ? 2 : 0)));
        ExtruderDuplicationEnabled = (Ena && (DuplicationEmask >= 3));
    }

    if ( ExtruderDuplicationEnabled )
    {
        printf("extruder duplication enabled\n");
    }
    else
    {
        printf("extruder duplication disenabled\n");
    }
    if ( Ena )
    {
        for ( U8 i = 0; i < HOTENDS; i++ )
        {
            if ( TEST(DuplicationEmask, i) )
            {
                printf("%d ", i);
            }
        }
    }


#endif

    return GCODE_EXEC_OK;
}

S32 GcodeM605Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
    {
        return Len;
    }

    return Len;
}

