#ifndef _GCODECOMMONFUNS_H
#define _GCODECOMMONFUNS_H

#include "CrInc/CrType.h"

#define NOTE_CHAR ';'
#define CMD_M43       "M43"

#ifdef __cplusplus
extern "C" {
#endif

S32 GetGcodeCmd(const S8 *Buff, S8 *Cmd);   //获取命令名称
S32 GetCmdOpt(const S8 *Buff, S8 *Opt); //获取命令一个操作的内容

#ifdef __cplusplus
}
#endif

#endif

