#ifndef _CRGCODEM155HANDLE_H
#define _CRGCODEM155HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM155Init();
VOID SetAutoReportTempInterval(S32 Sec);   //设置自动上报温度的间隔时间
S32 GcodeM155Parse(const S8 *CmdBuff, VOID *Arg); //M104命令的解析函数
S32 GcodeM155Exec(VOID *ArgPtr, VOID *ReplyResult);   //M104命令的执行函数
S32 GcodeM155Reply(VOID *ReplyResult, S8 *Buff);   //M104命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
