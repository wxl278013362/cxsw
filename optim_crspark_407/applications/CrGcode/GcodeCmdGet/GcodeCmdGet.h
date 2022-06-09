/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-06     cx2470       the first version
 */

#ifndef _GCODE_CMD_GET_H_
#define _GCODE_CMD_GET_H_

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include <stdio.h>

#define READ_BYTE_COUNT_ONCE    50
#define READ_TIMES              1
#define MAX_FILE_OPEN           5
#define MAX_FILE_NAME           60

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

S32 GcodeFileOpen(S8 *FileFullName);
/*
 * Ret = 0 :文件已经读完
 * Ret = -1: 出错
 * Ret = -2: 参数错了
 * Ret > 0: 读出成功
 * */
S32 GcodeFileGetCmd(S32 Fd, S8 *Buf);
VOID GcodeFileClose(S32 Fd);

FLOAT GetPercent(S32 Fd);
BOOL FileIsEnd(S32 Fd);
S32 GetFileFd(S32 Flag);     //设置偏移量

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _GCODE_CMD_GET_H_ */
