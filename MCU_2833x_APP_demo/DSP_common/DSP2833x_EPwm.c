// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:46:19 $
//###########################################################################
//
// FILE:   DSP2833x_EPwm.c
//
// TITLE:  DSP2833x ePWM Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "drv_GlobalVar.h"
//---------------------------------------------------------------------------
// InitEPwm: 
//---------------------------------------------------------------------------
// This function initializes the ePWM(s) to a known state.
//
void InitEPwm(void)
{
/*========================== PWM1 Config =====================================*/	
	EPwm1Regs.TBSTS.all=0;
	EPwm1Regs.TBPHS.half.TBPHS=0;
	EPwm1Regs.TBCTR=0;

//	EPwm1Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = 0;		// CMPA data loaded to shadow registor
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = 0;		// CMPB data loaded to shadow registor
	EPwm1Regs.CMPCTL.bit.LOADAMODE = 0;   	// CMPA data loaded at CTR=Zero
	EPwm1Regs.CMPCTL.bit.LOADBMODE = 0;   	// CMPA data loaded at CTR=Zero	

	EPwm1Regs.CMPA.half.CMPA = 0;
	EPwm1Regs.CMPB=0;

	EPwm1Regs.AQCTLA.all=0x90;              // EPWMxA = 0 when CTR=CMPA and counter inc
	                                        // EPWMxA = 1 when CTR=CMPA and counter dec
	EPwm1Regs.AQCTLB.all=0;
	EPwm1Regs.AQSFRC.all=0;
	EPwm1Regs.AQCSFRC.all=0;

	EPwm1Regs.DBCTL.all=0xB;                        // EPWMxB is inverted
	EPwm1Regs.DBRED= 0;//0xE1;//0x96;
	EPwm1Regs.DBFED= 0;//0xE1;//0x96;

	EALLOW;
	EPwm1Regs.TZSEL.all=0;
	EPwm1Regs.TZCTL.all=0xA;
	EPwm1Regs.TZEINT.all=0;
	EPwm1Regs.TZFLG.all=0;
	EPwm1Regs.TZCLR.all=0;
	EPwm1Regs.TZFRC.all=0;
    EPwm1Regs.TZFRC.bit.OST=1;
	EDIS;

	EPwm1Regs.ETSEL.all=0;                           // Interrupt when TBCTR = 0x0000
	EPwm1Regs.ETFLG.all=0;
	EPwm1Regs.ETCLR.all=0;
	EPwm1Regs.ETFRC.all=0;

	EPwm1Regs.PCCTL.all=0;

	EPwm1Regs.TBCTL.all=0x0010 + 0x2002;			// Enable Timer, load PRD value from shadow register when CTR =  ZERO
	EPwm1Regs.TBPRD=0;

/*=========================== Config SOC ==================================*/

//	EPwm1Regs.ETSEL.bit.INTEN 	= 1;
//	EPwm1Regs.ETSEL.bit.INTSEL 	= 2;
//	EPwm1Regs.ETPS.bit.INTPRD	= 1;

/*
	EPwm1Regs.ETSEL.bit.SOCAEN = 1;        // Enable SOC on A group
   	EPwm1Regs.ETSEL.bit.SOCASEL = 2;       // Select SOC from Period
   	EPwm1Regs.ETPS.bit.SOCAPRD = 1;        // Generate pulse on 1st event
*/
/*========================== PWM2 Config =====================================*/
	EPwm2Regs.TBSTS.all=0;
	EPwm2Regs.TBPHS.half.TBPHS=0;
	EPwm2Regs.TBCTR=0;

//	EPwm1Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = 0;		// CMPA data loaded to shadow registor
	EPwm2Regs.CMPCTL.bit.SHDWBMODE = 0;		// CMPB data loaded to shadow registor
	EPwm2Regs.CMPCTL.bit.LOADAMODE = 0;   	// CMPA data loaded at CTR=Zero
	EPwm2Regs.CMPCTL.bit.LOADBMODE = 0;   	// CMPA data loaded at CTR=Zero	

	EPwm2Regs.CMPA.half.CMPA = 0;
	EPwm2Regs.CMPB=0;

	EPwm2Regs.AQCTLA.all=0x90;        // EPWMxA = 0 when CTR=CMPA and counter inc
	                                  // EPWMxA = 1 when CTR=CMPA and counter dec
	EPwm2Regs.AQCTLB.all=0;
	EPwm2Regs.AQSFRC.all=0;
	EPwm2Regs.AQCSFRC.all=0;

	EPwm2Regs.DBCTL.all=0xB;          // EPWMxB is inverted
	EPwm2Regs.DBRED= 0x0;//0x96;
	EPwm2Regs.DBFED= 0x0;//0x96;

	EALLOW;
	EPwm2Regs.TZSEL.all=0;
	EPwm2Regs.TZCTL.all=0xA;
	EPwm2Regs.TZEINT.all=0;
	EPwm2Regs.TZFLG.all=0;
	EPwm2Regs.TZCLR.all=0;
	EPwm2Regs.TZFRC.all=0;
    EPwm2Regs.TZFRC.bit.OST=1;
	EDIS;

	EPwm2Regs.ETSEL.all=0;                           // Interrupt when TBCTR = 0x0000
	EPwm2Regs.ETFLG.all=0;
	EPwm2Regs.ETCLR.all=0;
	EPwm2Regs.ETFRC.all=0;

	EPwm2Regs.PCCTL.all=0;

	EPwm2Regs.TBCTL.all=0x0000 + 0x0006;//0x0010 + 0x2006;//0x0010 + 0x2002;			// Enable Timer, load PRD value from shadow register when CTR =  ZERO
	EPwm2Regs.TBPRD=0;

/*========================== PWM3 Config =====================================*/
	EPwm3Regs.TBSTS.all=0;
	EPwm3Regs.TBPHS.half.TBPHS=0;
	EPwm3Regs.TBCTR=0;

	EPwm3Regs.CMPCTL.bit.SHDWAMODE = 0;		// CMPA data loaded to shadow registor
	EPwm3Regs.CMPCTL.bit.SHDWBMODE = 0;		// CMPB data loaded to shadow registor
	EPwm3Regs.CMPCTL.bit.LOADAMODE = 0;   	// CMPA data loaded at CTR=Zero
	EPwm3Regs.CMPCTL.bit.LOADBMODE = 0;   	// CMPA data loaded at CTR=Zero	

	EPwm3Regs.CMPA.half.CMPA = 0;
	EPwm3Regs.CMPB=0;

	EPwm3Regs.AQCTLA.all=0x90;              // EPWMxA = 0 when CTR=CMPA and counter inc
	                                        // EPWMxA = 1 when CTR=CMPA and counter dec
	EPwm3Regs.AQCTLB.all=0;
	EPwm3Regs.AQSFRC.all=0;
	EPwm3Regs.AQCSFRC.all=0;

	EPwm3Regs.DBCTL.all=0xB;                // EPWMxB is inverted
	EPwm3Regs.DBRED= 0x0;
	EPwm3Regs.DBFED= 0x0;

	EALLOW;
	EPwm3Regs.TZSEL.all=0;
	EPwm3Regs.TZCTL.all=0xA;
	EPwm3Regs.TZEINT.all=0;
	EPwm3Regs.TZFLG.all=0;
	EPwm3Regs.TZCLR.all=0;
	EPwm3Regs.TZFRC.all=0;
    EPwm3Regs.TZFRC.bit.OST=1;
	EDIS;

	EPwm3Regs.ETSEL.all=0;            // Interrupt when TBCTR = 0x0000
	EPwm3Regs.ETFLG.all=0;
	EPwm3Regs.ETCLR.all=0;
	EPwm3Regs.ETFRC.all=0;

	EPwm3Regs.PCCTL.all=0;

	EPwm3Regs.TBCTL.all=0x0010 + 0x2002;			// Enable Timer
	EPwm3Regs.TBPRD = 0;

/*========================== PWM4 Config =====================================*/
	EPwm4Regs.TBSTS.all=0;
	EPwm4Regs.TBPHS.half.TBPHS=0;
	EPwm4Regs.TBCTR=0;

	EPwm4Regs.CMPCTL.bit.SHDWAMODE = 0;		// CMPA data loaded to shadow registor
	EPwm4Regs.CMPCTL.bit.SHDWBMODE = 0;		// CMPB data loaded to shadow registor
	EPwm4Regs.CMPCTL.bit.LOADAMODE = 0;   	// CMPA data loaded at CTR=Zero
	EPwm4Regs.CMPCTL.bit.LOADBMODE = 0;   	// CMPA data loaded at CTR=Zero	

	EPwm4Regs.CMPA.half.CMPA = 0;//HALF_DUTY / 2;
	EPwm4Regs.CMPB=0;

	EPwm4Regs.AQCTLA.all=0x90;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm4Regs.AQCTLB.all=0;//0x900; 		  // EPWMxB = 0 when CTR=CMPB and counter inc
	                                  // EPWMxB = 1 when CTR=CMPB and counter dec
	EPwm4Regs.AQSFRC.all=0;
	EPwm4Regs.AQCSFRC.all=0;

	EPwm4Regs.DBCTL.all=0xB;//0x0;         // EPWMxA EPWMxB is independ
	EPwm4Regs.DBRED=0x0;			 //  DEAD TIME 0us
	EPwm4Regs.DBFED=0x0;
	
	EALLOW;
	EPwm4Regs.TZSEL.all=0;
	EPwm4Regs.TZCTL.all=0xA;
	EPwm4Regs.TZEINT.all=0;
	EPwm4Regs.TZFLG.all=0;
	EPwm4Regs.TZCLR.all=0;
	EPwm4Regs.TZFRC.all=0;
    EPwm4Regs.TZFRC.bit.OST=1;
	EDIS;
	
	EPwm4Regs.ETSEL.all=0;            // Interrupt when TBCTR = 0x0000
	EPwm4Regs.ETFLG.all=0;
	EPwm4Regs.ETCLR.all=0;
	EPwm4Regs.ETFRC.all=0;

	EPwm4Regs.PCCTL.all=0;

	EPwm4Regs.TBCTL.all=0x0010 + 0x2002;			// Enable Timer
	EPwm4Regs.TBCTL.bit.HSPCLKDIV = 0x1;
	EPwm4Regs.TBPRD= 0;

/*========================== PWM5 Config =====================================*/
	EPwm5Regs.TBSTS.all= 0;
	EPwm5Regs.TBPHS.half.TBPHS= 0;//0x1388;
	EPwm5Regs.TBCTR=0;

	EPwm5Regs.CMPCTL.bit.SHDWAMODE = 0;		// CMPA data loaded to shadow registor
	EPwm5Regs.CMPCTL.bit.SHDWBMODE = 0;		// CMPB data loaded to shadow registor
	EPwm5Regs.CMPCTL.bit.LOADAMODE = 0;   	// CMPA data loaded at CTR=Zero
	EPwm5Regs.CMPCTL.bit.LOADBMODE = 0;   	// CMPA data loaded at CTR=Zero	

	EPwm5Regs.CMPA.half.CMPA = 0;//HALF_DUTY / 2;
	EPwm5Regs.CMPB=0;

	EPwm5Regs.AQCTLA.all=0x90;        // EPWMxA = 0 when CTR=CMPA and counter inc
	                                  // EPWMxA = 1 when CTR=CMPA and counter dec
	EPwm5Regs.AQCTLB.all=0;//0x900; 	  // EPWMxB = 0 when CTR=CMPB and counter inc
	                                  // EPWMxB = 1 when CTR=CMPB and counter dec
	EPwm5Regs.AQSFRC.all=0;
	EPwm5Regs.AQCSFRC.all=0;

	EPwm5Regs.DBCTL.all=0xB;//0x0;         // EPWMxA EPWMxB is independ
	EPwm5Regs.DBRED=0x0;			 //  DEAD TIME 0us
	EPwm5Regs.DBFED=0x0;

	EALLOW;
	EPwm5Regs.TZSEL.all=0;
	EPwm5Regs.TZCTL.all=0xA;
	EPwm5Regs.TZEINT.all=0;
	EPwm5Regs.TZFLG.all=0;
	EPwm5Regs.TZCLR.all=0;
	EPwm5Regs.TZFRC.all=0;
    EPwm5Regs.TZFRC.bit.OST=1;
	EDIS;

	EPwm5Regs.ETSEL.all=0;            // Interrupt when TBCTR = 0x0000
	EPwm5Regs.ETFLG.all=0;
	EPwm5Regs.ETCLR.all=0;
	EPwm5Regs.ETFRC.all=0;

	EPwm5Regs.PCCTL.all=0;

	EPwm5Regs.TBCTL.all=0x0010 + 0x2002;			// Enable Timer
	EPwm5Regs.TBCTL.bit.HSPCLKDIV = 0x1;
	EPwm5Regs.TBPRD= 0;

#if 0
	EALLOW;
	EPwm5Regs.HRCNFG.all = 0x0;
	EPwm5Regs.HRCNFG.bit.EDGMODE = HR_BEP;//HR_REP;				//MEP control on Rising edge
	EPwm5Regs.HRCNFG.bit.CTLMODE = HR_PHS;
	EPwm5Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;
	EDIS;
#endif


/*========================== PWM6 Config =====================================*/
	EPwm6Regs.TBSTS.all=0;
	EPwm6Regs.TBPHS.half.TBPHS= 0;//0x1388;
	EPwm6Regs.TBCTR=0;

	EPwm6Regs.CMPCTL.bit.SHDWAMODE = 0;		// CMPA data loaded to shadow registor
	EPwm6Regs.CMPCTL.bit.SHDWBMODE = 0;		// CMPB data loaded to shadow registor
	EPwm6Regs.CMPCTL.bit.LOADAMODE = 0;   	// CMPA data loaded at CTR=Zero
	EPwm6Regs.CMPCTL.bit.LOADBMODE = 0;   	// CMPA data loaded at CTR=Zero	

	EPwm6Regs.CMPA.half.CMPA = 0;//HALF_DUTY / 2;
	EPwm6Regs.CMPB=0;

	EPwm6Regs.AQCTLA.all= 0x90; //0x60;//0x90;       // EPWMxA = 0 when CTR=CMPA and counter inc
	                                  		// EPWMxA = 1 when CTR=CMPA and counter dec
	EPwm6Regs.AQCTLB.all=0;//0x900;      	 // EPWMxB = 0 when CTR=CMPB and counter inc
	                                  		// EPWMxB = 1 when CTR=CMPB and counter dec
	EPwm6Regs.AQSFRC.all=0;
	EPwm6Regs.AQCSFRC.all=0;

	EPwm6Regs.DBCTL.all=0xB;//0x0;          // EPWMxA EPWMxB is independ
	EPwm6Regs.DBRED=0x0;			 		//  DEAD TIME 0us
	EPwm6Regs.DBFED=0x0;

	EALLOW;
	EPwm6Regs.TZSEL.all=0;
	EPwm6Regs.TZCTL.all=0xA;
	EPwm6Regs.TZEINT.all=0;
	EPwm6Regs.TZFLG.all=0;
	EPwm6Regs.TZCLR.all=0;
	EPwm6Regs.TZFRC.all=0;
    EPwm6Regs.TZFRC.bit.OST=1;
	EDIS;

	EPwm6Regs.ETSEL.all=0;            // Interrupt when TBCTR = 0x0000
	EPwm6Regs.ETFLG.all=0;
	EPwm6Regs.ETCLR.all=0;
	EPwm6Regs.ETFRC.all=0;

	EPwm6Regs.PCCTL.all=0;

	EPwm6Regs.TBCTL.all=0x0010 + 0x2002;			// Enable Timer
	EPwm6Regs.TBCTL.bit.HSPCLKDIV = 0x1;
	EPwm6Regs.TBPRD= 0;
}

