// TI File $Revision: /main/4 $
// Checkin $Date: July 30, 2007   14:15:53 $
//###########################################################################
//
// FILE:	DSP2833x_Adc.c
//
// TITLE:	DSP2833x ADC Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "drv_Adc.h"
//#include "ECAT_Constant.h"

#define ADC_usDELAY  5000L

//---------------------------------------------------------------------------
// InitAdc:
//---------------------------------------------------------------------------
// This function initializes ADC to a known state.
//
void InitAdc(void)
{
    extern void DSP28x_usDelay(Uint32 Count);


    // *IMPORTANT*
	// The ADC_cal function, which  copies the ADC calibration values from TI reserved
	// OTP into the ADCREFSEL and ADCOFFTRIM registers, occurs automatically in the
	// Boot ROM. If the boot ROM code is bypassed during the debug process, the
	// following function MUST be called for the ADC to function according
	// to specification. The clocks to the ADC MUST be enabled before calling this
	// function.
	// See the device data manual and/or the ADC Reference
	// Manual for more information.
	
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
	ADC_cal();
	EDIS;

    // To powerup the ADC the ADCENCLK bit should be set first to enable
    // clocks, followed by powering up the bandgap, reference circuitry, and ADC core.
    // Before the first conversion is performed a 5ms delay must be observed
	// after power up to give all analog circuits time to power up and settle

    // Please note that for the delay function below to operate correctly the
	// CPU_CLOCK_SPEED define statement in the DSP2833x_Examples.h file must
	// contain the correct CPU clock period in nanoseconds.

    AdcRegs.ADCTRL3.all = 0x00E0;  // Power up bandgap/reference/ADC circuits
    DELAY_US(ADC_usDELAY);         // Delay before converting ADC channels
/*========================= ADC Configuration ================================*/
   	AdcRegs.ADCTRL3.bit.ADCCLKPS = 0x0; // 25M，高速时钟树配的就是 25M
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;   // 级联模式

    // 越大采样值越稳， 采样率 = 25M/（1+ACO_PS）最高 12.5M
    AdcRegs.ADCTRL1.bit.ACQ_PS = 9;    // 40ns * 11 = 0.44us 采 10 路总共 4.4us
   	// Configure ADC
   	// 右边的是通道 0x0 - 0x7 代表 A0 - A7
   	// 0x8 - 0xF 代表 B0 - B7
   	// ADCMAXCONV 是看左边的 CNV 通道配到哪个，就是哪个
   	AdcRegs.ADCMAXCONV.all = 10;       // Setup 1 conv's on SEQ1
   	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0; // Setup ADCINA0 as 1st SEQ1 conv.
   	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 1;
   	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 2;
   	AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 3;
   	AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 4;
   	AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 5;
   	AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 6;
   	AdcRegs.ADCCHSELSEQ2.bit.CONV07 = 7;
   	AdcRegs.ADCCHSELSEQ3.bit.CONV08 = 8;
   	AdcRegs.ADCCHSELSEQ3.bit.CONV09 = 9;
   	AdcRegs.ADCCHSELSEQ3.bit.CONV10 = 10;
//   	AdcRegs.ADCCHSELSEQ3.bit.CONV11 = 11;
//   	AdcRegs.ADCCHSELSEQ4.bit.CONV12 = 12;
//   	AdcRegs.ADCCHSELSEQ4.bit.CONV13 = 13;
//   	AdcRegs.ADCCHSELSEQ4.bit.CONV14 = 14;
//   	AdcRegs.ADCCHSELSEQ4.bit.CONV15 = 15;

   	// DMA 使用
   	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1; // Enable SEQ1 interrupt (every EOS)

   	//AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1;// Enable SOCA from ePWM to start SEQ1
   	//AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;  // Enable SEQ1 interrupt (every EOS)
// Assumes ePWM1 clock is already enabled in InitSysCtrl();
}
//===========================================================================
// End of file.
//===========================================================================

