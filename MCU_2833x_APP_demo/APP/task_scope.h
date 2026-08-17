#ifndef __TASK_SCOPE_H_
#define __TASK_SCOPE_H_

#include "TypeDefine.h"

/* ========================================================================== */
/*                                 宏定义                                */
/* ========================================================================== */

/* 示波器配置宏 */
#define DSO_CH_NUM          (2)      /* 通道数量 */
#define DSO_BUF_LEN         (512)    /* 每个通道的缓冲区深度 */

#define CH0_XXDATA          (0)
#define CH1_XXDATA          (1)
#define CH2_XXDATA          (2)
#define CH3_XXDATA          (3)

/* 触发模式枚举 */
typedef enum {
    DSO_TRIG_MODE_AUTO = 0,     /* 自动/立即触发 */
    DSO_TRIG_MODE_NORMAL,       /* 边沿或电平触发 */
    DSO_TRIG_MODE_SOFTWARE      /* 仅等待上位机指令 */
} DSO_TrigMode_t;

/* 触发条件枚举 */
typedef enum {
    DSO_COND_RISING = 0,        /* 上升沿 (简化演示，暂按大于阈值处理) */
    DSO_COND_FALLING            /* 下降沿 */
} DSO_TrigCond_t;

/* 运行状态机 */
typedef enum {
    DSO_STATE_IDLE = 0,         /* 空闲 */
    DSO_STATE_WAIT_TRIG,        /* 等待触发 (正在记录 Pre-trigger 数据) */
    DSO_STATE_POST_TRIG,        /* 触发后采样 (正在记录 Post-trigger 数据) */
    DSO_STATE_FINISHED,         /* 采样完成，等待发送 */
    DSO_STATE_SENDING           /* 正在发送 (防止重入) */
} DSO_StateEnum_t;

/* 通道控制结构体 */
typedef struct {
    /* --- 配置参数 --- */
    float32*        source_addr;    /* 观测变量的地址 */
    Uint32          div_factor;     /* 分频系数 (1 = 每次都采, 10 = 每10次采1次) */
    float32         trig_threshold; /* 触发阈值 */
    Uint32          trig_cmd;       /* 触发指令 */
    DSO_TrigMode_t  trig_mode;      /* 触发模式 */
    Uint32          post_trig_len;  /* 触发后需要保存的点数 (y值) */

    /* --- 运行时变量 --- */
    DSO_StateEnum_t state;          /* 当前状态 */
    Uint32          write_idx;      /* 当前写指针 (0 ~ DSO_BUF_LEN-1) */
    Uint32          div_cnt;        /* 分频计数器 */
    Uint32          post_cnt;       /* 触发后剩余计数器 */
    Uint32          id;             /* 通道ID */
} DSO_Channel_t;

typedef union
{
    float32      f32;
    struct
    {
        Uint32 LL:8;
        Uint32 LH:8;
        Uint32 HL:8;
        Uint32 HH:8;
    }u8;
}CONV_F32;





void DSO_Init(void);
void DSO_ConfigChannel(Uint32 ch_id, float32* src_addr, Uint32 div_factor, Uint32 post_len);
void DSO_Start(Uint32 ch_id);
void DSO_ForceTrigger(Uint32 ch_id);
void DSO_SelfTrigger(Uint32 ch_id);
void DSO_Sample(void);
void DSO_CmdParse(Uint8* rxbuf, Uint8 socket, Uint8* txbuf, Uint32 max_tx_byte, Uint16 (*fsendp)(Uint8, const Uint8*, Uint16));





#endif
