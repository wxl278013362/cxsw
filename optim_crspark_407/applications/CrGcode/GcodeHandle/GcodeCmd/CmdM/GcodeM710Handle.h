/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     GcodeM710Handle.h
  Author :        FuMin
  Version:        1.0
  Description:    M710 Gcode Command
  Other:
  Modify History:
          <author>        <time>      <version>     <desc>
          FuMin         2021-01-21     V1.0.0.1
****************************************************************************/

#ifndef _GCODEM710HANDLE_H_
#define _GCODEM710HANDLE_H_

#include "CrInc/CrType.h"

/*
 M710指令功能：控制器风扇设置
 */

#ifdef __cplusplus
extern "C" {
#endif

VOID GcodeM710Init(VOID *Model);

S32 GcodeM710Parse(const S8 *CmdBuff, VOID *Arg);     //M710命令的解析函数
S32 GcodeM710Exec(VOID *ArgPtr, VOID *ReplyResult);   //M710命令的执行函数
S32 GcodeM710Reply(VOID *ReplyResult, S8 *Buff);      //M710命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM710HANDLE_H_ */
