#include "GcodeM164Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"

#ifdef MIXING_EXTRUDER

#define CMD_M164 ("M164")

U8 Color[NR_MIXING_VIRTUAL_TOOLS][MIXING_STEPPERS];
S8 Mix[MIXING_STEPPERS];
U8 SelectedVtool = 0;

struct Gradient_t Gradient = {
    false,      // enabled
    {0},        // color (array)
    0, 0,       // start_z, end_z
    0, 1,       // start_vtool, end_vtool
    {0}, {0},   // start_mix[], end_mix[]
#ifdef GRADIENT_VTOOL
    -1          // vtool_index
#endif
};

VOID SetVtool(U8 Val)
{
    SelectedVtool = Val;
}

U8 GetVtool()
{
    return SelectedVtool;
}

VOID UpdateMixFromVtool(const S8 j = SelectedVtool)
{
    FLOAT Ctot = 0;
    MIXER_STEPPER_LOOP(i) Ctot += Color[j][i];
    MIXER_STEPPER_LOOP(i) Mix[i] = S8(100.0f * Color[j][i] / Ctot);
}

VOID RefreshGradient()
{
#ifdef GRADIENT_VTOOL
    const BOOL isGrd = (Gradient.VtoolIndex == -1 || SelectedVtool == (uint8_t)Gradient.VtoolIndex);
#else
    constexpr BOOL isGrd = true;
#endif

    Gradient.Enabled = isGrd && Gradient.StartVtool != Gradient.EndVtool && Gradient.StartZ < Gradient.EndZ;
    if ( Gradient.Enabled )
    {
        S8 MixBak[MIXING_STEPPERS];
        COPY(MixBak, Mix);
        UpdateMixFromVtool(Gradient.StartVtool);
        COPY(Gradient.StartMix, Mix);
        UpdateMixFromVtool(Gradient.EndVtool);
        COPY(Gradient.EndMix, Mix);

        //M164 TODO update_gradient_for_planner_z



        COPY(Mix, MixBak);
    }
}

static VOID MixerNormalize(const U8 ToolIndex)
{
    FLOAT Cmax = 0;

    MIXER_STEPPER_LOOP(i) {
      FLOAT v = Collector[i];
      NOLESS(Cmax, v);
    }

    const FLOAT Scale = FLOAT(COLOR_A_MASK) / Cmax;
    MIXER_STEPPER_LOOP(i) Color[ToolIndex][i] = Collector[i] * Scale;
    RefreshGradient();
}

S32 GcodeM164Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M164, strlen(CMD_M164)) != 0 )
    {
        return 0;
    }

    struct ArgM164_t *CmdArg = (struct ArgM164_t *)Arg;
    S32 Offset = strlen(CMD_M164) + 1; //加1是越过分隔符
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )  //无参，默认设置-1
        {
            printf("Gcode M164 has no param opt = %s\n", CmdBuff);

            CmdArg->HasS = false;
            CmdArg->S = -1;

            return GCODE_EXEC_OK;
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen == 1)
                {
                    CmdArg->HasValueS = false;
                }
                else
                {
                    CmdArg->HasValueS = true;
                    CmdArg->S = atoi(Opt + 1);
                }
                break;
            }

            default:
            {
                printf("Gcode M164 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}


S32 GcodeM164Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    struct ArgM164_t *Param = (struct ArgM164_t *)ArgPtr;

#if MIXING_VIRTUAL_TOOLS > 1
    const int ToolIndex = (Param->HasS && Param->HasValueS) ? Param->S : -1;
#else
    constexpr int ToolIndex = 0;
#endif

    if ( ToolIndex >= 0 && ToolIndex < MIXING_VIRTUAL_TOOLS )
    {
        MixerNormalize(ToolIndex);
    }
    else
    {
        MixerNormalize(SelectedVtool);
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM164Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

#endif
