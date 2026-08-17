#include "drv_fpga.h"
#include "string.h"

// XINTF 物理地址和底层访问只允许在本文件内使用。
#define FPGA_BASE_ADDR              0x100000UL
#define DataW(addr, data)           do { *((volatile Uint32 *)FPGA_BASE_ADDR + (addr)) = (Uint32)(data); } while (0)
#define DataR(addr)                 (*((volatile Uint32 *)FPGA_BASE_ADDR + (addr)))

// FPGA XINTF 地址分区：Main [0, 50)，ISR [50, 100)，RAM [200, +)。
#define FPGA_MAIN_ADDR_BASE         0U
#define FPGA_ISR_ADDR_BASE          50U
#define FPGA_RAM_ADDR_BASE          200U
#define FPGA_RAM_WORD_COUNT         512U

// Main 读写通道使用各自的 FPGA 读写寄存器空间。
#define FPGA_MAIN_CTRL_REG_ADDR     (FPGA_MAIN_ADDR_BASE + 0U)
#define FPGA_MAIN_INFO_ADDR         (FPGA_MAIN_ADDR_BASE + 0U)

// ISR 通道从地址 50 起始，不与 Main 通道交叉。
#define FPGA_ISR_SETPOINT_ADDR      (FPGA_ISR_ADDR_BASE + 0U)
#define FPGA_ISR_SAMPLE_DONE_ADDR   (FPGA_ISR_ADDR_BASE + 0U)

// RAM 地址必须通过该转换单独处理，不得当作 Main/ISR 寄存器地址。
#define FPGA_RAM_ADDR(offset)       (FPGA_RAM_ADDR_BASE + (offset))

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
    DataW(FPGA_MAIN_CTRL_REG_ADDR, st_fpga_main_wr.ctrl_reg.all);
}


void FpgaMainReadUpdate(void)
{
    st_fpga_main_rd.fpga_info.all = DataR(FPGA_MAIN_INFO_ADDR);
}

void FpgaISRWriteUpdate(void)
{
    DataW(FPGA_ISR_SETPOINT_ADDR, st_fpga_isr_wr.current_setpoint);
}

void FpgaISRReadUpdate(void)
{
    st_fpga_isr_rd.sample_done_flag = DataR(FPGA_ISR_SAMPLE_DONE_ADDR);
}

Uint16 FpgaRamRead(Uint16 offset, Uint32 *data)
{
    if((data == 0) || (offset >= FPGA_RAM_WORD_COUNT))
    {
        return 0U;
    }

    *data = DataR(FPGA_RAM_ADDR(offset));
    return 1U;
}

Uint16 FpgaRamWrite(Uint16 offset, Uint32 data)
{
    if(offset >= FPGA_RAM_WORD_COUNT)
    {
        return 0U;
    }

    DataW(FPGA_RAM_ADDR(offset), data);
    return 1U;
}
