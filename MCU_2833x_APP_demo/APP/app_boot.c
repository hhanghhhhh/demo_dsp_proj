#include "app_boot.h"
#include "app_boot_eeprom.h"
#include "drv_ModbusData.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#define APP_BOOT_WD_RESET_CONTROL    (0x0028U)

/* 检查升级命令，升级标志保存成功后复位进入Boot。 */
void AppBoot_Process(void)
{
    if (mgmd_stSCIRx.jump_cmd != JUMP_TO_BOOT)
    {
        return;
    }

    mgmd_stSCIRx.jump_cmd = 0U;
    g_app_boot_eeprom_param.download_flag = APP_BOOT_DOWNLOAD_FLAG;
    DisableDog();

    if (AppBootEeprom_Save() == 0U)
    {
        AppBoot_ResetToBoot();
    }

    g_app_boot_eeprom_param.download_flag = APP_BOOT_DOWNLOAD_CLEAR;
    EnableWDog();
}

void AppBoot_ResetToBoot(void)
{
    /* 由看门狗产生完整芯片复位，BootROM复位后进入Boot程序。 */
    DINT;
    DRTM;
    IER = 0x0000U;
    IFR = 0x0000U;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 0U;
    PieCtrlRegs.PIEACK.all = 0xFFFFU;

    /* 从完整计数周期开始，以最短周期等待复位，不再喂狗。 */
    ServiceDog();
    EALLOW;
    SysCtrlRegs.WDCR = APP_BOOT_WD_RESET_CONTROL;
    EDIS;

    for (;;)
    {
    }
}
