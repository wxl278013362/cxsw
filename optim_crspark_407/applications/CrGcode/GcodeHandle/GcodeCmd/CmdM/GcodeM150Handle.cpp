#include "GcodeM150Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrHeaterManager.h"

#define CMD_M150        "M150"

/*****额外定义的变量和函数*******/
#if NEOPIXEL2_SEPARATE
VOID Neo2_SetNeoindex(S8 Index)
{

}
S8 Neo2Brightness()
{
    return 0;
}
VOID Leds2SetColor(struct LedColor_t *Incol)
{

}
#endif

#if NEOPIXEL_LED
VOID Neo_SetNeoindex(S8 Index)
{

}
S8 NeoBrightness()
{
    return 0;
}
#endif

struct LedColor_t
{
    U8 R;
    U8 G;
    U8 B;
#if HAS_WHITE_LED
    U8 W;
#endif
#if NEOPIXEL_LED
    U8 I;
#endif
};

VOID LedsSetColor(struct LedColor_t *Incol)
{

}
/*****额外定义的变量和函数*******/

S32 GcodeM150Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    S32 Lenth = strlen(CmdBuff);
    if ( Lenth <= 1 || CmdBuff[0] != GCODE_CMD_M )  //Gcode命令长度需要大于1且是M命令
    {
        return 0;
    }

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )           //获取命令失败
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_M150) != 0 )                //传进来的命令不是M150命令
    {
        return 0;
    }

    S32 Offset = strlen(CMD_M150) + sizeof(S8);
    struct ArgM150_t *CmdArg = (struct ArgM150_t *)Arg;

    S32 OptLen = 0;
    while ( Offset < Lenth )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )    //获取参数失败
        {
            break;
        }

        OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M150 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_B:
            {
                CmdArg->HasB = true;
                if ( OptLen > 1 )
                {
                    CmdArg->B = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->B = 255;
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
                    CmdArg->I = -1;
                }
                break;
            }
            case OPT_P:
            {
                CmdArg->HasP = true;
                if ( OptLen > 1 )
                {
                    CmdArg->P = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->P = 255;
                }
                break;
            }
            case OPT_R:
            {
                CmdArg->HasR = true;
                if ( OptLen > 1 )
                {
                    CmdArg->R = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->R = 255;
                }
                break;
            }
            case OPT_U:
            {
                CmdArg->HasU = true;
                if ( OptLen > 1 )
                {
                    CmdArg->U = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->U = 255;
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
            case OPT_W:
            {
                CmdArg->HasW = true;
                if ( OptLen > 1 )
                {
                    CmdArg->W = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->W = 255;
                }
                break;
            }
            default:
                break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}
S32 GcodeM150Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    //判空
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM150_t *Param = (struct ArgM150_t *)ArgPtr;
    //如果支持NEOPIXEL_LED
    #ifdef NEOPIXEL_LED
        S8 Index = 0;
        if ( Param->HasI )
        {
            Index = Param->I;
        }
        else
        {
            Index = -1;
        }
        #ifdef NEOPIXEL2_SEPARATE
            S8 Brightness = 0;
            S8 Unit = 0;
            S8 Neo2neoIndex;
            S8 NeoneoIndex;
            if ( Param->HasS )
            {
                Unit = Param->S;
            }
            else
            {
                Unit = 0;
            }

            Brightness = Unit ? Neo2Brightness() : NeoBrightness();
            *( Unit ? &Neo2neoIndex : &NeoneoIndex) = Index;
        #else
            const U8 Brightness = NeoBrightness();
            NeoneoIndex = Index;
        #endif
    #endif
    U8 R,G,B;
    if ( Param->HasR )
        R = Param->R;
    else
        R = 0;

    if ( Param->HasU )
        G = Param->U;
    else
        G = 0;

    if ( Param->HasB )
        B = Param->B;
    else
        B = 0;

    struct LedColor_t Color = {R,G,B};
    #if HAS_WHITE_LED
        U8 W;
        if ( Param->HasW )
            W = Param->W;
        else
            W = 0;
        Color.W = W;
    #endif
    #if NEOPIXEL_LED
        if ( Param-> HasP )
            Color.I = Param->P;
        else
            Color.I = Brightness;
    #endif

    #ifdef NEOPIXEL2_SEPARATE
        if ( Unit == 1 )
        {
            Leds2SetColor(&Color);
            return ;
        }
    #endif
        LedsSetColor(&Color);
    return 1;
}
S32 GcodeM150Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
