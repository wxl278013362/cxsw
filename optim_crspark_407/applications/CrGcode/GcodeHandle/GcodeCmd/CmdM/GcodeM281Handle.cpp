#include "GcodeM281Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "CrModel/CrMotion.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrModel/CrServo.h"
#include "CrGpio/CrGpio.h"

#ifdef EDITABLE_SERVO_ANGLES

extern uint16_t servo_angles[NUM_SERVOS][2];
#define CMD_M281 ("M281")

S32 GcodeM281Parse(const S8 *CmdBuff, VOID *Arg)
{
    /*M281 [L<degrees>] P<index> [U<degrees>] P不可省略，U、L若省略则输出索引对应伺服的位置*/
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M281, strlen(CMD_M281)) != 0 )
    {
        return 0;
    }

    struct ArgM281_t *CmdArg = (struct ArgM281_t *)Arg;
    S32 Offset = strlen(CMD_M281) + 1; //加1是越过分隔符
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
            printf("Gcode M281 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_P:
            {
                CmdArg->HasP = true;
                if ( WITHIN(atoi(Opt + 1), 0, NUM_SERVOS - 1) )
                {
                    CmdArg->P = atoi(Opt + 1);
                }
                else
                {
                    return CrErr;
                }
                break;
            }

            case OPT_U:
            {
                CmdArg->HasU = true;
                CmdArg->U = atoi(Opt + 1);
                break;
            }

            case OPT_L:
            {
                CmdArg->HasL = true;
                CmdArg->L = atoi(Opt + 1);
                break;
            }

            default:
            {
                printf("Gcode M281 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}


S32 GcodeM281Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    struct ArgM281_t *Param = (struct ArgM281_t *)ArgPtr;
    if ( !Param->HasP )
    {
        return CrErr;
    }

#ifdef BLTOUCH
    if ( Param->P == Z_PROBE_SERVO_NR )
    {
        printf("BLTouch angles can't be changed.\n");

        return CrErr;
    }
#endif
    BOOL Angle_Change = false;
    if ( Param->HasL )
    {
        servo_angles[Param->P][0] = Param->L;
        Angle_Change  = true;
    }
    if ( Param->HasU )
    {
        servo_angles[Param->P][1] = Param->U;
        Angle_Change  = true;
    }

    if ( !Angle_Change )
    {
        printf("Servo : %d, L : %d, U : %d\n", Param->P, servo_angles[Param->P][0], servo_angles[Param->P][1]);
    }

    return GCODE_EXEC_OK;

}

S32 GcodeM281Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

#endif
