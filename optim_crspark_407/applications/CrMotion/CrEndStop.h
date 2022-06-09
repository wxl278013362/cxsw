/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date              Author                 Notes
 * 2020-12-21     TingFengXuan       the first version
 */

#ifndef _CRENDSTOP_H_
#define _CRENDSTOP_H_

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

enum EndStopCmd
{
    CRENDSTOP_SET_ENABLE,       //限位开关开始检测
    CRENDSTOP_SET_DISABLE,      //限位开关关闭检测
    CRENDSTOP_SET_TRAGLEVEL,    //设置限位开关触发电平
    CRENDSTOP_GET_TRAGLEVEL,    //获取限位开关触发电平
    CRENDSTOP_GET_TRAGSTATUS    //获取限位开关状态
};

#define ENDSTOPX_PIN_NAME "PE1" //ender-3 "PA5"
#define ENDSTOPY_PIN_NAME "PE0" //ender-3 "PA6"
#define ENDSTOPZ_PIN_NAME "PG12"  //ender-3 "PA7"

/**
 * 函数功能: 打开限位开关描述符
 * 函数参数:Name: 限位开关对应引脚名称,Flag:无实际意义,Mode:无实际意义
 * 输入: Name: 限位开关对应引脚名称,
 * 输出: 参考返回值
 * 返回值: 限位开关描述符
 * 注: 打开限位开关成功后，要设置开关的触发电平
 */
S32 CrEndStopOpen(S8 *Name, S32 Flag, S32 Mode);  //Name必须是常量字符串
/**
 * 函数功能: 关闭限位开关描述符
 * 函数参数:Fd:限位开关描述符
 * 输入: Fd:限位开关描述符
 * 输出: 无
 * 返回值: 无
 */
VOID CrEndStopClose(S32 Fd);
/**
 * 函数功能: 限位开关输入输出控制
 * 函数参数:Fd:限位开关描述符,Cmd: 控制指令,Arg:拓展参数
 * 输入: Fd:限位开关描述符,Cmd: 控制指令,Arg:拓展参数
 * 输出: Arg:拓展参数
 * 返回值: 小于0: 失败,0:成功
 */
S32 CrEndStopIoctl(S32 Fd, S32 Cmd, VOID *Arg);
/**
 * 函数功能: 限位开关应用层处理,限位开关未触发时,清除触发标识
 * 函数参数: 无
 * 输入: 无
 * 输出: 限位开关未触发时,清除触发标识
 * 返回值: 无
 */
VOID EndStopHandler();


#ifdef __cplusplus
}
#endif

#endif 
