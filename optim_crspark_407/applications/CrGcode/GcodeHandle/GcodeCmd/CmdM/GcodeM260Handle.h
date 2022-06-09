#ifndef _GCODEM260HANDLE_H_
#define _GCODEM260HANDLE_H_

#include "CrInc/CrType.h"

#define TWIBUS_BUFFER_SIZE 32
extern S8 ByteCount;
extern S8 I2CBuff[TWIBUS_BUFFER_SIZE];

#ifdef __cplusplus
extern "C" {
#endif

VOID ResetI2C();
S32 GcodeM260Parse(const S8 *CmdBuff, VOID *Arg);     //M260命令的解析函数
S32 GcodeM260Exec(VOID *ArgPtr, VOID *ReplyResult);   //M260命令的执行函数
S32 GcodeM260Reply(VOID *ReplyResult, S8 *Buff);      //M260命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM260HANDLE_H_ */
