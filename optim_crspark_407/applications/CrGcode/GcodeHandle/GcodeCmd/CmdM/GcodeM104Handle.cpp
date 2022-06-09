#include "GcodeM104Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrModel/CrHeaterManager.h"

#define CMD_M104        "M104"

S32 GcodeM104Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是G
        return 0;

    //
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M104) != 0 )  //传进来的命令不是G0命令
    {
        //printf("current cmd is not G91 cmd , is : %s\n", CmdBuff);
        return 0;
    }
    
    S32 Offset = strlen(CMD_M104) + sizeof(S8); //加1是越过分隔符
    struct ArgM104_t *CmdArg = (struct ArgM104_t *)Arg;

    S8 Opt[20] = {0};
    S32 OptLen = 0;
    
    CmdArg->IsDefaultT = 1;
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
            printf("Gcode M104 has no param opt = %s\n", CmdBuff);
        }
        
        switch (Opt[0])
        {
            case OPT_B:
                
                CmdArg->HasB = 1;
                
                if (OptLen > 1)
                {
                    CmdArg->B = atoi(Opt + 1);
                }

            break;
            case OPT_F:

                CmdArg->HasF = 1;
                if (OptLen > 1)
                {
                    CmdArg->F = atoi(Opt + 1);
                }
                
            break;
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
            case OPT_T:
                
                CmdArg->HasT = 1;
                CmdArg->IsDefaultT = 0;
            
                if (OptLen > 1)
                {
                    CmdArg->T = atoi(Opt + 1);
                }
            break;
        }
        Offset += strlen(Opt) + sizeof(S8);
    }
    return 1;
}

S32 GcodeM104Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    //printf("Cmd M104 arg = %p\n", ArgPtr);
    struct ArgM104_t *Param = (struct ArgM104_t *)ArgPtr;
    if ( ( HOTENDS < 1 ) || !HotEndArr[0] )
    {
        printf("M104 Nozzle is null!\n");
        return 1;
    }

    if (Param->IsDefaultT)
    {
        if (Param->HasB)
        {
            printf("Not Support OPT: B \n");
        }
        if (Param->HasF)
        {
            printf("Not Support OPT: F \n");
        }
        if (Param->HasS)
        {
            S32 index = Param->S;
            if (Param->HasI)
            {
                index = Param->I;
            }
            struct HeaterManagerAttr_t Attr = {0};
            Attr = HotEndArr[0]->CrHeaterMagagerGetAttr();
            Attr.TargetTemper = index;
            HotEndArr[0]->CrHeaterMagagerSetAttr(Attr);
        }
    }
    else
    {
        if (!Param->HasT)
        {
            rt_kprintf("has no hotend index.\n");
            return 0;
        }
        if (Param->HasS)
        {
            S32 index = Param->S;
            if (Param->HasI)
            {
                index = Param->I;
            }
            struct HeaterManagerAttr_t Attr = {0};
            Attr = HotEndArr[0]->CrHeaterMagagerGetAttr();
            Attr.TargetTemper = index;
            HotEndArr[0]->CrHeaterMagagerSetAttr(Attr);
        }
        if (Param->HasB)
        {
            printf("Not Support OPT: B \n");
        }
        if (Param->HasF)
        {
            printf("Not Support OPT: F \n");
        }
    }      
    
    if(!(Param->HasB || Param->HasF || Param->HasI || Param->HasS || Param->HasT))
    {
        struct HeaterManagerAttr_t Attr = {0};
        Attr = HotEndArr[0]->CrHeaterMagagerGetAttr();
        Attr.TargetTemper = 0.0f;
        HotEndArr[0]->CrHeaterMagagerSetAttr(Attr);
    }

    return 1;
}

S32 GcodeM104Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

