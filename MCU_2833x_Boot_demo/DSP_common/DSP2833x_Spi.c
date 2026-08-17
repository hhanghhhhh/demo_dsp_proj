// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:46:44 $
//###########################################################################
//
// FILE:   DSP2833x_Spi.c
//
// TITLE:  DSP2833x SPI Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

//---------------------------------------------------------------------------
// InitSPI: 
//---------------------------------------------------------------------------
// This function initializes the SPI(s) to a known state.
//
void InitSpi(void)
{
   // Initialize SPI-A/B/C/D
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;//
    SpiaRegs.SPICCR.all = 0x0047;       //The SPI software resets the polarity bit
                                                //to 1 (sending data along the falling edge),
                                                //moving in and out of the 8 bit word length each time,
                                                //and prohibiting the SPI internal loopback (LOOKBACK) function;
    SpiaRegs.SPICTL.all = 0x0006;       // Enable master mode, normal phase, // enable talk, and SPI int disabled.
    //SpiaRegs.SPISTS.all = 0x0000;       //溢出中断，禁止SPI中断；
    SpiaRegs.SPIBRR = 0x0003;           //SPI波特率=37.5M/4=9.37MHZ；
    SpiaRegs.SPIPRI.bit.FREE = 1;       //Set so breakpoints don't disturb xmission
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;
   //tbd...
 
}

//---------------------------------------------------------------------------
// Example: InitSpiGpio: 
//---------------------------------------------------------------------------
// This function initializes GPIO pins to function as SPI pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.  
// 
// Caution: 
// For each SPI peripheral
// Only one GPIO pin should be enabled for SPISOMO operation.
// Only one GPIO pin should be enabled for SPISOMI operation. 
// Only one GPIO pin should be enabled for SPICLKA operation. 
// Only one GPIO pin should be enabled for SPISTEA operation. 
// Comment out other unwanted lines.

void InitSpiGpio()
{

   InitSpiaGpio();
}

void InitSpiaGpio()
{

   EALLOW;
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user.  
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // Enable pull-up on GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;   // Enable pull-up on GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;   // Enable pull-up on GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;   // Enable pull-up on GPIO19 (SPISTEA)


//    GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0;   // Enable pull-up on GPIO54 (SPISIMOA)
//    GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;   // Enable pull-up on GPIO55 (SPISOMIA)
//    GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;   // Enable pull-up on GPIO56 (SPICLKA)
//    GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0;   // Enable pull-up on GPIO57 (SPISTEA)

/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // synch input GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // Asynch input GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)

//    GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3; // Asynch input GPIO54 (SPISIMOA)
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3; // Asynch input GPIO55 (SPISOMIA)
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO56 (SPICLKA)
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 3; // Asynch input GPIO57 (SPISTEA)

    
/* Configure SPI-A pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SPI functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1; // Configure GPIO16 as SPISIMOA
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1; // Configure GPIO17 as SPISOMIA
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1; // Configure GPIO18 as SPICLKA
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0; // Configure GPIO19 as SPISTEA

	//GpioCtrlRegs.GPADIR.bit.GPIO16	= 1; // Configure GPIO16 as Output for Sample

	//GpioDataRegs.GPASET.bit.GPIO16	= 1; // Initial let sample is high

//    GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1; // Configure GPIO54 as SPISIMOA
//    GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1; // Configure GPIO55 as SPISOMIA
//    GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1; // Configure GPIO56 as SPICLKA
//    GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 1; // Configure GPIO57 as SPISTEA
	

    EDIS;

}

//===========================================================================
// End of file.
//===========================================================================
