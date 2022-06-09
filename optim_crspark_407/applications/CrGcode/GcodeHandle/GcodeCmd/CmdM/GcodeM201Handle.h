#ifndef _GCODEM201HANDLE_H_
#define _GCODEM201HANDLE_H_

#include "CrInc/CrType.h"

/* M201指令
     设置打印时最大加速度(Units/s^2) 对XYZE轴
   M201 [E<accel>] [T<index>] [X<accel>] [Y<accel>] [Z<accel>]
 **/
#ifdef __cplusplus
extern "C" {
#endif

//VOID GcodeM201Init(VOID *Model);

S32 GcodeM201Parse(const S8 *CmdBuff, VOID *Arg);     //M201命令的解析函数
S32 GcodeM201Exec(VOID *ArgPtr, VOID *ReplyResult);   //M201命令的执行函数
S32 GcodeM201Reply(VOID *ReplyResult, S8 *Buff);      //M201命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM201HANDLE_H_ */
