#include "GcodeM500Handle.h"
#include <stdio.h>
#include <string.h>
#include "../../GcodeHandle.h"
#include "Common/Include/CrEepromApi.h"

#define CMD_M500 ("M500")

#ifdef EEPROM_USED

S32 GcodeM500Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    //printf("CmdBuff %s \n ", CmdBuff);
    if ( strncmp(CmdBuff, CMD_M500, strlen(CMD_M500)) != 0)
        return GCODE_CMD_ERROR;

    return GCODE_EXEC_OK;
}

S32 GcodeM500Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    EepromSaveSetting();

    return GCODE_EXEC_OK;
}

S32 GcodeM500Reply(VOID *ReplyResult, S8 *Buff)
{

    return 0;
}

#endif
