#include "drv_Eeprom.h"
#include "drv_GlobalVar.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

Uint16 WRE2ROM(Uint16 *Wdata, Uint16 RomAddress, Uint16 number)
{
    Uint16 i;

    if (I2caRegs.I2CSTR.bit.BB == 1)
    {
        return I2C_BUS_BUSY_ERROR;
    }
    while (!I2C_xrdy())
        ;

    I2caRegs.I2CSAR = EEPROM_ADDR;
    I2caRegs.I2CCNT = number + 2;
    I2caRegs.I2CDXR = (RomAddress >> 8);
    DELAY_US(1);
    I2caRegs.I2CMDR.all = 0x6E20;

    while (!I2C_xrdy())
        ;

    I2caRegs.I2CDXR = RomAddress;
    for (i = 0; i < number; i++)
    {
        while (!I2C_xrdy())
            ;

        I2caRegs.I2CDXR = *Wdata;
        Wdata++;
        if (I2caRegs.I2CSTR.bit.NACK == 1)
        {
            return I2C_BUS_BUSY_ERROR;
        }
    }
    return I2C_SUCCESS;
}

Uint16 RDE2ROM(Uint16 *RamAddr, Uint16 RomAddress, Uint16 number)
{
    Uint16 i;
    Uint16 temp;

    if (I2caRegs.I2CSTR.bit.BB == 1)
    {
        return I2C_BUS_BUSY_ERROR;
    }
    while (!I2C_xrdy())
        ;

    I2caRegs.I2CSAR = EEPROM_ADDR;
    I2caRegs.I2CCNT = 2;
    I2caRegs.I2CDXR = (RomAddress >> 8);
    DELAY_US(1);
    I2caRegs.I2CMDR.all = 0x6620;

    while (!I2C_xrdy())
        ;

    I2caRegs.I2CDXR = RomAddress;
    if (I2caRegs.I2CSTR.bit.NACK == 1)
    {
        return I2C_BUS_BUSY_ERROR;
    }

    DELAY_US(50);
    while (!I2C_xrdy())
        ;

    I2caRegs.I2CSAR = EEPROM_ADDR;
    I2caRegs.I2CCNT = number;
    I2caRegs.I2CMDR.all = 0x6C20;
    if (I2caRegs.I2CSTR.bit.NACK == 1)
    {
        return I2C_BUS_BUSY_ERROR;
    }

    for (i = 0; i < number; i++)
    {
        while (!I2C_rrdy())
            ;

        temp = I2caRegs.I2CDRR;
        if (I2caRegs.I2CSTR.bit.NACK == 1)
        {
            return I2C_BUS_BUSY_ERROR;
        }
        *RamAddr = temp;
        RamAddr++;
    }
    return I2C_SUCCESS;
}

Uint16 I2C_xrdy(void)
{
    return I2caRegs.I2CSTR.bit.XRDY;
}

Uint16 I2C_rrdy(void)
{
    return I2caRegs.I2CSTR.bit.RRDY;
}
