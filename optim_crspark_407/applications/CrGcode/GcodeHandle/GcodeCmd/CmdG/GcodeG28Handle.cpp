#include "GcodeG28Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrTime.h"
#include "Common/Include/CrSleep.h"
#include "CrInc/CrConfig.h"

#define CMD_G28            "G28"
#define SET_BIT(value,bit)  ((value) | (0x01 << (bit)))

S32 GcodeG28Parse(const S8 *CmdBuff, VOID *Arg)
{
    /*G28 的操作有 O R X Y E */

    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_G) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_G28) != 0 )  //传进来的命令不是G0命令
    {
        return 0;
    }
    
    S32 Offset = strlen(CMD_G28) + sizeof(S8); //加1是越过分隔符
    struct ArgG28_t *CmdArg = (struct ArgG28_t *)Arg;

    while ( Offset < Len )  //处理结束
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;    //获取参数失败
        }

//        S32 OptLen = strlen(Opt);
//        if ( OptLen <= 1 )
//        {
//            printf("Gcode G28 has no param opt = %s\n", CmdBuff);
//        }
        
        switch ( Opt[0] )  //判断命令
        {
            case OPT_O:
            {
                CmdArg->HasO = 1;
                break;
            }
            case OPT_R:
            {
                CmdArg->HasR = 1;
                break;
            }
            case OPT_X:
            {
                CmdArg->HasX = 1;
                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = 1;
                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = 1;
                break;
            }
            default:
            {
                printf("Gcode G28 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }    

    return 1;
}


S32 GcodeG28Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
        return 0;
    U8 Mark = 0;
    struct ArgG28_t *Param = (struct ArgG28_t *)ArgPtr;
    //printf("G28 cmd opts: ");
    if ( Param->HasO )
    {
        // 如果已经各个步进电机的原点位置 可以不进行归位就退出。
        Mark |= HOME_OPT_O;
        //printf("opt O = %d ", Param->HasO);
    }

    if ( Param->HasR )
    {
        // 归位前升高
        Mark |= HOME_OPT_R;
        //printf("opt R = %d ", Param->HasR);
    }

    if ( Param->HasX )
    {
        Mark |= HOME_OPT_X;

        //printf("opt X = %d ", Param->HasX);
    }

    if ( Param->HasY )
    {
        Mark |= HOME_OPT_Y;
        //printf("opt Y = %d ", Param->HasY);
    }

    if ( Param->HasZ )
    {
        Mark |= HOME_OPT_Z;
        //printf("opt Z = %d ", Param->HasZ);
    }
    if (!( Param->HasX ) && !( Param->HasY ) && !( Param->HasZ ))
    {
        Mark = (HOME_OPT_X | HOME_OPT_Y | HOME_OPT_Z);
    }
    if(MotionModel)
    {
        MotionModel->AutoMoveHome(Mark);
    }
    
    return 1;
}


S32 GcodeG28Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


