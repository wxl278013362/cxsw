#include "GcodeM165Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"

#include "CrInc/CrConfig.h"

#define CMD_M165 ("M165")
/********额外定义的变量和函数*************/
//M165命令需要支持DIRECT_MIXING_IN_G1、MIXING_EXTRUDER

static FLOAT Collector[MIXING_STEPPERS] = {0};


S16 MixerColor[NR_MIXING_VIRTUAL_TOOLS][MIXING_STEPPERS] = {0};
U8  MixerSelectedvtool = 0;

#define _BV(n) (1<<(n))
#define TEST(n,b) (!!((n)&_BV(b)))
#define SBI(A,B) (A |= _BV(B))


VOID MixRefreshGradient()
{
    //更改后刷新gradient
}

VOID MixerSetCollector(U8 C,FLOAT F)
{
    if ( C > MIXING_STEPPERS - 1)
    {
        return;
    }
    if ( F > 0.0f )
    {
        Collector[C] = F;
    }
    else
    {
        Collector[C] = 0.0f;
    }
}

VOID MixerNormalize(U8 ToolIndex)
{
    FLOAT Cmax = 0;
    if ( ToolIndex > NR_MIXING_VIRTUAL_TOOLS -1 )   //超出数组最大值
    {
        return ;
    }
#ifdef MIXER_NORMALIZER_DEBUG
    FLOAT Csum = 0;
#endif
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        const FLOAT V = Collector[i];
        if ( V > Cmax ) Cmax = V;
#ifdef MIXER_NORMALIZER_DEBUG
        Csum += V;
#endif
    }
#ifdef MIXER_NORMALIZER_DEBUG
    printf("Mixer: Old relation : [ ");
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        printf("%.3f ",Collector[i] / Csum);
    }
    printf("]\n");
#endif
    const FLOAT Scale = FLOAT(COLOR_A_MASK) / Cmax;
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        MixerColor[ToolIndex][i] = Collector[i] * Scale;
    }
#ifdef MIXER_NORMALIZER_DEBUG
    Csum = 0;
    printf("Mixer: Normalize to : [ ");
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        printf("%d ",U16(MixerColor[ToolIndex][i]));
        Csum += MixerColor[ToolIndex][i];
    }
    printf("]\n");
    printf("Mixer: New relation : [ ");
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        printf("%.3f ",U16(MixerColor[ToolIndex][i]) / Csum);
    }
    printf("]\n");
#endif


#ifdef GRADIENT_MIX
    MixRefreshGradient();
#endif

}
/********额外定义的变量和函数*************/

S32 GcodeM165Parse(const S8 *CmdBuff, VOID *Arg) //M165命令的解析函数
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    S8 Len = strlen(CmdBuff);
    if ( Len <= 1 || CmdBuff[0] != GCODE_CMD_M )
    {
        return 0;
    }

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )   //获取命令失败
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_M165) != 0 )
    {
        return 0;
    }

    S8 Offset = strlen(CMD_M165) + sizeof(S8);
    struct ArgM165_t *CmdArg = (struct ArgM165_t *)Arg;

    S32 OptLen = 0;
    while( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;
        }

        OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M165 has no param opt = %s\n",CmdBuff);
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
                    CmdArg->HasA = false;
                    CmdArg->A = 0;
                }
                break;
            }
            case OPT_B:
            {
                CmdArg->HasB = true;
                if ( OptLen > 1 )
                {
                    CmdArg->B = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->B = 0;
                    CmdArg->HasB = false;
                }
                break;
            }
            case OPT_C:
            {
                CmdArg->HasC = true;
                if ( OptLen > 1 )
                {
                    CmdArg->C = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->C = 0;
                    CmdArg->HasC = false;
                }
                break;
            }
            case OPT_D:
            {
                CmdArg->HasD = true;
                if ( OptLen > 1 )
                {
                    CmdArg->D = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->D = 0;
                    CmdArg->HasD = false;
                }
                break;
            }
            case OPT_H:
            {
                CmdArg->HasH = true;
                if ( OptLen > 1 )
                {
                    CmdArg->H = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->H = 0;
                    CmdArg->HasH = false;
                }
                break;
            }
            case OPT_I:
            {
                CmdArg->HasI = true;
                if ( OptLen > 1 )
                {
                    CmdArg->I = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->I = 0;
                    CmdArg->HasI = false;
                }
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt + sizeof(S8));
    }
    return 1;
}
S32 GcodeM165Exec(VOID *ArgPtr, VOID *ReplyResult)   //M165命令的执行函数
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM165_t *Param = (struct ArgM165_t *)ArgPtr;
    S8 MixingCodes[6] = {0};
    switch(MIXING_STEPPERS)
    {
        case 1:
        {
            MixingCodes[0] = 'A';
            break;
        }
        case 2:
        {
            MixingCodes[0] = 'A';
            MixingCodes[1] = 'B';
            break;
        }
        case 3:
        {
            MixingCodes[0] = 'A';
            MixingCodes[1] = 'B';
            MixingCodes[2] = 'C';
            break;
        }
        case 4:
        {
            MixingCodes[0] = 'A';
            MixingCodes[1] = 'B';
            MixingCodes[2] = 'C';
            MixingCodes[3] = 'D';
            break;
        }
        case 5:
        {
            MixingCodes[0] = 'A';
            MixingCodes[1] = 'B';
            MixingCodes[2] = 'C';
            MixingCodes[3] = 'D';
            MixingCodes[4] = 'H';
            break;
        }
        case 6:
        {
            MixingCodes[0] = 'A';
            MixingCodes[1] = 'B';
            MixingCodes[2] = 'C';
            MixingCodes[3] = 'D';
            MixingCodes[4] = 'H';
            MixingCodes[5] = 'I';
            break;
        }
        default:
            break;
    }

    U8 MixBits = 0;
    for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
    {
        switch( MixingCodes[i] )
        {
            case OPT_A:
            {
                if ( Param->HasA )
                {
                    SBI(MixBits, i);
                    MixerSetCollector(i,Param->A);
                }
                break;
            }
            case OPT_B:
            {
                if ( Param->HasB )
                {
                    SBI(MixBits, i);
                    MixerSetCollector(i,Param->B);
                }
                break;
            }
            case OPT_C:
            {
                if ( Param->HasC )
                {
                    SBI(MixBits, i);
                    MixerSetCollector(i,Param->C);
                }
                break;
            }
            case OPT_D:
            {
                if ( Param->HasD )
                {
                    SBI(MixBits, i);
                    MixerSetCollector(i,Param->D);
                }
                break;
            }
            case OPT_H:
            {
                if ( Param->HasH )
                {
                    SBI(MixBits, i);
                    MixerSetCollector(i,Param->H);
                }
                break;
            }
            case OPT_I:
            {
                if ( Param->HasI )
                {
                    SBI(MixBits, i);
                    MixerSetCollector(i,Param->I);
                }
                break;
            }
            default:
                break;
        }
    }

    if ( MixBits )
    {
        for ( U8 i = 0; i < MIXING_STEPPERS; i++ )
        {
            if ( !TEST(MixBits,i ) )
            {
                MixerSetCollector(i,0.0f);
            }
        }
        MixerNormalize(MixerSelectedvtool);
    }
    return 1;
}
S32 GcodeM165Reply(VOID *ReplyResult, S8 *Buff)   //M165命令的回复函数
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
