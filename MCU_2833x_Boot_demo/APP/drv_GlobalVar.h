#ifndef __DRV_GLOBAL_VAR_H_
#define __DRV_GLOBAL_VAR_H_

/***************************************************************************
*			END, do not code behind this line!!                            *
****************************************************************************/
#define DSP                 (1)

#include "TypeDefine.h"
/***************************************************************************
*           END, do not code behind this line!!                            *
****************************************************************************/
/*=========================TIME BASE===============================*/
#define     SYS_PERIOD          (50L)     //us
#define     TIMER_CNT_MAX       (150L * SYS_PERIOD)   //150M 50us时，TIMER计数值为150*50 = 7500
#define     SYS_Tb_ms           (1000L / SYS_PERIOD)     //1000/SYS_PERIOD
#define     Tms_CNT(t)          ((t) * SYS_Tb_ms)

#define     NOW_TIMER_CNT       (CpuTimer0Regs.TIM.all)
/*=========================LIMIT VALUE===============================*/
    //W5500
    #define W5500_RESET_H           (GpioDataRegs.GPBSET.bit.GPIO49 = 1)
    #define W5500_RESET_L           (GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1)
    #define W5500_CSN_H             (GpioDataRegs.GPASET.bit.GPIO19 = 1)
    #define W5500_CSN_L             (GpioDataRegs.GPACLEAR.bit.GPIO19 = 1)

/*=============== Main Time Base variable ()============================*/

/***********************************************************************
* Function Name : Global Variable ()
***********************************************************************/

extern Uint32    task_run_cnt;
extern Uint32    task_run_time;
extern Uint32    max_task_run_time;
/*=====================Fault=======================================*/




extern              void InitUserPara(void);
extern              void EnableWDog(void);
//ServiceDog();
//DisableDog();
/***********************************************************************
* typedef of union and structure
***********************************************************************/


#endif
