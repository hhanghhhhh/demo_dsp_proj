#include <stdint.h>
#include <string.h> 
#include "task_scope.h" 


/* ========================================================================== */
/*                                 全局变量                                   */
/* ========================================================================== */

/* 静态分配波形存储区 (Float32) */
static float32 DSO_Buffer[DSO_CH_NUM][DSO_BUF_LEN];

/* 通道控制实例 */
static DSO_Channel_t DSO_Channels[DSO_CH_NUM];

/* ========================================================================== */
/*                                 函数实现                                   */
/* ========================================================================== */

Uint32 debug_sizeof_buf = 0;
/**
 * @brief 初始化示波器模块
 */
void DSO_Init(void)
{
    Uint32 i;
    for (i = 0; i < DSO_CH_NUM; i++)
    {
        memset(DSO_Buffer[i], 0, sizeof(DSO_Buffer)/DSO_CH_NUM);
        DSO_Channels[i].id = i;
        DSO_Channels[i].state = DSO_STATE_IDLE;
        DSO_Channels[i].write_idx = 0;
        DSO_Channels[i].div_cnt = 0;
        DSO_Channels[i].source_addr = NULL;
        /* 默认配置 */
        DSO_Channels[i].div_factor = 1;
        DSO_Channels[i].post_trig_len = DSO_BUF_LEN / 2; 
    }
}

/**
 * @brief 配置指定通道
 * @param ch_id 通道索引
 * @param src_addr 变量地址
 * @param trig_cmd 触发条件，上升沿触发
 * @param div_factor 分频系数
 * @param post_len 触发后保留长度 (y)
 */
void DSO_ConfigChannel(Uint32 ch_id, float32* src_addr, Uint32 div_factor, Uint32 post_len)
{
    if (ch_id >= DSO_CH_NUM) return;
    
    DSO_Channel_t* ch = &DSO_Channels[ch_id];
    
    ch->source_addr = src_addr;
    ch->div_factor = (div_factor == 0) ? 1 : div_factor; // 防止除0
    
    /* 限制 post_len 不超过缓冲区大小 */
    if (post_len > DSO_BUF_LEN) post_len = DSO_BUF_LEN;
    ch->post_trig_len = post_len;
    
    ch->state = DSO_STATE_IDLE;
}

/**
 * @brief 启动采样 (上位机下发开始指令)
 * @param ch_id 通道ID
 */
void DSO_Start(Uint32 ch_id)
{
    DSO_Channel_t* ch = &DSO_Channels[ch_id];

    if (ch_id >= DSO_CH_NUM) return;
    if (ch->state != DSO_STATE_IDLE) return;

    ch->div_cnt = 0;
    ch->write_idx = 0;
    
    ch->state = DSO_STATE_WAIT_TRIG;
}

/**
 * @brief 强制触发，保存当前时刻的波形，触发位置0，len = buf 大小
 */
void DSO_ForceTrigger(Uint32 ch_id)
{
    if (ch_id >= DSO_CH_NUM) return;
    DSO_Channel_t* ch = &DSO_Channels[ch_id];

    ch->write_idx = 0;
    ch->post_cnt = DSO_BUF_LEN;
    ch->state = DSO_STATE_POST_TRIG;
}

/**
 * @brief 程序触发，包含触发条件前后的波形，触发位置根据 post_trig_len 确定
 */
void DSO_SelfTrigger(Uint32 ch_id)
{
    if (ch_id >= DSO_CH_NUM) return;
    DSO_Channel_t* ch = &DSO_Channels[ch_id];

    /* 只有在等待触发状态下才有效 */
    if (ch->state == DSO_STATE_WAIT_TRIG) {
        ch->post_cnt = ch->post_trig_len;
        ch->state = DSO_STATE_POST_TRIG;
    }
}


/**
 * @brief 核心采样函数 (需在中断中调用)
 *        支持分频、环形存储、触发检测
 */
void DSO_Sample(void)
{
    Uint32 i;
    float32 current_val;

    for (i = 0; i < DSO_CH_NUM; i++)
    {
        DSO_Channel_t* ch = &DSO_Channels[i];

        /* 如果未启动或已完成，跳过 */
        if (ch->state == DSO_STATE_IDLE || ch->state == DSO_STATE_FINISHED || ch->state == DSO_STATE_SENDING) {
            continue;
        }

        /* 1. 分频处理 */
        ch->div_cnt++;
        if (ch->div_cnt < ch->div_factor) {
            continue; /* 未达到分频点，退出 */
        }
        ch->div_cnt = 0; /* 重置分频计数 */

        /* 2. 获取当前值并存入环形缓冲区 */
        if (ch->source_addr != NULL) {
            current_val = *(ch->source_addr);
        } else {
            continue; /* 未配置，退出 */
        }

        DSO_Buffer[i][ch->write_idx] = current_val;

        /* 3. 更新写指针 (环形逻辑) */
//        Uint32 prev_idx = ch->write_idx; /* 保存当前位置用于触发标记(可选) */
        ch->write_idx++;
        if (ch->write_idx >= DSO_BUF_LEN) {
            ch->write_idx = 0; /* 回环 */
        }

        /* 4. 状态机逻辑 */
        if (ch->state == DSO_STATE_WAIT_TRIG)
        {
            /* 判断触发条件 (此处以大于阈值为例) */
            /* 实际应用中可能需要加入迟滞或边沿判断 */
//            if (current_val > ch->trig_threshold)
//            {
//                ch->post_cnt = ch->post_trig_len;
//                ch->state = DSO_STATE_POST_TRIG;
//            }
        }
        else if (ch->state == DSO_STATE_POST_TRIG)
        {
            /* 触发后倒计数 y 个点 */
            if (ch->post_cnt > 0) {
                ch->post_cnt--;
            }
            
            if (ch->post_cnt == 0) {
                ch->state = DSO_STATE_FINISHED; /* 采样结束，锁定数据 */
            }
        }
    }
}

