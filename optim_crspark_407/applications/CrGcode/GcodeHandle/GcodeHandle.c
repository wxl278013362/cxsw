#include "GcodeHandle.h"
#include "GcodeList.h"
#include "GcodeType.h"
#include "CrLog/CrLog.h"

#include <string.h>


#define NOTE_CHAR ';'
#define CMD_M43       "M43"

//#define NULL          (0)

U8 IsRelativeMode = 0;
static S32 GcodeBuffToCmd(S8 *Buff, S8 *Cmd);
//static struct GcodeItem_t *GcodeFindItem(struct GcodeItem_t *ItemList, S8 *Buff, S32 Len);
const struct GcodeItem_t *GcodeFindItem(S8 *Buff);

S32 GcodeHandle(S8 *Buff, S32 Len, S8 *Reply)
{
    //命令合法性检查
    //rt_kprintf("+++++++++++++++++++++++++++++++++GcodeHandle is null or len <= 1 Buf:%s len:%d\n",Buff,Len);
    if ( !Buff || (Len <= 1) )
    {
        //rt_kprintf("Buff is null or len <= 1 Buf:%s len:%d\n",Buff,Len);
        //LOG("Buff is null or len <= 1 Buf:%s len:%d\n",Buff,Len);
        return GCODE_CMD_ERROR;
    }
    S32  ret = 0;
    //是否为能处理的命令
    if ( (Buff[0] != GCODE_CMD_G) && (Buff[0] != GCODE_CMD_M)
        && (Buff[0] != GCODE_CMD_T) )
    {
        rt_kprintf("%s Cmd Not Support!\n",Buff[0]);
        //LOG("%s Cmd Not Support!\n",Buff[0]);
        return GCODE_NOT_SUPPOR;
    }
    //rt_kprintf("+++++++++++++++++++++++++++++++++GcodeHandle is null or len <= 1 Buf:%s len:%d\n",Buff,Len);
    const struct GcodeItem_t *Item = GcodeFindItem(Buff);
    if ( !Item )
    {
        //rt_kprintf("Cmd %s Not handle ", Buff);
        return GCODE_NOT_FOUND;  //找不到命令直接返回成功跳过
    }
    if ( !(Item->ParseFunc) || !(Item->ExecFunc) )
    {
        //rt_kprintf("ParseFunc or ExecFunc is NULL\n");
        //LOG("ParseFunc or ExecFunc is NULL\n");
        return GCODE_CMD_ERROR;
    }
    S8 ArgBuff[MAX_ARG_LENGTH] = {0};
    if ( !(Item->ParseFunc((S8*)Buff, ArgBuff)) )  //解析失败
    {
        //LOG("Parse Cmd:%s Failed!\n",(S8 *)Buff);
        return GCODE_PARSE_FAIL;
    }
    S8  ReplyResult[128] = {0};
    if ( !(ret = Item->ExecFunc(ArgBuff, (VOID*)ReplyResult)) )   //执行失败
    {
        //LOG("Exec Cmd:%s Failed!\n",(S8 *)Buff);
        return GCODE_EXEC_FAIL;
    }

    if ( Item->ReplyFunc && Reply )
    {
        if ( Item->ReplyFunc((VOID*)ReplyResult, Reply) )  //回复失败
        {
            //LOG("Reply Cmd:%s Failed!\n",(S8 *)Buff);
            return GCODE_REPLY_FAIL;
        }
    }

    return ret;
}

static S32 GcodeBuffToCmd(S8 *Buff, S8 *Cmd)
{
    if ( !Buff || !Cmd )
    {
        return 0;
    }
    S16 Len = strlen(Buff);
    if ( (Len <= 1) || (Buff[0] == NOTE_CHAR ) )  //是注释或内容不足以构成一条命令
    {
        return 0;
    }
    S8 *Separate = strchr(Buff, CMD_ARG_SEPARATE); 
    if ( !Separate )  //只有命令，没有参数    (没有对非正常命令进行处理)
    {
        strcpy(Cmd, Buff);
        S32 Len = strlen(Cmd);
        if ( Len )
        {
            if ( Cmd[Len - 1] == '\n' )
            {
                Cmd[Len - 1] = '\0';
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

const struct GcodeItem_t *GcodeFindItem(S8 *Buff)
{
    
    if ( !Buff )
    {
        return NULL;
    }
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if( !GcodeBuffToCmd(Buff, Cmd) )
    {
        return NULL;
    }
    
    S32 Index = 0;
    struct GcodeItem_t * Item = &GcodeList[Index ++];
    while ( Item->Cmd )
    {
        if ( strcmp(Item->Cmd, Cmd) == 0 )
        {
            break;
        }
        Item = &GcodeList[Index ++];
    }

    if ( !(Item->Cmd) )
    {
        return NULL;
    }
    return Item;
}

