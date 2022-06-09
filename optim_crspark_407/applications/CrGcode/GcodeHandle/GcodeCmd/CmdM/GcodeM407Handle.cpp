#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "GcodeM407Handle.h"
#include "Common/Include/CrTime.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "Common/Include/CrEepromApi.h"


#define CMD_M407 "M407"

/******额外定义的变量和函数***********/
FLOAT MeasuredFilament = DEFAULT_NOMINAL_FILAMENT_DIA; //测量得到的灯丝直径（默认值一般赋预期的灯丝直径）
/******额外定义的变量和函数***********/

S32 GcodeM407Parse(const S8 *CmdBuff, VOID *Arg)
{
    //1、判空
     if ( !CmdBuff || !Arg )
     {
         return 0;
     }

     //2、判断Gcode命令长度是否大于1且是否为M命令
     S32 Len = strlen(CmdBuff);
     if ( Len <= 1 || CmdBuff[0] != GCODE_CMD_M )
     {
         return 0;
     }

     //3、获取命令
     S8 Cmd[CMD_BUFF_LEN] = {0};
     if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )   //防止获取命令失败
     {
         return 0;
     }

     if ( strcmp(Cmd,CMD_M407) != 0 )    //判断是否是M407命令
     {
         return 0;
     }
     return 1;
}

S32 GcodeM407Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    printf("Filament dia (measured mm):%f\n",MeasuredFilament);
    return 1;
}

S32 GcodeM407Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
