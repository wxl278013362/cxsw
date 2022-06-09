/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     GcodeG20Handle.h
  Author :        FuMin
  Version:        1.0
  Description:    G20 GCODE Command
  Other:
  Modify History:
          <author>        <time>      <version>         <desc>
          FuMin         2021-01-22     V1.0.0.1
****************************************************************************/

#ifndef _GCODEG20HANDLE_H_
#define _GCODEG20HANDLE_H_

#include "CrInc/CrType.h"

/* G20指令
     设置单位为英寸
     在此模式下，GCODE指令参数中指定的所有位置、偏移、速率、加速度等都被解释为英寸。
 */
#ifdef __cplusplus
extern "C" {
#endif

VOID GcodeG20Init(VOID *Model);
S32 GcodeG20Parse(const S8 *CmdBuff, VOID *Arg);    //G20命令的解析函数
S32 GcodeG20Exec(VOID *ArgPtr, VOID *ReplyResult);  //G20命令的执行函数
S32 GcodeG20Reply(VOID *ReplyResult, S8 *Buff);     //G20命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEG20HANDLE_H_ */



