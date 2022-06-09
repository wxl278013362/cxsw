#include "GcodeM31Handle.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrPrinter.h"
#include "CrModel/CrGcode.h"

#define CMD_M31 ("M31")

S32 GcodeM31Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    if ( strncmp(CmdBuff, CMD_M31, strlen(CMD_M31)) != 0)
        return 0;

    return 1;
}

VOID TimeToString(S8 *Buffer, S32 Time)
{
    int s = Time % 60;
    int m = (Time / 60 ) % 60;
    int h = (Time / 60 / 60) % 24;
    int d = (Time / 60 / 60 / 24) % 365;
    int y = (Time / 60 / 60 / 24 / 365);

    if (y) rt_sprintf(Buffer, "%dy %dd %dh %dm %ds", y, d, h, m, s);
    else if (d) rt_sprintf(Buffer, "%dd %dh %dm %ds", d, h, m, s);
    else if (h) rt_sprintf(Buffer, "%dh %dm %ds", h, m, s);
    else if (m) rt_sprintf(Buffer, "%dm %ds", m, s);
    else rt_sprintf(Buffer, "%ds", s);
}

S32 GcodeM31Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !Printer )
        return 0;

    S8 Buffer[21] = {0};
    TimeToString(Buffer, Printer->GetUsedTime());
    rt_kprintf("Print time: %s\n", Buffer);

    return 1;
}

S32 GcodeM31Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
