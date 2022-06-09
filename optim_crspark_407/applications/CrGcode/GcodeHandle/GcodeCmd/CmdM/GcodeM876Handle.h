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

#ifndef _GCODEM876HANDLE_H_
#define _GCODEM876HANDLE_H_

#include "CrInc/CrType.h"

/*
 M710指令功能：控制器风扇设置
 */

#ifdef __cplusplus
extern "C" {
#endif

enum PromptReason_t : U8 {
  PROMPT_NOT_DEFINED,
  PROMPT_FILAMENT_RUNOUT,
  PROMPT_USER_CONTINUE,
  PROMPT_FILAMENT_RUNOUT_REHEAT,
  PROMPT_PAUSE_RESUME,
  PROMPT_INFO
};

extern PromptReason_t HostPromptReason;
extern const S8 CONTINUE_STR[];

S32 GcodeM876Parse(const S8 *CmdBuff, VOID *Arg);     //M876命令的解析函数
S32 GcodeM876Exec(VOID *ArgPtr, VOID *ReplyResult);   //M876命令的执行函数
S32 GcodeM876Reply(VOID *ReplyResult, S8 *Buff);      //M876命令的回复函数

VOID HostPromptDo(const PromptReason_t reason, const S8 * const Pstr, const S8* const Pbtn =nullptr);

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM710HANDLE_H_ */
