#include "GcodeM166Handle.h"
#include "GcodeM165Handle.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define CMD_M166 "M166"

/**********额外定义的变量和函数****************/
#define WITHIN(N,L,H)       ((N) >= (L) && (N) <= (H))
#define RECIPROCAL(x) (WITHIN(x,-0.000001f,0.000001f) ? 0 : (1 / FLOAT(x)))

struct Gradient_t
{
    BOOL    Enabled;
    U8      Color[MIXING_STEPPERS];
    FLOAT   Startz;
    FLOAT   Endz;
    S8      Startvtool;
    S8      Endvtool;
    S8      Startmix[MIXING_STEPPERS];
    S8      Endmix[MIXING_STEPPERS];
//#ifdef GRADIENT_VTOOL
    S8      Vtoolindex;
//#endif
};
struct Gradient_t MixerGradient = {0};
static FLOAT PrevZ;


S8 MixerMix[MIXING_STEPPERS];
FLOAT MixerPrevZ = 0;
VOID GetCartesianFromSteppers()
{
    //1、获取当前轴的位置

    //2、将以度为单位的ABC输入转换为以毫米为单位的XYZ输出
}

VOID UpdateMixFromVtool(const U8 J = MixerSelectedvtool)
{
    FLOAT Ctot = 0;
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        Ctot += MixerColor[J][i];
    }
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        MixerMix[i] = (S8)(100.0f * MixerColor[J][i] / Ctot);
    }

    printf("V-tool %d [ ",J);
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        printf("%d",MixerColor[J][i]);
    }
    printf(" ] to Mix [ ");
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        printf("%d",MixerMix[i]);
    }
    printf(" ]\n");
}

VOID UpdateMixFromMixerGradient()
{
    FLOAT Ctot = 0;
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        Ctot += MixerGradient.Color[i];
    }
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        MixerMix[i] = (S8)ceilf(100.0f * MixerGradient.Color[i] / Ctot);
    }
}

VOID UpdateGradientForZ(const FLOAT Z)
{
    if ( Z == MixerPrevZ )
        return;
    MixerPrevZ = Z;
    const FLOAT Slice = MixerGradient.Endz - MixerGradient.Startz;
    FLOAT Pct = ( Z - MixerGradient.Startz ) / Slice;

    if ( Pct < 0.0f )
        Pct = 0.0f;
    if ( Pct > 1.0f )
        Pct = 1.0f;

    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        const S8 Sm = MixerGradient.Startmix[i];
        MixerMix[i] = Sm + (S32)(MixerGradient.Endmix - Sm) * Pct;
    }

    //copy MixerMix to MixerColor
    FLOAT MaxMixer = MixerMix[0];

    for ( int i = 0; i < MIXING_STEPPERS; i++ )
    {
        if ( MaxMixer < MixerMix[i] )
            MaxMixer = MixerMix[i];
    }
    const FLOAT Scale = (COLOR_A_MASK) * MaxMixer;
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        MixerGradient.Color[i] = MixerMix[i] * Scale;
    }

    printf("Mix [ ");
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        printf("%d",MixerMix[i]);
    }
    printf(" ] to Color [ ");
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        printf("%d", MixerGradient.Color[i]);
    }
    printf(" ]\n");

}
VOID UpdateGradientForPlannerZ()
{
    if ( !MotionModel )
    {
        return ;
    }
#ifdef DELTA
    XYZFloat_t Cartes;
    Cartes = GetCartesianFromSteppers();//获取笛卡尔数据（暂未实现）
    UpdateGradientForZ(Cartes.Z);       //传入的是笛卡尔坐标的Z轴数据
#else
    UpdateGradientForZ(MotionModel->GetCurPosition().PosZ);
#endif
}
/**********额外定义的变量和函数****************/

S32 GcodeM166Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    S8 Len = strlen(CmdBuff);
    if ( Len < 1 || (CmdBuff[0] != GCODE_CMD_M) )
    {
        return 0;
    }

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )   //获取命令失败
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_M166) != 0 )//传进来的命令不是M166命令
    {
        return 0;
    }

    S8 Offset = strlen(CMD_M166) + sizeof(S8);
    struct ArgM166_t *CmdArg = (struct ArgM166_t *)Arg;

    while ( Offset < Len  )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;
        }
        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M166 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_A:
            {
                CmdArg->HasA = true;
                if ( OptLen > 1 )
                {
                    CmdArg->A = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->A = 0;
                    CmdArg->HasA = false;
                }
                break;
            }
            case OPT_I:
            {
                CmdArg->HasI = true;
                if ( OptLen > 1 )
                {
                    CmdArg->I = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->I = 0;
                    CmdArg->HasI = false;
                }
                break;
            }
            case OPT_J:
            {
                CmdArg->HasJ = true;
                if ( OptLen > 1 )
                {
                    CmdArg->J = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->J = 0;
                    CmdArg->HasJ = false;
                }
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                {
                    CmdArg->S = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->S = 0;
                }
                break;
            }
            case OPT_T:
            {
                CmdArg->HasT = true;
                if ( OptLen > 1 )
                {
                    CmdArg->T = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->T = -1;
                }
                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Z = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->Z = 0;
                    CmdArg->HasZ = false;
                }
                break;
            }
        }
        Offset += strlen(Opt + sizeof(S8));
    }
    return 1;
}

