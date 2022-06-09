#include "GcodeM540Handle.h"
#include <stdlib.h>
#include "CrModel/CrMotion.h"
#include "CrModel/CrPrinter.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"

#define CMD_M540 ("M540")

S32 GcodeM540Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S8 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是m
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M540) != 0 )  //传进来的命令不是G0命令
        return 0;

    S8 Offset = strlen(CMD_M540) + sizeof(S8); //加1是越过分隔符
    struct ArgM540_t *CmdArg = (struct ArgM540_t *)Arg;

    S8 Opt[20] = {0};
    S8 OptLen = 0;

    while ( Offset < Len )
    {
        memset(Opt, 0, sizeof(Opt));
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
            break;    //获取参数失败

        OptLen = strlen(Opt);

        switch ( Opt[0] )
        {
            case OPT_S:

                CmdArg->HasS = true;

                if (OptLen > 1)
                    CmdArg->S = atoi(Opt + 1);
            break;

        }
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

S32 GcodeM540Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel || (HOTENDS < 1) )
        return 0;

    struct ArgM540_t *Param = (struct ArgM540_t *)ArgPtr;

    if ( Param->HasS )
    {
        if ( Printer )
            Printer->SetPrintStatus(STOP_PRINT);

        struct PwmControllerAttr_t Attr;
        memset(&Attr, 0, sizeof( struct PwmControllerAttr_t));
        if ( (HOTENDS >= 1) && HotEndArr[0] )
            HotEndArr[0]->GetHeater()->CrHeaterSetAttr(Attr);

        if ( BedHeater )
            BedHeater->GetHeater()->CrHeaterSetAttr(Attr);

        if ( MotionModel )
            MotionModel->StopMotionUrgently();
    }

    return 1;
}

S32 GcodeM540Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
