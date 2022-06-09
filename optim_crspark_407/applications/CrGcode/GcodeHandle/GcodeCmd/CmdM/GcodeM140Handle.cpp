#include "GcodeM140Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrHeaterManager.h"

#define CMD_M140        "M140"
CrHeaterManager *M140BedHeater = NULL;

S32 GcodeM140Parse(const S8 *CmdBuff, VOID *Arg)
{
    //printf("M140\n");
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是G
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M140) != 0 )  //传进来的命令不是G0命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }

    S32 Offset = strlen(CMD_M140) + sizeof(S8); //加1是越过分隔符
    struct ArgM140_t *CmdArg = (struct ArgM140_t *)Arg;

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
            printf("Gcode G0 has no param opt = %s\n", CmdBuff);
        }
        switch (Opt[0])
        {
            case OPT_I:
                    
                CmdArg->HasI = 1;
                    
                if (OptLen > 1)
                {
                    CmdArg->I = atoi(Opt + 1);
                }
                    
            break;
            case OPT_S:

                CmdArg->HasS = 1;

                if (OptLen > 1)
                {
                    CmdArg->S = atoi(Opt + 1);
                }
            break;
        }
        
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

S32 GcodeM140Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    //printf("Cmd M140 arg = %p\n", ArgPtr);
    struct ArgM140_t *Param = (struct ArgM140_t *)ArgPtr;


    if(Param->HasS ||Param->HasI)
    {
        /* get Bed pointer.*/
        S32 index = Param->S;
        if(Param->HasI)
        {
            index = Param->I;
        }
        /* set Bed temperature.*/
        if ( BedHeater )
        {
            struct HeaterManagerAttr_t Attr = {0};
            Attr = BedHeater->CrHeaterMagagerGetAttr();
            Attr.TargetTemper = index;
            BedHeater->CrHeaterMagagerSetAttr(Attr);
        }
        else
        {
            printf("M140 Bed heater is null!\n");
        }
        
    }
    else
    {
        printf("has no target temperature.\n");
    }
    return 1;
}

S32 GcodeM140Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}