//---------------------------------------------------------------------------
// Example: InitEPwmGpio: 
//---------------------------------------------------------------------------
// This function initializes GPIO pins to function as ePWM pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.  
// 

void InitEPwmGpio(void)
{
   InitEPwm1Gpio();
   InitEPwm2Gpio();
   InitEPwm3Gpio();
#if DSP28_EPWM4
   InitEPwm4Gpio();
#endif // endif DSP28_EPWM4
#if DSP28_EPWM5    
   InitEPwm5Gpio();
#endif // endif DSP28_EPWM5
#if DSP28_EPWM6
   InitEPwm6Gpio();
#endif // endif DSP28_EPWM6 
}

void InitEPwm1Gpio(void)
{
   EALLOW;
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user. 
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;    // Enable pull-up on GPIO0 (EPWM1A)
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;    // Enable pull-up on GPIO1 (EPWM1B)   
   
/* Configure ePWM-1 pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be ePWM1 functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;   // Configure GPIO0 as EPWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;   // Configure GPIO1 as EPWM1B
    EDIS;
}

void InitEPwm2Gpio(void)
{
   EALLOW;
	
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user. 
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;    // Enable pull-up on GPIO2 (EPWM2A)
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;    // Enable pull-up on GPIO3 (EPWM3B)

/* Configure ePWM-2 pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be ePWM2 functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;   // Configure GPIO2 as EPWM2A
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;   // Configure GPIO3 as EPWM2B
   
    EDIS;
}

void InitEPwm3Gpio(void)
{
   EALLOW;
   
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user. 
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;    // Enable pull-up on GPIO4 (EPWM3A)
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;    // Enable pull-up on GPIO5 (EPWM3B)
       
/* Configure ePWM-3 pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be ePWM3 functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;   // Configure GPIO4 as EPWM3A
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;   // Configure GPIO5 as EPWM3B
	
    EDIS;
}


#if DSP28_EPWM4
void InitEPwm4Gpio(void)
{
   EALLOW;
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user. 
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;    // Enable pull-up on GPIO6 (EPWM4A)
    GpioCtrlRegs.GPAPUD.bit.GPIO7 = 1;    // Enable pull-up on GPIO7 (EPWM4B)

/* Configure ePWM-4 pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be ePWM4 functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;   // Configure GPIO6 as EPWM4A
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;   // Configure GPIO7 as EPWM4B
	
    EDIS;
}
#endif // endif DSP28_EPWM4  


#if DSP28_EPWM5
void InitEPwm5Gpio(void)
{
   EALLOW;
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user. 
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 1;    // Enable pull-up on GPIO8 (EPWM5A)
    GpioCtrlRegs.GPAPUD.bit.GPIO9 = 1;    // Enable pull-up on GPIO9 (EPWM5B)

/* Configure ePWM-5 pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be ePWM5 functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;   // Configure GPIO8 as EPWM5A
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;   // Configure GPIO9 as EPWM5B
	
    EDIS;
}
#endif // endif DSP28_EPWM5


#if DSP28_EPWM6
void InitEPwm6Gpio(void)
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user. 
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO10 = 1;    // Enable pull-up on GPIO10 (EPWM6A)
    GpioCtrlRegs.GPAPUD.bit.GPIO11 = 1;    // Enable pull-up on GPIO11 (EPWM6B)

/* Configure ePWM-6 pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be ePWM6 functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;   // Configure GPIO10 as EPWM6A
    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;   // Configure GPIO11 as EPWM6B
	
    EDIS;
}
#endif // endif DSP28_EPWM6  

//---------------------------------------------------------------------------
// Example: InitEPwmSyncGpio: 
//---------------------------------------------------------------------------
// This function initializes GPIO pins to function as ePWM Synch pins
//

void InitEPwmSyncGpio(void)
{

   EALLOW;

/* Configure EPWMSYNCI  */
   
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user. 
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;    // Enable pull-up on GPIO6 (EPWMSYNCI)
// GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;   // Enable pull-up on GPIO32 (EPWMSYNCI)    

/* Set qualification for selected pins to asynch only */
// This will select synch to SYSCLKOUT for the selected pins.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAQSEL1.bit.GPIO6 = 0;   // Synch to SYSCLKOUT GPIO6 (EPWMSYNCI)
// GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 0;  // Synch to SYSCLKOUT GPIO32 (EPWMSYNCI)    

/* Configure EPwmSync pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be EPwmSync functional pins.
// Comment out other unwanted lines.   

   GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 2;    // Enable pull-up on GPIO6 (EPWMSYNCI)
// GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 2;   // Enable pull-up on GPIO32 (EPWMSYNCI)    



/* Configure EPWMSYNC0  */

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user. 
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

// GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;    // Enable pull-up on GPIO6 (EPWMSYNC0)
   GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;   // Enable pull-up on GPIO33 (EPWMSYNC0)    

// GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 3;    // Enable pull-up on GPIO6 (EPWMSYNC0)
   GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 2;   // Enable pull-up on GPIO33 (EPWMSYNC0)    
}



