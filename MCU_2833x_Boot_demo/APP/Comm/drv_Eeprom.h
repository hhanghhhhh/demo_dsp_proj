#ifndef __EEPROM_H_
#define __EEPROM_H_

#include "drv_GlobalVar.h"

/* Boot_demo 与 APP_demo 使用同一个 EEPROM 从地址。 */
#define EEPROM_ADDR                (0x54)

#define EEROM_DATAOK               (6666)
#define EEROM_ERR_HEAD             (1001)
#define EEROM_ERR_TAIL             (1002)
#define EEROM_ERR_CRC              (1003)
#define EEROM_DATAERR_OVERRANG     (1004)

extern Uint16 RDE2ROM(Uint16 *RamAddr, Uint16 RomAddress, Uint16 number);
extern Uint16 WRE2ROM(Uint16 *Wdata, Uint16 RomAddress, Uint16 number);
extern Uint16 I2C_xrdy(void);
extern Uint16 I2C_rrdy(void);

#endif
