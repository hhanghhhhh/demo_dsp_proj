#include "task_eeprom_download.h"
#include "task_boot_eeprom.h"
#include "drv_ModbusData.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "Flash.h"

Uint16 glb_download_flag = 0;

typedef void (*pFunction)(void);
#define APP_START_ADDR       ((Uint32)0x320000)
#define BOOT_WDCR_FAST_RESET ((Uint16)0x0028)

void BOOT_JumpToApplication(Uint32 Addr)
{
    pFunction jump;
    jump = (pFunction)(Addr);
    jump();
}

void load_application(void)
{
    DINT;
    DRTM;
    BOOT_JumpToApplication(APP_START_ADDR);
}

void Boot_ResetToApplication(void)
{
    DINT;
    DRTM;

    /* 清零计数后开启最短周期看门狗，此后不再喂狗。 */
    ServiceDog();
    EALLOW;
    SysCtrlRegs.WDCR = BOOT_WDCR_FAST_RESET;
    EDIS;

    for (;;)
    {
        /* 等待看门狗复位。 */
    }
}

void InitEeromPara_Downloads(void)
{
    Uint16 flash_begin[2] = {0, 0};

    BootEepromParam_Init();
    glb_download_flag = g_boot_eeprom_param.download_flag;

    mgmd_stSCIRx.ip.addr1 = g_boot_eeprom_param.ip[0];
    mgmd_stSCIRx.ip.addr2 = g_boot_eeprom_param.ip[1];
    mgmd_stSCIRx.ip.addr3 = g_boot_eeprom_param.ip[2];
    mgmd_stSCIRx.ip.addr4 = g_boot_eeprom_param.ip[3];

    Flash_RD(APP_START_ADDR, flash_begin, 2);

    if ((glb_download_flag != APP_BOOT_DOWNLOAD_FLAG) &&
        (flash_begin[0] != 0xFFFF) &&
        (flash_begin[1] != 0xFFFF))
    {
        load_application();
    }
}

void WriteEeromPara_Downloads(Uint16 download_flag)
{
    DisableDog();

    glb_download_flag = download_flag;
    BootEepromParam_SetDownloadFlag(download_flag);
    DELAY_US(10000);

    EnableWDog();
}
