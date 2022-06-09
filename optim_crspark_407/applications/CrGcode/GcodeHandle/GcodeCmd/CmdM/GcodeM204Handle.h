#ifndef _GCODEM204HANDLE_H_
#define _GCODEM204HANDLE_H_

#include "CrInc/CrType.h"

/* M204指令
     设置启动时各种移动的加速度
   M204 [P<accel>] [R<accel>] [S<accel>] [T<accel>]
     旧版S<accel>已弃用
 **/
#ifdef __cplusplus
extern "C" {
#endif

//VOID GcodeM204Init(VOID *Model);

S32 GcodeM204Parse(const S8 *CmdBuff, VOID *Arg);     //M204命令的解析函数
S32 GcodeM204Exec(VOID *ArgPtr, VOID *ReplyResult);   //M204命令的执行函数
S32 GcodeM204Reply(VOID *ReplyResult, S8 *Buff);      //M204命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM204HANDLE_H_ */
