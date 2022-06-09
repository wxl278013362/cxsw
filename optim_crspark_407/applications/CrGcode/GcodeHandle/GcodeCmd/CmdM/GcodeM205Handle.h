#ifndef _GCODEM205HANDLE_H_
#define _GCODEM205HANDLE_H_

#include "CrInc/CrType.h"

/* M205指令
      运动的高级设置
   M205 [B<µs>] [E<jerk>] [J<deviation>] [S<units/s>] [T<units/s>] [X<jerk>] [Y<jerk>] [Z<jerk>]
 **/
#ifdef __cplusplus
extern "C" {
#endif

//VOID GcodeM205Init(VOID *Model);

S32 GcodeM205Parse(const S8 *CmdBuff, VOID *Arg);     //M205命令的解析函数
S32 GcodeM205Exec(VOID *ArgPtr, VOID *ReplyResult);   //M205命令的执行函数
S32 GcodeM205Reply(VOID *ReplyResult, S8 *Buff);      //M205命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM205HANDLE_H_ */
