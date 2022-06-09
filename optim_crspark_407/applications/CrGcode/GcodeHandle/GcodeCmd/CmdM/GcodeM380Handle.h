#ifndef _CRGCODEM380HANDLE_H
#define _CRGCODEM380HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SOLENOID_COUNT  5   //挤出机最大数量

S32 GcodeM380Parse(const S8 *CmdBuff, VOID *Arg);       //M380命令的解析函数
S32 GcodeM380Exec(VOID *ArgPtr, VOID *ReplyResult);     //M380命令的执行函数
S32 GcodeM380Reply(VOID *ReplyResult, S8 *Buff);            //M380命令的回复函数
U8 GetActiveExtruderIndex(void);                            //获取活动的挤出机的编号
S32 GetFdByExtruderIndex(U8 ActiveExtruderIndex);   //根据挤出机编号获取fd

#define MANUAL_SOLENOID_CONTROL     // Manual control of docking solenoids with M380 S / M381

#ifdef __cplusplus
}
#endif

#endif
