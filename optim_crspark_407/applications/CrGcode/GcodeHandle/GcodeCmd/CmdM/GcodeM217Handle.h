#ifndef _GCODEM217HANDLE_H_
#define _GCODEM217HANDLE_H_

#include "CrInc/CrType.h"
#include "Common/Include/CrEepromApi.h"

#define CONSTRAIN(value, arg_min, arg_max) ((value) < (arg_min) ? (arg_min) :((value) > (arg_max) ? (arg_max) : (value)))

struct MigrationSettings_t
{
      U8   Target;
      U8   Last;
      BOOL AutoMode;
      BOOL InProgress;
};

extern struct MigrationSettings_t Migration;
extern BOOL EnableFirstPrime;

#ifdef __cplusplus
extern "C" {
#endif

VOID GcodeM217Init(struct ToolChangeSettings_t *Setting);

S32 GcodeM217Parse(const S8 *CmdBuff, VOID *Arg);     //M217命令的解析函数
S32 GcodeM217Exec(VOID *ArgPtr, VOID *ReplyResult);   //M217命令的执行函数
S32 GcodeM217Reply(VOID *ReplyResult, S8 *Buff);      //M217命令的回复函数

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM217HANDLE_H_ */
