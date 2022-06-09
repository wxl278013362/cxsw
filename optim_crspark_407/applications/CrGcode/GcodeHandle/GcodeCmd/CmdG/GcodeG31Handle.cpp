#include "GcodeG31Handle.h"

#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "../../GcodeType.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_G31 "G31"

S32 PutProbeDeployed(BOOL Enable)
{
    /* 参考 set_probe_deployed 函数，主要probe_specific_action(deploy);的实现 */
    /* 探针动作设置 */
    /* 动作坐标移动（使用旧坐标） */
    /* 设置停止时的探针动作 */
    return 1;
}

S32 GcodeG31Parse(const S8 *CmdBuff, VOID *Arg)
{
    /* G31 指令无参数 */
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( Arg == NULL )
    {
        return CrErr;
    }

    if ( (GetGcodeCmd(CmdBuff, Cmd) == 0) || (strcmp(Cmd, CMD_G31) != 0) )
    {
        return GCODE_CMD_ERROR;
    }

    if ( strlen(CmdBuff) > (strlen(CMD_G31) + sizeof(S8)) )
    {
        printf("Error: G31 has no parameter \n");
        return CrErr;
    }
    return 1;
}

S32 GcodeG31Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    PutProbeDeployed(true);
    return 1;
}

S32 GcodeG31Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
