#include "GcodeM149Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrHeaterManager.h"

#define CMD_M149        "M149"


S32 GcodeM149Parse(const S8 *CmdBuff, VOID *Arg)
{
    printf("M149\n");
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M149) != 0 )  //传进来的命令不是G0命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    S32 Offset = strlen(CMD_M149) + sizeof(S8); //加1是越过分隔符
    struct ArgM149_t *CmdArg = (struct ArgM149_t *)Arg;

    S8 Opt[20] = {0};
    S32 OptLen = 0;
    while (Offset < Len)
    {
        memset(Opt,0,sizeof(Opt));
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            //printf(" Cmd get opt failed  cmd = %s\n", CmdBuff);
            break;    //获取参数失败
        }
        OptLen = strlen(Opt);

        if ( OptLen <= 1 )
        {
            printf("Gcode M149 has no param opt = %s\n", CmdBuff);
        }
        switch (Opt[0])
        {
            case OPT_C:
                CmdArg->HasC = 1;
            break;
            case OPT_F:
                CmdArg->HasF = 1;
            break;
            case OPT_K:
                CmdArg->HasK = 1;
            break;
        }
        
        Offset += strlen(Opt) + sizeof(S8);
        
    }
    return 1;
}

S32 GcodeM149Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !BedHeater || (HOTENDS < 1))
        return 0;

    if ( !HotEndArr[0] )
        return 0;

    struct ArgM149_t *Param = (struct ArgM149_t *)ArgPtr;
    enum TemperUnits_t Unit = CELSIUS;
    if ( Param->HasF )
    {
        Unit = FAHRENHEIT;
    }
    else if ( Param->HasK )
    {
        Unit = THERMODYNAMIC;
    }

    HotEndArr[0]->SetTemperUnit(Unit);
    BedHeater->SetTemperUnit(Unit);

    return 1;
}

S32 GcodeM149Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


