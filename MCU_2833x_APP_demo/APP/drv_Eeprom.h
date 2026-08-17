#ifndef __EEPROM_H_
#define __EEPROM_H_

#include "TypeDefine.h"

//0x50--1024   0x54--512
#define EEPROM_ADDR (0x54)

extern Uint16 RDE2ROM(Uint16 *RamAddr, Uint16 RomAddress, Uint16 number);
extern Uint16 WRE2ROM(Uint16 *Wdata, Uint16 RomAddress, Uint16 number);

#endif
