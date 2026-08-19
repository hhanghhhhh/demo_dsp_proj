#include "drv_GlobalVar.h"
#include "Main.h"
#include "drv_Adc.h"
#include "DSP2833x_Device.h"   // Header file Include File
#include "DSP2833x_Examples.h" // Examples Include File
#include "Version.h"
#include "task_scope.h"
#include "validation/task_scope_validation.h"
#include "drv_Fpga.h"

Uint32 task_run_cnt = 0;
Uint32 task_run_time = 0;
Uint32 max_task_run_time = 0;

extern void HH_test_INT(void);

// CANbINT0
interrupt void ISR_CanbInt0(void)
{
    //    RecvCmdFormCtrlBoard();  //主控板报文解析
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

// 定时器中断
interrupt void INT6(void)
{
    CpuTimer0Regs.TCR.bit.TIF = 1U;

    // 读 fpga 寄存器
    // FpgaISRReadUpdate();

    // dsp 自带 adc 采样
    GetAdc();

    // 写 fpga 寄存器
    // FpgaISRWriteUpdate();

    task_run_cnt++;
    task_run_time = TIMER_CNT_MAX - NOW_TIMER_CNT;
    if (task_run_time > max_task_run_time)
    {
        max_task_run_time = task_run_time;
    }
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    return;
}

/***************************************************************************
 *			END, do not code behind this line!!                            *
 ****************************************************************************/
