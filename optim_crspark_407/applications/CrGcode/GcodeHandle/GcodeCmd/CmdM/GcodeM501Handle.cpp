#include "GcodeM501Handle.h"
#include <string.h>
#include "Common/Include/CrEepromApi.h"
#include "../../GcodeHandle.h"

#define CMD_M501 ("M501")

S32 GcodeM501Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    //rt_kprintf("CmdBuff %s \n ", CmdBuff);
    if ( strncmp(CmdBuff, CMD_M501, strlen(CMD_M501)) != 0)
        return 0;

    return GCODE_EXEC_OK;
}

S32 GcodeM501Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    EepromLoadSetting();


    return GCODE_EXEC_OK;
}

S32 GcodeM501Reply(VOID *ReplyResult, S8 *Buff)
{

    return 0;
}