/**
 * @brief 内部静态函数：发送一段连续内存
 * @param socket 句柄
 * @param start_addr 起始地址
 * @param total_bytes 总字节数
 */
static void DSO_SendChunk(Uint8 socket, Uint8* tx_buf, Uint32 max_tx_byte, float32* start_addr, Uint32 total_num, Uint16 (*fsendp)(Uint8, const Uint8*, Uint16))
{
    Uint32 sent_num = 0;
    Uint32 curr_tx_size = 0;
    CONV_F32 tmp_cnv;
    Uint32 tx_cnt = 0;
    Uint32 i = 0;

    while (sent_num < total_num)
    {
        /* 计算本次发送大小，不超过tx_buf，1 个 float 填充 4 个 tx_buf */
        Uint32 remaining = (total_num - sent_num);
        if (remaining > (max_tx_byte >> 2)) {
            curr_tx_size = (max_tx_byte >> 2);
        } else {
            curr_tx_size = remaining;
        }

        tx_cnt = 0;
        // 调整字节序，c2000 没有 Uint8
        for(i = 0; i < curr_tx_size; i++)
        {
            tmp_cnv.f32 = *(start_addr + sent_num + i);
            tx_buf[tx_cnt++] = tmp_cnv.u8.LL;
            tx_buf[tx_cnt++] = tmp_cnv.u8.LH;
            tx_buf[tx_cnt++] = tmp_cnv.u8.HL;
            tx_buf[tx_cnt++] = tmp_cnv.u8.HH;
        }

        /* 调用底层发送函数 */
        fsendp(socket, tx_buf, tx_cnt);

        sent_num += curr_tx_size;
    }
}

/**
 * @brief 处理数据发送 (需在主循环或低优先级任务中调用)
 *        零拷贝逻辑：将环形数据分为两段发送，无需 memcpy
 * @param socket_record 通信句柄
 * @param ch_id 要回读的通道
 */
void DSO_ProcessSend(Uint8 socket_record, Uint8* txbuf, Uint32 max_tx_byte, Uint32 ch_id, Uint16 (*fsendp)(Uint8, const Uint8*, Uint16))
{
    if (ch_id >= DSO_CH_NUM) return;
    DSO_Channel_t* ch = &DSO_Channels[ch_id];

    /* 只有在 FINISHED 状态下才发送 */
    if (ch->state != DSO_STATE_FINISHED) {
        return;
    }

//    ch->state = DSO_STATE_SENDING; /* 标记为发送中 */

    /*
       发送顺序：
       Step 1: 发送 [WriteIdx] 到 [BUF_LEN-1] (历史最老的数据)
       Step 2: 发送 [0] 到 [WriteIdx-1] (最新的数据)
    */

    Uint32 idx_cut = ch->write_idx;
    float32* base_addr = &DSO_Buffer[ch_id][0]; /* 转为字节指针方便计算 */

    /* --- Part 1: 从当前写指针到缓冲区末尾 --- */
    /* 地址 = Base + Index * 4 */
    /* 长度 = (TotalLen - Index) * 4 */
    float32* part1_addr = base_addr + idx_cut;
    Uint32 part1_len = DSO_BUF_LEN - idx_cut;

    if (part1_len > 0) {
        DSO_SendChunk(socket_record, txbuf, max_tx_byte, part1_addr, part1_len, fsendp);
    }

    /* --- Part 2: 从缓冲区头部到写指针前一个 --- */
    /* 地址 = Base */
    /* 长度 = Index * 4 */
    float32* part2_addr = base_addr;
    Uint32 part2_len = idx_cut;

    if (part2_len > 0) {
        DSO_SendChunk(socket_record, txbuf, max_tx_byte, part2_addr, part2_len, fsendp);
    }

    /* 发送完成，回到 IDLE 状态，等待下次 Start */
//    ch->state = DSO_STATE_IDLE;
}


// 上位机查询数据的时候发送过去
void DSO_CmdParse(Uint8* rxbuf, Uint8 socket, Uint8* txbuf, Uint32 max_tx_byte, Uint16 (*fsendp)(Uint8, const Uint8*, Uint16))
{
//    CONV_F32 tmp_data;
//    Uint16 send_cnt = 0;

    if(!memcmp("wave_getcur", rxbuf, 11))
    {
        DSO_ProcessSend(socket, txbuf, max_tx_byte, CH0_XXDATA, fsendp);
    }
    else if(!memcmp("wave_getvol", rxbuf, 11))
    {

    }
    else if(!memcmp("wave_trigger", rxbuf, 12))
    {
        // 保存当前时刻的波形，触发位置0，len = buf 大小
        DSO_ForceTrigger(CH0_XXDATA);
    }
    else if(!memcmp("wave_getfreq_ns", rxbuf, 15))
    {
//        tmp_data.f32 = st_wave.to_fpga.bit.intervl_cnt * 20; // /50*1000  (50Mclk)
//        txbuf[send_cnt++] = tmp_data.u8.LL;
//        txbuf[send_cnt++] = tmp_data.u8.LH;
//        txbuf[send_cnt++] = tmp_data.u8.HL;
//        txbuf[send_cnt++] = tmp_data.u8.HH;
//        fsendp(socket, txbuf, send_cnt);
    }
}
