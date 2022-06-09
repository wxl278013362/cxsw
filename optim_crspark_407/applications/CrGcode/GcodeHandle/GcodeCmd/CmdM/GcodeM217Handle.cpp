#include "GcodeM217Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"

#define CMD_M217 "M217"

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

static struct ToolChangeSettings_t* ToolChange = NULL;
struct MigrationSettings_t Migration = {0};
BOOL EnableFirstPrime = false;

static VOID ToolChangePrime()
{

}

#ifdef TOOLCHANGE_MIGRATION_FEATURE
static BOOL ExtruderMigration()
{

    return true;
}
#endif

static void M217Report(const BOOL Eeprom)
{
#ifdef TOOLCHANGE_FILAMENT_SWAP
    printf("%s", Eeprom ? "  M217" : "Toolchange:");
    if ( ToolChange )
    {
        printf(" S%f", ToolChange->SwapLength);
        printf(" B%f E%f P%d", ToolChange->ExtraResume, ToolChange->ExtraPrime, ToolChange->PrimeSpeed);
        printf(" R%d U%d F%d G%d", ToolChange->RetractSpeed, ToolChange->UnRetractSpeed, ToolChange->FanSpeed, ToolChange->FanTime);

#ifdef TOOLCHANGE_PARK
        printf(" W%d", ToolChange->EnablePark);
        printf(" X%f", ToolChange->ChangePointX);
        printf(" Y%f", ToolChange->ChangePointY);
#endif
    }

#ifdef TOOLCHANGE_MIGRATION_FEATURE
    printf(" A%d", int(Migration.AutoMode));
    printf(" L%d", Migration.Last);
#endif

#ifdef TOOLCHANGE_FS_PRIME_FIRST_USED
    printf(" V%d", (S32)EnableFirstPrime);
#endif

#else

  UNUSED(Eeprom);

#endif

    if ( ToolChange )
        printf(" Z%f", ToolChange->RaiseZ);

    printf("\n");

    return;
}


VOID GcodeM217Init(struct ToolChangeSettings_t *Setting)
{
    ToolChange = Setting;
}

S32 GcodeM217Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;


    if ( strcmp(Cmd, CMD_M217) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M217) + sizeof(S8);
    struct ArgM217_t *CmdArg = (struct ArgM217_t *)Arg;

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
            case OPT_Q:
                CmdArg->HasQ = true;
                break;
            case OPT_A:
                CmdArg->HasA = true;
                if ( OptLen > 1 )
                    CmdArg->A = atoi(Opt + 1);
                break;
            case OPT_B:
                CmdArg->HasB = true;
                if ( OptLen > 1 )
                    CmdArg->B = atof(Opt + 1);
                break;
            case OPT_E:
                CmdArg->HasE = true;
                if ( OptLen > 1 )
                    CmdArg->E = atof(Opt + 1);
                break;
            case OPT_F:
                CmdArg->HasF = true;
                if ( OptLen > 1 )
                    CmdArg->F = atoi(Opt + 1);
                break;
            case OPT_G:
                CmdArg->HasG = true;
                if ( OptLen > 1 )
                    CmdArg->G = atoi(Opt + 1);
                break;
            case OPT_L:
                CmdArg->HasL = true;
                if ( OptLen > 1 )
                    CmdArg->L = atoi(Opt + 1);
                break;
            case OPT_P:
                CmdArg->HasP = true;
                if ( OptLen > 1 )
                    CmdArg->P = atoi(Opt + 1);
                break;
            case OPT_R:
                CmdArg->HasR = true;
                if ( OptLen > 1 )
                    CmdArg->R = atoi(Opt + 1);
                break;
            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                    CmdArg->S = atof(Opt + 1);
                break;
            case OPT_T:
                CmdArg->HasT = true;
                if ( OptLen > 1 )
                {
                    CmdArg->T = atoi(Opt + 1);
                }
                else   //T后面没有数值时
                {
                    CmdArg->T = -1;
                }
                break;
            case OPT_U:
                CmdArg->HasU = true;
                if ( OptLen > 1 )
                    CmdArg->U = atoi(Opt + 1);
                break;
            case OPT_V:
                CmdArg->HasV = true;
                if ( OptLen > 1 )
                    CmdArg->V = atof(Opt + 1);
                break;
            case OPT_W:
                CmdArg->HasW = true;
                if ( OptLen > 1 )
                    CmdArg->W = atof(Opt + 1);
                break;
            case OPT_X:
                CmdArg->HasX = true;
                if ( OptLen > 1 )
                    CmdArg->X = atoi(Opt + 1);
                break;
            case OPT_Y:
                CmdArg->HasY = true;
                if ( OptLen > 1 )
                    CmdArg->Y = atoi(Opt + 1);
                break;
            case OPT_Z:
                CmdArg->HasZ = true;
                if ( OptLen > 1 )
                    CmdArg->Z = atof(Opt + 1);
                break;
            default:
                printf("Warn:unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
 }


