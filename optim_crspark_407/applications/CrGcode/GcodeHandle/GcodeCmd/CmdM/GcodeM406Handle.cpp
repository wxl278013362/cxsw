#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "GcodeM405Handle.h"
#include "GcodeM406Handle.h"
#include "Common/Include/CrTime.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrGcode/GcodeHandle/GcodeType.h"


#define CMD_M406 "M406"

/******额外定义的变量和函数***********/
BOOL VolumetricEnabled = false;                 //在parser中提供的用于定义是否存在的
FLOAT VolumetricMultiplier[EXTRUDERS] = {0};    //料丝横截面积的倒数
FLOAT FilamentSize[EXTRUDERS] = {0};
FLOAT VolumetricExtruderLimit[EXTRUDERS] ={0};  //挤出机可处理的最大挤出速度
FLOAT VolumetricExtruderFeedrateLimit[EXTRUDERS] = {0};    //基于体积_挤出机_限制预先计算的挤出机进给速度限制
FLOAT FactorE[EXTRUDERS] = {1};
FLOAT FlowPerCentage[EXTRUDERS] = {100};        //每个挤出头的挤出系数

VOID FilamentDisable(const BOOL Flag)
{
    //1、状态置为disable
    SensorFlag = Flag;

    //2、启用时需要再添加相应的传感器开启函数
}

FLOAT CalCulateVolvumetricMultiplier(FLOAT Diameter)
{
    if ( VolumetricEnabled && Diameter )
    {
        return 1.0f / M_PI * powf(Diameter * 0.5f, 2);
    }
    else
    {
        return 1;
    }
}

VOID RefreshFactorE(const U8 E)
{
#ifdef NO_VOLUMETRICS
    FactorE[E] = FlowPerCentage[E] * 0.01 * 1.0f;
#else
    FactorE[E] = FlowPerCentage[E] * 0.01 * VolumetricMultiplier[E];
#endif
}

VOID CalculateVolumetricExtruderLimit(U8 E)
{
   FLOAT &Lim = VolumetricExtruderLimit[E];
   FLOAT &Siz = FilamentSize[E];
   VolumetricExtruderFeedrateLimit[E] = (Lim && Siz) ? Lim / M_PI *powf(Siz * 0.5,2) : 0;
}

VOID CalculateVolumetricExtruderLimits()
{
    for ( int i = 0; i < EXTRUDERS; i++ )
    {
        CalculateVolumetricExtruderLimit(i);
    }
}

VOID CalCulateVolumetricMultipliers()
{
    for ( int i = 0; i < sizeof(FilamentSize)/sizeof(FLOAT); i++ )
    {
        VolumetricMultiplier[i] = CalCulateVolvumetricMultiplier(FilamentSize[i]);
        RefreshFactorE(i);
    }
#ifdef VOLUMETRIC_EXTRUDER_LIMIT

#endif
    CalculateVolumetricExtruderLimits();
}


/******额外定义的变量和函数***********/

S32 GcodeM406Parse(const S8 *CmdBuff, VOID *Arg)
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

    if ( strcmp(Cmd,CMD_M406) != 0 )    //判断是否是M406命令
    {
        return 0;
    }
    return 1;
}

S32 GcodeM406Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    //1、关闭料丝传感器
    FilamentDisable(false);

    //2、恢复正确的体积、乘数积
    CalCulateVolumetricMultipliers();

    return 1;
}

S32 GcodeM406Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
