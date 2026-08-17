#ifndef DRV_FPGA_H_
#define DRV_FPGA_H_


#include "TypeDefine.h"


//-----------------------------------------------------------------------------
// 1. 物理地址映射
//-----------------------------------------------------------------------------
#define FPGA_BASE_ADDR      0x100000    // XINTF Zone 6 起始地址

// 地址偏移定义
#define ADDR_RAM_RD_BASE    200         // [ISR读] RAM 读取基地址
#define ADDR_RAM_WR_BASE    200         // [ISR写] RAM 写入基地址


//-----------------------------------------------------------------------------
// 2. 底层访问宏 (Hardware Access Macros)
//-----------------------------------------------------------------------------
// 必须加 volatile 防止编译器优化读写指令
#define DataW(addr, data)   do{ *((volatile Uint32 *)FPGA_BASE_ADDR + (addr)) = (Uint32)(data); }while(0)
#define DataR(addr)         (  *((volatile Uint32 *)FPGA_BASE_ADDR + (addr)) )

//-----------------------------------------------------------------------------
// 3. 结构体
//-----------------------------------------------------------------------------

// (1) Main Loop 下行数据结构 (DSP -> FPGA)
// 用途：存放非实时的配置参数、状态机控制
typedef struct {
    union {
        Uint32                all;
        struct {
            Uint32 value123:16;
            Uint32 version:8;
            Uint32 rsvd:8;
        }bit;
    }ctrl_reg;
    Uint32                param_kp; // 低速 PID 参数 Kp
    Uint32                param_ki; // 低速 PID 参数 Ki
} FPGA_MAIN_WR_T;

// (2) Main Loop 上行数据结构 (FPGA -> DSP)
// 用途：读取非实时的状态、错误码、版本信息
typedef struct {
    union {
        Uint32                all;
        struct {
            Uint32 value123:16;
            Uint32 version:16;
        }bit;
    }fpga_info;
    Uint32 status_code;             // 运行状态码
    Uint32 version_info;            // FPGA 版本号
    Uint32 error_flags;             // 错误标志位
} FPGA_MAIN_RD_T;

// (3) ISR 下行数据结构 (DSP -> FPGA)
// 用途：实时控制指令，要求极低延迟
typedef struct {
    Uint32 current_setpoint;        // 电流给定
    Uint32 duty_cycle_override;     // 占空比直接控制
} FPGA_ISR_WR_T;

// (4) ISR 上行数据结构 (FPGA -> DSP)
typedef struct {
    Uint32 sample_done_flag;        // 采样完成标志
} FPGA_ISR_RD_T;

//-----------------------------------------------------------------------------
// 4. 全局变量声明
//-----------------------------------------------------------------------------
extern FPGA_MAIN_WR_T st_fpga_main_wr;
extern FPGA_MAIN_RD_T st_fpga_main_rd;
extern FPGA_ISR_WR_T  st_fpga_isr_wr;
extern FPGA_ISR_RD_T  st_fpga_isr_rd;

//-----------------------------------------------------------------------------
// 5. 函数接口声明
//-----------------------------------------------------------------------------
void FpgaDrvInit(void);           // 初始化 XINTF 和结构体

// 主循环调用，先 read 再 write
void FpgaMainReadUpdate(void);   // Main Loop 读刷新
void FpgaMainWriteUpdate(void);  // Main Loop 写刷新

// 中断调用，先 read 再 write
void FpgaISRReadUpdate(void);    // ISR 读刷新
void FpgaISRWriteUpdate(void);   // ISR 写刷新








/******************************************************************************
* == END OF MODULE/FILE (EOF) ==
******************************************************************************/
#endif
