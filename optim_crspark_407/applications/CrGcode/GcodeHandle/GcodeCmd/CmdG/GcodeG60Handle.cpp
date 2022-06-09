#include "GcodeG60Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "CrMotion/CrMotionPlanner.h"
#include "CrMotion/StepperApi.h"
#include "CrModel/CrMotion.h"

#define CMD_G60          "G60"

S32 GcodeG60Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_G) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_G60) != 0 )  //传进来的命令不是G0命令
    {
        return 0;
    }

    S32 Offset = strlen(CMD_G60) + sizeof(S8); //加1是越过分隔符
    struct ArgG60_t *CmdArg = (struct ArgG60_t *)Arg;

    while ( Offset < Len )  //处理结束
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
            break;    //获取参数失败

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
            printf("Gcode G60 has no param opt = %s\n", CmdBuff);
        
        switch ( Opt[0] )  //判断命令
        {
            case OPT_S:
            {
                CmdArg->HasS = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->S = atoi(Opt + 1);
                }

                break;
            }
            default:
            {
                printf("Gcode G60 has no define opt = %s\n", CmdBuff);
                break;
            }
        }
        
        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }
    return 1;
}


S32 GcodeG60Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !MotionModel)
        return 0;

    /*FIXME:并将当前点保存到内存插槽中*/
    struct ArgG60_t *Arg = (struct ArgG60_t *)ArgPtr;
    S32 Index = 0;
    if ( Arg->HasS )
        Index = Arg->S;

#ifdef SLOT_COUNT
    if ( Index >= SLOT_COUNT )
        return 1;
    
    struct MotionPositionMsg_t Pos = MotionModel->GetCurPosition();
    StoredPos[Index].CoorX = Pos.PosX;
    StoredPos[Index].CoorY = Pos.PosY;
    StoredPos[Index].CoorZ = Pos.PosZ;
#endif

    return 1;
}

S32 GcodeG60Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


