#include "drv_fpga.h"
#include "string.h"

FPGA_MAIN_WR_T st_fpga_main_wr;
FPGA_MAIN_RD_T st_fpga_main_rd;
FPGA_ISR_WR_T  st_fpga_isr_wr;
FPGA_ISR_RD_T  st_fpga_isr_rd;

void FpgaDrvInit(void)
{
    memset(&st_fpga_main_wr, 0, sizeof(FPGA_MAIN_WR_T));
    memset(&st_fpga_main_rd, 0, sizeof(FPGA_MAIN_RD_T));
    memset(&st_fpga_isr_wr, 0, sizeof(FPGA_ISR_WR_T));
    memset(&st_fpga_isr_rd, 0, sizeof(FPGA_ISR_RD_T));

    // 变量赋初值

}

void FpgaMainWriteUpdate(void)
{
    DataW(0, st_fpga_main_wr.ctrl_reg.all);
}


void FpgaMainReadUpdate(void)
{
    st_fpga_main_rd.fpga_info.all = DataR(0);
}

void FpgaISRWriteUpdate(void)
{
//    DataW(1, st_fpga_isr_wr.current_setpoint);
}

void FpgaISRReadUpdate(void)
{
//    st_fpga_isr_rd.sample_done_flag = DataR(0);
}
