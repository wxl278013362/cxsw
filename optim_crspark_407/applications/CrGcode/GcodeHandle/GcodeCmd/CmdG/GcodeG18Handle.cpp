#include "GcodeG18Handle.h"

#include "../../GcodeCommonFunc.h"
#include "../../GcodeHandle.h"
#include "../../GcodeType.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_G18 "G18"

#ifdef CNC_WORKSPACE_PLANES
static S8 WorkspacePlane = 0;
#endif /* CNC_WORKSPACE_PLANES */

S32 GcodeG18Parse(const S8 *CmdBuff, VOID *Arg)
{
    /* G18 指令无参数 */
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( Arg == NULL )
    {
        return 0;
    }

    if ( (GetGcodeCmd(CmdBuff, Cmd) == 0) || (strcmp(Cmd, CMD_G18) != 0) )
    {
        return GCODE_CMD_ERROR;
    }

    if ( strlen(CmdBuff) > (strlen(Cmd) + sizeof(S8)) )
    {
        printf("Error: G18 has no parameter \n");
        return 0;
    }
    return 1;
}

S32 GcodeG18Exec(VOID *ArgPtr, VOID *ReplyResult)
{
#ifdef CNC_WORKSPACE_PLANES
    WorkspacePlane = WORKSPACE_PLANE_ZX;
    printf("Workspace Plane: ZX\n");
#endif /* CNC_WORKSPACE_PLANES */
    return 1;
}

S32 GcodeG18Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
