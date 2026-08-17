#ifndef __TASK_BOOT_EEPROM_H_
#define __TASK_BOOT_EEPROM_H_

#include "drv_Eeprom.h"

#define APP_BOOT_EEPROM_START         0x0000
#define APP_BOOT_EEPROM_VERSION       1
#define APP_BOOT_EEPROM_MAGIC         0x55AA
#define APP_BOOT_EEPROM_TAIL          0xAA55

#define APP_BOOT_DOWNLOAD_FLAG        11
#define APP_BOOT_DOWNLOAD_CLEAR       0

typedef struct
{
    Uint16 magic;
    Uint16 version;
    Uint16 ip[4];
    Uint16 download_flag;
    Uint16 checksum;
    Uint16 tail;
} APP_BOOT_EEPROM_PARAM;

#define APP_BOOT_EEPROM_WORDS         sizeof(APP_BOOT_EEPROM_PARAM)
#define APP_BOOT_EEPROM_CHECK_WORDS   (sizeof(APP_BOOT_EEPROM_PARAM) - 2)

extern APP_BOOT_EEPROM_PARAM g_boot_eeprom_param;
extern Uint16 g_boot_eeprom_status;

void BootEepromParam_LoadDefault(void);
Uint16 BootEepromParam_Init(void);
Uint16 BootEepromParam_Save(void);
Uint16 BootEepromParam_SetDownloadFlag(Uint16 download_flag);

#endif
