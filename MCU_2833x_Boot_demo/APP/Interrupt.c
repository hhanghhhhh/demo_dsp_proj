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
    CpuTimer0Regs.TCR.bit.TIF=1;

    task_run_cnt++;
	task_run_time = TIMER_CNT_MAX - NOW_TIMER_CNT;
	if(task_run_time > max_task_run_time)
	{
	    max_task_run_time = task_run_time;
	}

    /* 中断源处理完成后再释放 PIE Group 1。 */
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	return;	
}

/***************************************************************************
*			END, do not code behind this line!!                            *
****************************************************************************/

