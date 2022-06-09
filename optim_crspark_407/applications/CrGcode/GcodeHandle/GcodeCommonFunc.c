#include "GcodeCommonFunc.h"
#include "GcodeType.h"
#include <string.h>
#include <rtthread.h>

//获取命令一个操作的内容
S32 GetCmdOpt(const S8 *Buff, S8 *Opt)
{
    if ( !Buff || !Opt )
        return 0;

    S16 Len = strlen(Buff);
    if ( (Len <= 0) || (Buff[0] == NOTE_CHAR) )  //是注释或者没有操作的
        return 0;

    S8 *Separate = strchr(Buff, CMD_ARG_SEPARATE); //要获取操作数
    int Count = 0;
    if ( !Separate )   //没有分隔符
    {
        if ( Buff[Len - 1] == '\n' )   //注意有时候会有换行符
        {
            Count = Len - 1;
        }
        else
            Count = Len;
    }
    else
    {
        Count = (Separate - Buff) / sizeof(S8);  //获取字符端
    }

    strncpy(Opt, Buff, Count);
    Opt[Count] = '\0'; //结束符

    return 1;
}

S32 GetGcodeCmd(const S8 *Buff, S8 *Cmd)
{
    if ( !Buff || !Cmd )
        return 0;

    S16 Len = strlen(Buff);
    if ( (Len <= 1) || (Buff[0] == NOTE_CHAR ) )  //是注释或内容不足以构成一条命令
        return 0;

    S8 *Separate = strchr(Buff, CMD_ARG_SEPARATE); 
    if ( !Separate )  //只有命令，没有参数    (没有对非正常命令进行处理)
    {
//        printf("%s %d: cmd = %s\n", __FUNCTION__, __LINE__, Buff);
        strcpy(Cmd, Buff);
        S32 Len = strlen(Cmd);
//        rt_kprintf("line %d: cmd = %s, Len = %d\n",  __LINE__, Cmd, Len);
        if ( Len )
        {
#if 0
            rt_kprintf("\r\n modify line %d, change char = %0x\r\n",  __LINE__, '\n');
            S32 i = 0;
            for ( i = 0; i < Len ; i++ )
            {
                rt_kprintf("%0x ", Cmd[i]);
            }
            rt_kprintf("\r\n",  __LINE__);
#endif
            if ( Cmd[Len - 1] == '\n' )
            {
                Cmd[Len - 1] = '\0';
                rt_kprintf("modify line %d: cmd = %s, Len = %d\n",  __LINE__, Cmd, Len);
            }
        }
    }
    else
    {
        S32 Count = (Separate - Buff) / sizeof(S8);
        strncpy(Cmd, Buff, Count);
        Cmd[Count] = '\0';


        if ( strcmp(Cmd, CMD_M43) == 0 )  //对M43有特殊处理
        {
            if ( *(Buff + Count + 1) == 'T' ) //参数是T表明可能是命令"43 T"
            {
                if ( *(Buff + Count + 2) == '\0'     //T后面是字符串结束符
                    || *(Buff + Count + 2) == CMD_ARG_SEPARATE )  //T后面是命令分隔符
                {
                    Cmd[Count] = CMD_ARG_SEPARATE;
                    Cmd[Count + 1] = 'T';
                    Cmd[Count + 2] = '\0';
                }
            }
        }
    }

    return 1;
}


