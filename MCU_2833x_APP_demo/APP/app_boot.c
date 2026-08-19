#include "app_boot.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#define APP_BOOT_WD_RESET_CONTROL    (0x0028U)

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
