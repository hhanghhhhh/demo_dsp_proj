

#ifndef __ECAT_INT_H_
#define __ECAT_INT_H_


/***********************************************************************
Declare external variables
***********************************************************************/

/***********************************************************************
* Function header definition
***********************************************************************/

extern void IntTimeBase(void);

#if DSP
    extern interrupt    void    INT6(void);
    extern interrupt void ISR_CanbInt0(void);
#else
    extern              void    INT6(void);
#endif

#endif
/***************************************************************************
*			END, do not code behind this line!!                            *
****************************************************************************/

