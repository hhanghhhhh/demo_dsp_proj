/*********************************************************************************************
 * scib 驱动模块
 * 使用接收 fifo 中断以及主循环接收两种方式共同接收
 * 接收 fifo 中断只在 fifo 中有固定数量时触发，用于快速接收 fifo 中值，防止溢出
 * 在主循环中用超时方法查询 fifo 中是否有剩余值并将其接收
 *
 * 发送使用 fifo 发送，在 fifo 未满时将值写入，fifo 满时等待
 *
 *********************************************************************************************/
#include <drv_scib.h>
#include <string.h>
#include "DSP2833x_Device.h"   // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h" // DSP2833x Examples Include File
#include "drv_GlobalVar.h"

ST_RINGBUF st_scib_rx_ringbuf;
void Init_ScibData(void)
{
    memset(&st_scib_rx_ringbuf, 0, sizeof(ST_RINGBUF));
    st_scib_rx_ringbuf.head = st_scib_rx_ringbuf.buf;
    st_scib_rx_ringbuf.tail = st_scib_rx_ringbuf.buf;
}

#define RXFIFO_INT_NUM 15
void Init_Scib(void)
{
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;   // Enable pull-up for GPIO22 (SCITXDB)
    GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0;   // Enable pull-up for GPIO23 (SCIRXDB)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 3; // Asynch input GPIO23 (SCIRXDB)
    GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 1;  // SCI Tx
    GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 1;  // SCI Rx
    EDIS;
    ScibRegs.SCIFFTX.all = 0x8000;
    ScibRegs.SCICCR.all = 0x0007;  // 1 stop bit,  No loopback, No parity,8 char bits;   async mode, idle-line protocol
    ScibRegs.SCICTL1.all = 0x0003; // enable TX, RX, internal SCICLK;  Disable RX ERR, SLEEP, TXWAKE
    ScibRegs.SCICTL2.all = 0x0003;
    // BRR = LSPCLK / (SCI Baud * 8) - 1   = 37.5M/921,600 - 1
    // BRR = (SCIHBAUD << 8) + (SCILBAUD)
    ScibRegs.SCIHBAUD = 0x0000; // 9600 baud @LSPCLK = 37.5MHz. = 0x01E7   115200 = 0x0028
    ScibRegs.SCILBAUD = 0x0028;
    ScibRegs.SCICTL1.all = 0x0023; // Relinquish SCI from Reset
    // fifo
    ScibRegs.SCIFFCT.all = 0x0;
    ScibRegs.SCIFFTX.all = 0xE040;
    ScibRegs.SCIFFRX.all = 0x2060 + RXFIFO_INT_NUM; // enable rxfifo interrupt, rxfifo interrupt num = 15byte
    // init data
    Init_ScibData();
}

// 发送一个字节
void Scib_Xmit(char a)
{
    while (SCIB_TX_FIFO == 16)
    {
    }
    SCIB_SEED_DATA_BUF = a;
}

// 发送len个字节
void ScibSendMsg(char *msg, Uint16 len)
{
    int i;
    // 串口发送
    for (i = 0; i < len; i++)
    {
        Scib_Xmit(msg[i]);
    }
}

// scib接收中断 INT9.3
interrupt void ScibRxFifoIsr(void)
{
    Uint16 i;
    ST_RINGBUF *pbuf = &st_scib_rx_ringbuf;
    for (i = 0; i < RXFIFO_INT_NUM; i++)
    {
        *pbuf->tail++ = SCIB_RECEIVE_DATA_BUF;
        if (pbuf->tail == pbuf->buf + RINGBUF_SIZE)
            pbuf->tail = pbuf->buf;
    }

    ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1;     // Clear Overflow flag
    ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1;     // Clear Interrupt flag
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP9; // Issue PIE ack---scib rx INT9.3
}

// 中断向量表配置
void ScibRxIntPieCfg(void)
{
    DINT;
    EALLOW;
    PieVectTable.SCIRXINTB = &ScibRxFifoIsr;
    EDIS;
    PieCtrlRegs.PIEIER9.bit.INTx3 = 1; // scib_rx
    IER |= M_INT9;                     // sicb_rx
}

// 主循环中运行
// 超时将fifo剩余数据取出，超时时间为2倍接收15字节的时间（其中15字节是fifo中断的值）
void ScibRecvMsgMain(void)
{
    ST_RINGBUF *pbuf = &st_scib_rx_ringbuf;
    static Uint16 last_fifo_num = 0;
    static Uint32 last_time_cnt = 0;

    if (SCIB_RX_FIFO != 0)
    {
        if (SCIB_RX_FIFO != last_fifo_num)
        {
            // 更新last记录
            last_fifo_num = SCIB_RX_FIFO;
            last_time_cnt = task_run_cnt;
            return;
        }

        if (task_run_cnt - last_time_cnt > Tms_CNT(3))
        {
            while (SCIB_RX_FIFO != 0)
            {
                *pbuf->tail++ = SCIB_RECEIVE_DATA_BUF;
                if (pbuf->tail == pbuf->buf + RINGBUF_SIZE)
                    pbuf->tail = pbuf->buf;
            }

            // 更新last记录
            last_fifo_num = 0;
            last_time_cnt = task_run_cnt;
        }
    }
}

Uint16 RingBufSize(void)
{
    ST_RINGBUF *pbuf = &st_scib_rx_ringbuf;
    return ((pbuf->head + RINGBUF_SIZE - pbuf->tail) % RINGBUF_SIZE);
}

/******************************************************************************
 * == END OF MODULE/FILE (EOF) ==
 ******************************************************************************/
