#include "app_boot.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

typedef void (*pFunction)(void);

#define BOOT_START_ADDR ((Uint32)0x33FFF6)

void BOOT_JumpToBoot(Uint32 addr)
{
    pFunction jump = (pFunction)(addr);
    jump();
}

void load_boot(void)
{
    DINT;
    DRTM;
    DELAY_US(10000);
    BOOT_JumpToBoot(BOOT_START_ADDR);
}
