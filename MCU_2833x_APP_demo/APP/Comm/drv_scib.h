#ifndef DRV_SCI_H_
#define DRV_SCI_H_

#include "TypeDefine.h"

#define       SCIB_RX_FIFO            (ScibRegs.SCIFFRX.bit.RXFFST)
#define       SCIB_TX_FIFO            (ScibRegs.SCIFFTX.bit.TXFFST)
#define       SCIB_RECEIVE_DATA_BUF   (ScibRegs.SCIRXBUF.bit.RXDT)
#define       SCIB_SEED_DATA_BUF      (ScibRegs.SCITXBUF)




#define RINGBUF_SIZE    2048
typedef struct
{
    char buf[RINGBUF_SIZE];
    char* head;
    char* tail;
}ST_RINGBUF;





extern ST_RINGBUF st_scib_rx_ringbuf;   //接收环形缓冲区，当 head != tail 时代表有新的数据


extern void Init_Scib(void);    //初始化scib以及变量
extern void Scib_Xmit(char a);
extern void ScibSendMsg(char* msg, Uint16 len);  //scib发送函数
extern void ScibRecvMsgMain(void);  //scib接收函数，主循环运行
extern void ScibRxIntPieCfg(void);  //scib中断配置，在初始化中断向量表时调用



/******************************************************************************
* == END OF MODULE/FILE (EOF) ==
******************************************************************************/
#endif
