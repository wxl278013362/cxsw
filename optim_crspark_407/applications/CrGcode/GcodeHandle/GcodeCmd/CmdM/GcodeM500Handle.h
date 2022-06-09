#ifndef _CRGCODEM500HANDLE_H
#define _CRGCODEM500HANDLE_H

#include "CrInc/CrType.h"
#ifdef __cplusplus
extern "C" {
#endif

#define EEPROM_USED 1

#ifdef EEPROM_USED

S32 GcodeM500Parse(const S8 *CmdBuff, VOID *Arg); //M500命令的解析函数
S32 GcodeM500Exec(VOID *ArgPtr, VOID *ReplyResult);   //M500命令的执行函数
S32 GcodeM500Reply(VOID *ReplyResult, S8 *Buff);   //M500命令的回复函数

#endif

#ifdef __cplusplus
}
#endif

#endif
