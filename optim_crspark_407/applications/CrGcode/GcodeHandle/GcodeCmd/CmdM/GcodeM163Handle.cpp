#include "GcodeM163Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrGcode.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"

#ifdef MIXING_EXTRUDER

#define WITHIN(N,L,H)       ((N) >= (L) && (N) <= (H))
#define CMD_M163            "M163"

// MARK:最终应该在混合挤出机模块定义并添加其他的必须操作
class Mixer
{
    public:
        static float Collector[MIXING_STEPPERS];
        static void SetCollector(const uint8_t Index, const float Factor)
        {
            Collector[Index] = Factor;
        }
    private:
};

float Mixer::Collector[MIXING_STEPPERS] = {0};
class Mixer Mixer;
// M164直接使用这个数组，但是不应该这样做，应该像Marlin一样通过类来管理，这儿引出给164使用验证
float *Collector = static_cast<float *>(Mixer.Collector);

/**
 * 解析M163命令
 *
 * @param CmdBuff 上位机传来的命令Buff
 * @param Arg 保存解析到的命令参数
 * @return 解析成功返回1，否则返回0
 */
S32 GcodeM163Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
    {
        return 0;
    }

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_M163) != 0 )
    {
        return 0;
    }

    struct ArgM163_t *CmdArg = (struct ArgM163_t *)Arg;
    memset((char *)CmdArg, 0, sizeof(struct ArgM163_t));

    S32 Offset = strlen(CMD_M163) + 1; //加1是越过分隔符
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
            printf("Gcode M163 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_S:
            {
                // 获取要设置混合因子的挤出头的电机
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                {
                    CmdArg->S = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->S = INT16_MAX;
                }
                break;
            }
            case OPT_P:
            {
                // 获取要设置的混合因子
                CmdArg->HasP = true;
                if ( OptLen > 1 )
                {
                    CmdArg->P = atoff(Opt + 1);
                }
                else
                {
                    CmdArg->P = 100.0;
                }
                break;
            }
            default:
            {
                printf("Gcode M163 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}

/**
 * 执行M163命令
 * @param ArgPtr M163命令的命令参数
 * @param ReplyResult 执行完命令需要回复的内容
 * @return 执行成功返回1，否则返回0
 */
S32 GcodeM163Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return 0;
    }

    struct ArgM163_t *Param = (struct ArgM163_t *)ArgPtr;
    if ( !Param->HasP || !Param->HasS)
    {
        return 0;
    }

    if ( !WITHIN(Param->S, 0, MIXING_STEPPERS - 1) )
    {
        return 0;
    }

    if ( !WITHIN(Param->P, 0, 1.0) )
    {
        return 0;
    }

    Mixer.SetCollector(Param->S, Param->P);

    return 1;
}

S32 GcodeM163Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
    {
        return Len;
    }

    return Len;
}

#endif
