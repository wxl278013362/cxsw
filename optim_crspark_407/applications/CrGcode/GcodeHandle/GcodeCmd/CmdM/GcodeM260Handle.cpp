#include "GcodeM260Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"

#define CMD_M260 "M260"


static S32  I2CAddr = 0;
S8 I2CBuff[TWIBUS_BUFFER_SIZE] = {0};
S8 ByteCount = 0;
#define I2C_MAX_ADDR 127
#define I2C_MIN_ADDR 8

VOID ResetI2C()
{
    memset(I2CBuff, 0, sizeof(I2CBuff));
    ByteCount = 0;
}

static VOID SendI2CData()
{
    //发送数据 (  )


    ResetI2C();
}



S32 GcodeM260Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M260
    if ( strcmp(Cmd, CMD_M260) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M260) + sizeof(S8);
    struct ArgM260_t *CmdArg = (struct ArgM260_t *)Arg;

    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) ) break;

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )
        {
            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                    CmdArg->S = atoi(Opt + 1);
                break;

            case OPT_A:
                CmdArg->HasA = true;
                if ( OptLen > 1 )
                    CmdArg->A = atoi(Opt + 1);

            case OPT_B:
                CmdArg->HasB = true;
                if ( OptLen > 1 )
                    CmdArg->B = atoi(Opt + 1);

            case OPT_R:
                CmdArg->HasR = true;
                if ( OptLen > 1 )
                    CmdArg->R = atoi(Opt + 1);

            default:
                printf("Warn:unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
 }


S32 GcodeM260Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM260_t *Param = (struct ArgM260_t *)ArgPtr;
    if ( Param->HasA  )
    {
        if ( Param->A >= 0 )
        {
            I2CAddr = Param->A;
            if ( (I2CAddr < I2C_MIN_ADDR) || (I2CAddr > I2C_MAX_ADDR) )
            {
                printf ("Bad I2C address (8-127)");
            }
        }
    }

    if ( Param->HasB && (ByteCount <= TWIBUS_BUFFER_SIZE))
    {
        I2CBuff[ByteCount++] = Param->B;
    }

    if ( Param->HasS )
    {
        SendI2CData();
    }

    if ( Param->HasR )
        if ( Param->R )
            ResetI2C();

    return 1;
}

S32 GcodeM260Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

