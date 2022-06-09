/****************************************************************************
  Copyright(C)    2021,      Creality Co. Ltd.
  File name :     GcodeM107Handle.h
  Author :        FuMin
  Version:        1.0
  Description:    M107 GCODE Command
  Other:
  Modify History:
          <author>        <time>      <version>         <desc>
          FuMin         2021-01-21     V1.0.0.1
          cx2470        2020-10-28     first version
****************************************************************************/

#ifndef _GCODEM107HANDLE_H_
#define _GCODEM107HANDLE_H_

#include "CrInc/CrType.h"

/* M107指令
      风扇关
 **/
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM107Parse(const S8 *CmdBuff, VOID *Arg);     //M107命令的解析函数
S32 GcodeM107Exec(VOID *ArgPtr, VOID *ReplyResult);   //M107命令的执行函数
S32 GcodeM107Reply(VOID *ReplyResult, S8 *Buff);      //M107命令的回复函数

#ifdef __cplusplus
}
#endif




#endif /* _GCODEM107HANDLE_H_ */
