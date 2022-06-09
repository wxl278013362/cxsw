#include "GcodeM355Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"

#ifdef CASE_LIGHT_ENABLE

#define CMD_M355 ("M355")

#define CONSTRAIN(value, arg_min, arg_max) ((value) < (arg_min) ? (arg_min) :((value) > (arg_max) ? (arg_max) : (value)))

struct CaseLight_t
{
    BOOL    On;
    U8      Brightness;
};

CaseLight_t CaseLight = {0};

VOID CaseLightUpdate(const BOOL sFlag)
{
    //M355 TODO
    //参考Marlin函数 CaseLight::update(const bool sflag)
}

S32 GcodeM355Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M355, strlen(CMD_M355)) != 0 )
    {
        return 0;
    }

    struct ArgM355_t *CmdArg = (struct ArgM355_t *)Arg;
    S32 Offset = strlen(CMD_M355) + 1; //加1是越过分隔符
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
            printf("Gcode M355 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_P:
            {
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
            }

            case OPT_S:
            {
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
            }

            default:
            {
                printf("Gcode M355 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}


S32 GcodeM355Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    BOOL Didset = false;
    struct ArgM355_t *Param = (struct ArgM355_t *)ArgPtr;
#ifdef CASELIGHT_USES_BRIGHTNESS
    if ( Param->HasP && Param->HasValueP )
    {
        Didset = true;
        CaseLight.Brightness = CONSTRAIN(Param->P, 0, 255);
    }
#endif
    const BOOL sFlag = (Param->HasS && Param->HasValueS);
    if ( sFlag )
    {
        Didset = true;
        if ( !Param->S )
        {
            CaseLight.On = true;    //非0和空都为true
        }
        else
        {
            CaseLight.On = false;
        }
    }

    if ( Didset )
    {
        CaseLightUpdate(sFlag);   //更新状态
    }

    // Always report case light status
    if ( !CaseLight.On )
    {
        printf("Case light: OFF\n");
    }
    else
    {
        printf("Case light:ON, brightness: %d\n", CaseLight.Brightness);
    }

    return GCODE_EXEC_OK;

}

S32 GcodeM355Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

#endif