S32 GcodeM217Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !ToolChange )
        return 0;

    struct ArgM217_t *Param = (struct ArgM217_t *)ArgPtr;
#ifdef TOOLCHANGE_FILAMENT_SWAP

    static constexpr FLOAT MaxExtrude = 500;     ///TERN(PREVENT_LENGTHY_EXTRUDE, EXTRUDE_MAXLENGTH, 500);
#ifdef PREVENT_LENGTHY_EXTRUDE
#ifdef EXTRUDE_MAXLENGTH
    MaxExtrude = EXTRUDE_MAXLENGTH;
#endif
#endif
    if ( Param->HasQ )
    {
        tool_change_prime();
        return ;
    }

    if ( Param->HasS )
    {
        ToolChange->SwapLength = CONSTRAIN(Param->S, 0, MaxExtrude);
    }

    if ( Param->HasB )
        ToolChange->ExtraResume = CONSTRAIN(Param->B, -10, 10);

    if ( Param->HasE )
        ToolChange->ExtraPrime = CONSTRAIN(Param->E, 0, MaxExtrude);

    if ( Param->HasP )
        ToolChange->PrimeSpeed = CONSTRAIN(Param->P, 10, 5400);    //单位mm/min

    if ( Param->HasR )
        ToolChange->RetractSpeed = CONSTRAIN(Param->R, 10, 5400);    //单位mm/min

    if ( Param->HasU )
        ToolChange->UnRetractSpeed = CONSTRAIN(Param->U, 10, 5400);    //单位mm/min

#if TOOLCHANGE_FS_FAN >= 0 && HAS_FAN

    if ( Param->HasF )
        ToolChange->FanSpeed = CONSTRAIN(Param->F, 0, 255);

    if ( Param->HasG )
        ToolChange->FanTime = CONSTRAIN(Param->G, 1, 30);

#endif
#endif

#ifdef TOOLCHANGE_FS_PRIME_FIRST_USED
    if ( Param->HasV )
        EnableFirstPrime = Param->V;

#endif

#ifdef TOOLCHANGE_PARK
    if ( Param->HasW )
        ToolChange->EnablePark = Param->W;
    if ( Param->HasX )
        ToolChange->ChangePointX = Param->X;

    if ( ToolChange->HasY )
        ToolChange->ChangePointY = Param->Y;

#endif

    if ( Param->HasZ )
        ToolChange->RaiseZ = Param->Z;

#ifdef TOOLCHANGE_MIGRATION_FEATURE
    Migration.Target = 0;   // 0 = disabled

    S8 ActiveExtruder = 0;  //获取当前活动的挤出机索引
    if ( Param->HasL )
    {
        if ( (Param->L >= 0) &&  ( Param->L <= EXTRUDERS - 1 ) )
        {
            Migration.Last = Param->L;
            Migration.AutoMode = (ActiveExtruder < Migration.Last);
        }
    }

    if ( Param->HasA )  // Auto on/off
    {
        Migration.AutoMode = Param->A;
    }

    if ( Param->HasT )
    {
        if ( Param->T >= 0 )   //表示T后面跟有数值
        {
            if ( (Param->T >= 0) && (Param->T <= (EXTRUDERS - 1)) && (Param->T != ActiveExtruder) )
            {
                Migration.Target = Param->T + 1;
                ExtruderMigration();
                Migration.Target = 0; // disable
                return 1;
            }
            else
            {
                Migration.Target = 0;  // disable
            }
        }
        else
        {
            ExtruderMigration();
            return 1;
        }
    }
#endif

    M217Report(false);

    return 1;
}

S32 GcodeM217Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

