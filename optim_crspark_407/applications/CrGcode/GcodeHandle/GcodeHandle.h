#ifndef _GCODEHANDLE_H
#define _GCODEHANDLE_H

#include "CrInc/CrType.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_ARG_LENGTH  200

enum GcodeReadAction_t
{
    CrErr = 0,                  //未知错误
    GCODE_EXEC_OK,
    GCODE_EXEC_WAIT,            //执行成功，并阻止gcode执行器继续执行，等待条件达成
    GCODE_EXEC_PROCESS,
    GCODE_EXEC_FAIL,            //执行失败
    GCODE_PARSE_FAIL,           //解析失败
    GCODE_REPLY_FAIL,           //回复失败
    GCODE_NOT_FOUND,            //未找到指令
    GCODE_CMD_ERROR,            //指令错误
    GCODE_NOT_SUPPOR,           //指令不支持
};

S32 GcodeHandle(S8 *Buff, S32 Len, S8 *Replay);

#ifdef __cplusplus
}
#endif

#endif