VOID MixerRefreshGradient()
{
#ifdef GRADIENT_VTOOL
    BOOL IsGrd = (MixerGradient.Vtoolindex == -1 || MixerSelectedvtool == (U8)MixerGradient.Vtoolindex);
#else
    BOOL IsGrd = true;
#endif
    MixerGradient.Enabled = IsGrd && MixerGradient.Startvtool != MixerGradient.Endvtool && MixerGradient.Startz < MixerGradient.Endz;
    if ( MixerGradient.Enabled )
    {
        S8 Mixbak[MIXING_STEPPERS];
        memcpy(Mixbak,MixerMix,sizeof(Mixbak) < sizeof(MixerMix) ? sizeof(Mixbak) : sizeof(MixerMix));
        UpdateMixFromVtool(MixerGradient.Startvtool);

        memcpy(MixerGradient.Startmix,MixerMix,sizeof(MixerGradient.Startmix) < sizeof(MixerMix) ? sizeof(MixerGradient.Startmix) : sizeof(MixerMix));
        UpdateMixFromVtool(MixerGradient.Endvtool);

        memcpy(MixerGradient.Endmix,MixerMix,sizeof(MixerGradient.Endmix) < sizeof(MixerMix) ? sizeof(MixerGradient.Endmix) : sizeof(MixerMix));
        UpdateGradientForPlannerZ();

        memcpy(MixerMix,Mixbak,sizeof(MixerMix) < sizeof(Mixbak) ? sizeof(MixerMix) : sizeof(Mixbak));
        PrevZ = -1;
    }
}
S32 GcodeM166Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM166_t *Param = (struct ArgM166_t *)ArgPtr;

    //解析的数据赋值
    if ( Param->HasA )
    {
        MixerGradient.Startz = Param->A;
    }
    if ( Param->HasZ )
    {
        MixerGradient.Endz = Param->Z;
    }
    if ( Param->HasI )
    {
        MixerGradient.Startvtool = Param->I;
        if ( MixerGradient.Startvtool < 0 )
            MixerGradient.Startvtool = 0;
        if ( MixerGradient.Startvtool > MIXING_VIRTUAL_TOOLS )
            MixerGradient.Startvtool = MIXING_VIRTUAL_TOOLS;
    }
    if ( Param->HasJ )
    {
        MixerGradient.Endvtool = Param->J;
        if ( MixerGradient.Endvtool < 0 )
            MixerGradient.Endvtool = 0;
        if ( MixerGradient.Endvtool > MIXING_VIRTUAL_TOOLS )
            MixerGradient.Endvtool = MIXING_VIRTUAL_TOOLS;
    }

#ifdef GRADIENT_VTOOL
    if ( Param->HasT )
    {
        MixerGradient.Vtoolindex = Param->T;
    }
#endif
    if ( Param->HasS )
    {
        MixerGradient.Enabled = Param->S;
    }

    //修改并刷新Gradient结构体中的数据
    MixerRefreshGradient();

    //打印提示
    printf("Gradient Mix ");
    if ( MixerGradient.Enabled )
        printf("ON");
    else
        printf("OFF");

    if ( MixerGradient.Enabled )
    {
#ifdef GRADIENT_VTOOL
        if ( MixerGradient.Vtoolindex >= 0)
        {
            printf(" (T%d)",MixerGradient.Vtoolindex);
        }
#endif
        printf(" ; Start");

        UpdateMixFromVtool(MixerGradient.Startvtool);
        printf("%f%d",MixerGradient.Startz,MixerGradient.Startvtool);//输出start_z,start_vtool
        printf(" (%d%%|%d%%) :End",MixerMix[0],MixerMix[1]);            //要输出'%'

        printf("%f%d",MixerGradient.Endz,MixerGradient.Endvtool);
        printf(" (%d%%|%d%%)",MixerMix[0],MixerMix[1]);

        //从MixerGradient中更新mix
        UpdateMixFromMixerGradient();

        printf(" ; Current Z");
#ifdef DELTA
        XYZFloat_t Cartes;
        Cartes = GetCartesianFromSteppers();
        printf("%f",Cartes.Z));
#else
        printf("%f",MotionModel->GetCurPosition().PosZ);
#endif
        printf(" (%d%%|%d%%)",MixerMix[0],MixerMix[1]);
    }
    printf("\n");
    return 1;
}

S32 GcodeM166Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
