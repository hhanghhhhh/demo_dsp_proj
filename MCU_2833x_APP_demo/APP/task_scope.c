#include <string.h>
#include "task_scope.h"

#define DSO_INPUT_SAMPLE_PERIOD_NS (10000UL)

typedef enum
{
    DSO_STATE_IDLE = 0,
    DSO_STATE_CAPTURING,
    DSO_STATE_FINISHED
} DSO_State_t;

typedef union
{
    float32 f32;
    struct
    {
        Uint32 LL : 8;
        Uint32 LH : 8;
        Uint32 HL : 8;
        Uint32 HH : 8;
    } byte;
} DSO_FloatBytes_t;

static float32 dso_buffer[DSO_BUF_LEN];
static volatile DSO_State_t dso_state;
static volatile Uint32 dso_write_index;
static volatile Uint32 dso_div_factor;
static volatile Uint32 dso_div_count;
static volatile Uint32 dso_capture_count;

static void DSO_SendUFloat32(Uint8 socket,
                             Uint8 *txbuf,
                             float32 value,
                             Uint16 (*fsendp)(Uint8,
                                              const Uint8 *,
                                              Uint16));
static void DSO_SendChunk(Uint8 socket,
                          Uint8 *txbuf,
                          Uint32 max_tx_byte,
                          float32 *start_addr,
                          Uint32 total_num,
                          Uint16 (*fsendp)(Uint8,
                                           const Uint8 *,
                                           Uint16));
static void DSO_ProcessSend(Uint8 socket,
                            Uint8 *txbuf,
                            Uint32 max_tx_byte,
                            Uint16 (*fsendp)(Uint8,
                                             const Uint8 *,
                                             Uint16));

/* 初始化线性波形记录状态。 */
void DSO_Init(void)
{
    dso_state = DSO_STATE_IDLE;
    dso_write_index = 0UL;
    dso_div_factor = 1UL;
    dso_div_count = 0UL;
    dso_capture_count = 0UL;
}

/* 最新一次启动直接覆盖旧波形。 */
void DSO_CaptureStart(Uint32 div_factor, Uint32 capture_count)
{
    dso_write_index = 0UL;
    dso_div_count = 0UL;
    dso_div_factor = div_factor;
    if (dso_div_factor == 0UL)
    {
        dso_div_factor = 1UL;
    }

    dso_capture_count = capture_count;
    if (dso_capture_count > DSO_BUF_LEN)
    {
        dso_capture_count = DSO_BUF_LEN;
    }

    if (dso_capture_count == 0UL)
    {
        dso_state = DSO_STATE_FINISHED;
    }
    else
    {
        dso_state = DSO_STATE_CAPTURING;
    }
}

/* 按固定整数分频保存采样值，达到目标点数后自动停止。 */
void DSO_CaptureSample(float32 value)
{
    if (dso_state != DSO_STATE_CAPTURING)
    {
        return;
    }

    dso_div_count++;
    if (dso_div_count < dso_div_factor)
    {
        return;
    }
    dso_div_count = 0UL;

    dso_buffer[dso_write_index] = value;
    dso_write_index++;
    if (dso_write_index >= dso_capture_count)
    {
        dso_state = DSO_STATE_FINISHED;
    }
}

/* 以小端字节顺序发送一个Uint32。 */
static void DSO_SendUFloat32(Uint8 socket,
                             Uint8 *txbuf,
                             float32 value,
                             Uint16 (*fsendp)(Uint8,
                                              const Uint8 *,
                                              Uint16))
{
    DSO_FloatBytes_t data;
    data.f32 = value;
    txbuf[0] = (Uint8)data.byte.LL;
    txbuf[1] = (Uint8)data.byte.LH;
    txbuf[2] = (Uint8)data.byte.HL;
    txbuf[3] = (Uint8)data.byte.HH;
    (void)fsendp(socket, txbuf, 4U);
}

/* 将连续float32数据按发送缓冲区容量分包发送。 */
static void DSO_SendChunk(Uint8 socket,
                          Uint8 *txbuf,
                          Uint32 max_tx_byte,
                          float32 *start_addr,
                          Uint32 total_num,
                          Uint16 (*fsendp)(Uint8,
                                           const Uint8 *,
                                           Uint16))
{
    Uint32 sent_num;
    Uint32 remaining;
    Uint32 current_num;
    Uint32 tx_count;
    Uint32 i;
    DSO_FloatBytes_t data;

    if (max_tx_byte < 4UL)
    {
        return;
    }

    sent_num = 0UL;
    while (sent_num < total_num)
    {
        remaining = total_num - sent_num;
        current_num = max_tx_byte >> 2U;
        if (current_num > remaining)
        {
            current_num = remaining;
        }

        tx_count = 0UL;
        for (i = 0UL; i < current_num; i++)
        {
            data.f32 = start_addr[sent_num + i];
            txbuf[tx_count++] = (Uint8)data.byte.LL;
            txbuf[tx_count++] = (Uint8)data.byte.LH;
            txbuf[tx_count++] = (Uint8)data.byte.HL;
            txbuf[tx_count++] = (Uint8)data.byte.HH;
        }

        (void)fsendp(socket, txbuf, (Uint16)tx_count);
        sent_num += current_num;
    }
}

/* 仅在本次波形完成后发送实际有效数据。 */
static void DSO_ProcessSend(Uint8 socket,
                            Uint8 *txbuf,
                            Uint32 max_tx_byte,
                            Uint16 (*fsendp)(Uint8,
                                             const Uint8 *,
                                             Uint16))
{
    if (dso_state != DSO_STATE_FINISHED)
    {
        return;
    }

    DSO_SendChunk(socket,
                  txbuf,
                  max_tx_byte,
                  &dso_buffer[0],
                  dso_write_index,
                  fsendp);
}

/* 上位机查询波形、有效点数或实际存储间隔。 */
void DSO_CmdParse(Uint8 *rxbuf,
                  Uint8 socket,
                  Uint8 *txbuf,
                  Uint32 max_tx_byte,
                  Uint16 (*fsendp)(Uint8, const Uint8 *, Uint16))
{
    if (!memcmp("wave_getcur", rxbuf, 11U))
    {
        DSO_ProcessSend(socket, txbuf, max_tx_byte, fsendp);
    }
    else if (!memcmp("wave_getcount", rxbuf, 13U))
    {
        DSO_SendUFloat32(socket, txbuf, (float32)dso_write_index, fsendp);
    }
    else if (!memcmp("wave_getfreq_ns", rxbuf, 15U))
    {
        DSO_SendUFloat32(socket,
                         txbuf,
                         (float32)DSO_INPUT_SAMPLE_PERIOD_NS * dso_div_factor,
                         fsendp);
    }
}
