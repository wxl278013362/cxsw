#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "GcodeM250Handle.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"

#define CMD_M250 "M250"
/***新定义的变量和函数****/

#define LCD_CONTRAST_MIN 0         //定义LCD屏幕对比度的最小值
#define LCD_CONTRAST_MAX 255       //定义LCD屏幕对比度的最大值

S32 GetLCDContrast()
{
    S32 Contrast = 0;
    return Contrast;
}

VOID SetLCDContrast(S32 Contrast)
{

}
/***新定义的变量和函数****/


 S32 GcodeM250Prase(const S8 *CmdBuff, VOID *Arg)
{
    //1、进入函数判指针是否为空
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    //2、判断Gcode命令长度是否大于1且是否为M命令
    S32 Length = strlen(CmdBuff);
    if ( Length <= 1 || CmdBuff[0] != GCODE_CMD_M )
    {
        return 0;
    }

    //3、获取命令
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )   //防止获取命令失败
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_M250) != 0 )    //判断是否是M250命令
    {
        return 0;
    }

    //4、取参数
    S32 Offset = strlen(CMD_M250) + sizeof(S8);
    struct ArgM250_t *CmdArg = (struct ArgM250_t *)Arg;

    S32 OptLen = 0;
    while ( Offset < Length )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;
        }

        OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M250 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_C:
            {
                CmdArg->HasC = true;
                if ( OptLen > 1 )
                {
                    CmdArg->C = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->C = 0;
                }
                break;
            }

            default :
                break;
        }
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

S32 GcodeM250Exec(VOID *ArgPtr,VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM250_t *Param = (struct ArgM250_t *)ArgPtr;
    if ( false == Param->HasC ) //没有传入C，结束执行
    {
        return 1;
    }
    //1、设置LCD对比度的值
    if ( Param->C > LCD_CONTRAST_MAX )
    {
        Param->C =  LCD_CONTRAST_MAX;
    }
    else if ( Param->C < LCD_CONTRAST_MIN )
    {
        Param->C = LCD_CONTRAST_MIN;
    }
    SetLCDContrast(Param->C);

    //2、发送提示消息
    printf("LCD Contrast: %d.\n",Param->C);

    return 1;
}

S32 GcodeM250Reply(VOID *ReplyReslut, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
