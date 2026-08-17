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

#define     ABS(x)              (((x) > 0) ? (x) : -(x))
/*=========================LIMIT VALUE===============================*/
    //reset FPGA 高电平时reset
    #define RESET_CMD_H             (GpioDataRegs.GPCSET.bit.GPIO86 = 1)
    #define RESET_CMD_L             (GpioDataRegs.GPCCLEAR.bit.GPIO86 = 1)
	
    //W5500
    #define W5500_RESET_H           (GpioDataRegs.GPASET.bit.GPIO24 = 1)
    #define W5500_RESET_L           (GpioDataRegs.GPACLEAR.bit.GPIO24 = 1)
    #define W5500_CSN_H             (GpioDataRegs.GPASET.bit.GPIO19 = 1)
    #define W5500_CSN_L             (GpioDataRegs.GPACLEAR.bit.GPIO19 = 1)


    //user




    //read
//    #define ADCDATAOK               (GpioDataRegs.GPCDAT.bit.GPIO82)
/*=============== Main Time Base variable ()============================*/

// 1. 函数内定义
//  static ST_FSM_DELAY st_delay = {0, 0, 0};
// 2. 使用：
// delay 并且状态跳转
//        FSM_DELAY_NEXT(st_delay, x_ms, S_STATE);
// 3. 增加delay状态
//    case S_FSM_DELAY:
//        FSM_DELAY_FUNC(st_delay);
//        break;

#define S_FSM_DELAY     999

typedef struct
{
    Uint32 now_time;
    Uint32 delay_ms;
    Uint16 next_state;
}ST_FSM_DELAY;


#define     FSM_DELAY_NEXT(v, delayms, nextstate)           \
do{                                                         \
    v.now_time = task_run_cnt;                              \
    v.delay_ms = delayms;                                   \
    v.next_state = nextstate;                               \
    state = S_FSM_DELAY;                                    \
}while(0)


#define     FSM_DELAY_FUNC(v)                               \
do{                                                         \
    if((task_run_cnt - v.now_time) > Tms_CNT(v.delay_ms))   \
    {                                                       \
        state = v.next_state;                               \
    }                                                       \
}while(0)


/***********************************************************************
* Function Name : Global Variable ()
***********************************************************************/

extern Uint32    task_run_cnt;
extern Uint32    task_run_time;
extern Uint32    max_task_run_time;
/*=====================Fault=======================================*/

extern Uint16 err_flag;
extern Uint32 err_code;
extern Uint16 fpga_state;









extern              void InitUserPara(void);
extern              void EnableWDog(void);
//ServiceDog();
//DisableDog();
/***********************************************************************
* typedef of union and structure
***********************************************************************/


#endif
