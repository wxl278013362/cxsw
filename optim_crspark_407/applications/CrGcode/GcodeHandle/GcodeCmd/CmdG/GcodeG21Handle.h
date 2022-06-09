
/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     GcodeG21Handle.h
  Author :        FuMin
  Version:        1.0
  Description:    G21 GCODE Command
  Other:
  Modify History:
          <author>        <time>      <version>         <desc>
          FuMin         2021-01-22     V1.0.0.1
****************************************************************************/

#ifndef _GCODEG21HANDLE_H_
#define _GCODEG21HANDLE_H_

#include "CrInc/CrType.h"

/* G21指令
     设置单位为毫米
     在此模式下，GCODE指令参数中指定的所有位置、偏移、速率、加速度等都被解释为毫米。
 */
#ifdef __cplusplus
extern "C" {
#endif


S32 GcodeG21Parse(const S8 *CmdBuff, VOID *Arg);    //G21命令的解析函数
S32 GcodeG21Exec(VOID *ArgPtr, VOID *ReplyResult);  //G21命令的执行函数
S32 GcodeG21Reply(VOID *ReplyResult, S8 *Buff);     //G21命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEG21HANDLE_H_ */

