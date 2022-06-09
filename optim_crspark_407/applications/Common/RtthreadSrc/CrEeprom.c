#include "../Include/CrEeprom.h"

const S8 Version[4] = EEPROM_VERSION;

S32  CrEepromOpen()
{

    return 1;
}

S32  CrEepromWrite(U32 Pos, S8 *Buff, S32 Len)
{
    if ( !Buff || (Len <= 0) )
        return 0;



    return Len;
}

S32  CrEepromRead(U32 Pos, S8 *Buff, S32 Len)
{
    if ( !Buff || (Len <= 0) )
        return 0;



    return Len;
}

VOID CrEepromClose()
{

}

S32  CrEepromIoctl(U32 Cmd, VOID *ArgBuff)
{
    if ( !ArgBuff )
        return 0;





    return 1;
}
