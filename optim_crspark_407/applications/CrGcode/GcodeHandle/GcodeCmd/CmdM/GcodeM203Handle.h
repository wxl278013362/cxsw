#ifndef _GCODEM203HANDLE_H_
#define _GCODEM203HANDLE_H_

#include "CrInc/CrType.h"

/* M203指令
     设置最大进给速度(Units/s) 对XYZE轴
   M203 [E<units/s>] [T<index>] [X<units/s>] [Y<units/s>] [Z<units/s>]
 **/
#ifdef __cplusplus
extern "C" {
#endif

VOID GcodeM203Init(VOID *Model);

S32 GcodeM203Parse(const S8 *CmdBuff, VOID *Arg);     //M203命令的解析函数
S32 GcodeM203Exec(VOID *ArgPtr, VOID *ReplyResult);   //M203命令的执行函数
S32 GcodeM203Reply(VOID *ReplyResult, S8 *Buff);      //M203命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM203HANDLE_H_ */