//---------------------------------------------------------------------------
// Example: InitTzGpio: 
//---------------------------------------------------------------------------
// This function initializes GPIO pins to function as Trip Zone (TZ) pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.  
// 

void InitTzGpio(void)
{
   EALLOW;
   
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user. 
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.
   GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;    // Enable pull-up on GPIO12 (TZ1)
//   GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;    // Enable pull-up on GPIO13 (TZ2)
//   GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;    // Enable pull-up on GPIO14 (TZ3)
//   GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    // Enable pull-up on GPIO15 (TZ4)

//   GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;    // Enable pull-up on GPIO16 (TZ5)
// GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    // Enable pull-up on GPIO28 (TZ5)

//   GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;    // Enable pull-up on GPIO17 (TZ6) 
// GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;    // Enable pull-up on GPIO29 (TZ6)  
   
/* Set qualification for selected pins to asynch only */
// Inputs are synchronized to SYSCLKOUT by default.  
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 3;  // Asynch input GPIO12 (TZ1)
//   GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 3;  // Asynch input GPIO13 (TZ2)
//   GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 3;  // Asynch input GPIO14 (TZ3)
//   GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input GPIO15 (TZ4)

//   GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3;  // Asynch input GPIO16 (TZ5)
// GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (TZ5)

//   GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3;  // Asynch input GPIO17 (TZ6) 
// GpioCtrlRegs.GPAQSEL2.bit.GPIO29 = 3;  // Asynch input GPIO29 (TZ6)  

   
/* Configure TZ pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be TZ functional pins.
// Comment out other unwanted lines.   
   GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 1;  // Configure GPIO12 as TZ1
//   GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 1;  // Configure GPIO13 as TZ2
//   GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 1;  // Configure GPIO14 as TZ3
//   GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 1;  // Configure GPIO15 as TZ4

//   GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 3;  // Configure GPIO16 as TZ5
// GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 3;  // Configure GPIO28 as TZ5

//   GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 3;  // Configure GPIO17 as TZ6               
// GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 3;  // Configure GPIO29 as TZ6  

   EDIS;
}


#if DSP
void PWMParaSet(float Fsw, float DeadT, float PhaseD, float DutyC)
{
//    Uint16 Fsw_t, DeadT_t, DutyC_t, fDeadComp,fDCNomConst,PhaseD_t ;
//
//    Fsw_t       =  75000 / Fsw;
//    fDCNomConst =  Fsw_t;
//    DeadT_t     =  DeadT * 150;
//    PhaseD_t    =  PhaseD * Fsw_t;
//    DutyC_t     =  DutyC * Fsw_t;
//
//    fDeadComp   = (float) DeadT_t / Fsw_t;
//
//    EPwm1Regs.TBPRD = 375;//Fsw_t;
//    EPwm2Regs.TBPRD = 375;//Fsw_t;
//    EPwm3Regs.TBPRD = Fsw_t;
//    EPwm4Regs.TBPRD = Fsw_t;
//    EPwm5Regs.TBPRD = Fsw_t;
//    EPwm6Regs.TBPRD = Fsw_t;
//
//    EPwm1Regs.CMPA.half.CMPA = DutyC_t;     //+ DeadT_t;
//    EPwm2Regs.CMPA.half.CMPA = DutyC_t;     //+ DeadT_t;
//    EPwm3Regs.CMPA.half.CMPA = DutyC_t;     //+ DeadT_t;
//    EPwm4Regs.CMPA.half.CMPA = DutyC_t;     //+ DeadT_t;
//    EPwm5Regs.CMPA.half.CMPA = DutyC_t;     //+ DeadT_t;
//    EPwm6Regs.CMPA.half.CMPA = DutyC_t;     //+ DeadT_t;
//
//    EPwm1Regs.DBRED = 30;//DeadT_t;
//    EPwm1Regs.DBFED = 30;//DeadT_t;
//    EPwm2Regs.DBRED = 30;//DeadT_t;
//    EPwm2Regs.DBFED = 30;//DeadT_t;
//    EPwm3Regs.DBRED = 75;//DeadT_t;
//    EPwm3Regs.DBFED = 75;//DeadT_t;
//    EPwm4Regs.DBRED = DeadT_t;
//    EPwm4Regs.DBFED = DeadT_t;
//    EPwm5Regs.DBRED = DeadT_t;
//    EPwm5Regs.DBFED = DeadT_t;
//    EPwm6Regs.DBRED = DeadT_t;
//    EPwm6Regs.DBFED = DeadT_t;
//
//    EALLOW;
//    EPwm1Regs.TZFRC.bit.OST=1;
//    EPwm2Regs.TZFRC.bit.OST=1;
//    EPwm3Regs.TZFRC.bit.OST=1;
//    EPwm4Regs.TZFRC.bit.OST=1;
//    EPwm5Regs.TZFRC.bit.OST=1;
//    EPwm6Regs.TZFRC.bit.OST=1;
//    EDIS;
//
//    EALLOW;
//    EPwm2Regs.TBPHS.half.TBPHS  = 0;
//    EDIS;
}
#endif
//===========================================================================
// End of file.
//===========================================================================
