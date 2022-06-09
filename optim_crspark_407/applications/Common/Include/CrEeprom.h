#ifndef _CREEPROM_H
#define _CREEPROM_H

#include "CrInc/CrType.h"
#include <stdio.h>

#define EEPROM_OFFSET 100
#define EEPROM_CAPACITY 4096           //大小是4K
#define EEPROM_VERSION "V81"

//获取命令
#define CMD_GET_EEPROM_CAPACITY   0x0001

extern const S8 Version[4];

#ifdef __cplusplus
extern "C" {
#endif

S32  CrEepromOpen();
S32  CrEepromWrite(U32 Pos, S8 *Buff, S32 Len);
S32  CrEepromRead(U32 Pos, S8 *Buff, S32 Len);
VOID CrEepromClose();
S32  CrEepromIoctl(U32 Cmd, VOID *ArgBuff);

#ifdef __cplusplus
}
#endif

#endif
