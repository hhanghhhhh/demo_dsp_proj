#include "drv_GlobalVar.h"
#include "DSP2833x_Device.h"     // Header file Include File
#include "DSP2833x_Examples.h"   // Examples Include File

Uint32          task_run_cnt = 0;
Uint32          task_run_time = 0;
Uint32          max_task_run_time = 0;


extern void HH_test_INT(void);


//定时器中断
interrupt void INT6(void) 				
{
	DINT;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    CpuTimer0Regs.TCR.bit.TIF=1;
    CpuTimer0Regs.TCR.bit.TRB=1;

    //enable can interrupt
    IER |= M_INT9;
    EINT;







    task_run_cnt++;
	task_run_time = TIMER_CNT_MAX - NOW_TIMER_CNT;
	if(task_run_time > max_task_run_time)
	{
	    max_task_run_time = task_run_time;
	}
	return;	
}

/***************************************************************************
*			END, do not code behind this line!!                            *
****************************************************************************/

