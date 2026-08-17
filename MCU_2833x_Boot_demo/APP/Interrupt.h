

#ifndef __ECAT_INT_H_
#define __ECAT_INT_H_


/***********************************************************************
Declare external variables
***********************************************************************/

extern Uint16   slINTcnt;
extern Uint16   slINT1ms;
extern Uint16   slINT10ms;
extern Uint16   slINT100ms;
extern Uint16   slINT1s;



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

