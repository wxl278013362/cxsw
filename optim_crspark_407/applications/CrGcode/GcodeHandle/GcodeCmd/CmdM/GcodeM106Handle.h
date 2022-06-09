/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     GcodeM106Handle.h
  Author :        FuMin
  Version:        1.0
  Description:    M106 GCODE Command
  Other:
  Modify History:
          <author>        <time>      <version>         <desc>
          FuMin         2021-01-21     V1.0.0.1
          cx2470        2020-10-28     first version
****************************************************************************/

#ifndef _GCODEM106HANDLE_H_
#define _GCODEM106HANDLE_H_

#include "CrInc/CrType.h"

/* M106指令
      设置风扇速度
 **/
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM106Parse(const S8 *CmdBuff, VOID *Arg);     //M106命令的解析函数
S32 GcodeM106Exec(VOID *ArgPtr, VOID *ReplyResult);   //M106命令的执行函数
S32 GcodeM106Reply(VOID *ReplyResult, S8 *Buff);      //M106命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM106HANDLE_H_ */
