////###########################################################################		
// Original Author: S.S.
//
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################
#include "device.h"
#include "socket.h"
#include "w5500.h"
#include "string.h"
#include "drv_Spi.h"
#include "drv_GlobalVar.h"
#include "../Comm/drv_MgModbus.h"
#include "DSP2833x_Device.h"     // Header file Include File
#include "DSP2833x_Examples.h"   // Examples Include File
// Prototype statements for functions found within this file.
// interrupt void ISRTimer2(void);
MODBUS_TCP_CNT    modbus_tcp_cnt[SOCKET_NUM_USE];

Uint16 W5500_recv_buff[W5500_RXTXBUF];       // 网口接收缓存   ，长度与串口modbus的缓存一致
Uint16 W5500_trans_buff[W5500_RXTXBUF];      // 网口发送缓存


extern Uint16 local_port;
extern Uint16  slINT1s;


void Init_modbusTCP_data()
{
    Uint16 i = 0;
    for(i = 0; i < SOCKET_NUM_USE; i++)
    {
        modbus_tcp_cnt[i].recv_all_cnt = 0;
        modbus_tcp_cnt[i].recv_frame_err_cnt = 0;
    }

}

Uint8 hh_test_ip[4];
extern Uint8 local_lip[];

void Init_W5500(void)
{

    Init_modbusTCP_data();
    Reset_W5500();

    set_default();
    set_network();
}

void WIZ_CS(Uint8 val)
{
    if (val == 0)
    {
        W5500_CSN_L;
    }
    else if (val == 1)
    {
        W5500_CSN_H;
    }
}



Uint16 reset_w5500_cnt = 0;

void CheckW5500Status(void)
{
    static Uint32 now_time_cnt = 0;
    static Uint16 first_flag = 0;

    // 赋初值
    if(first_flag == 0)
    {
        first_flag = 1;
        hh_test_ip[0] = local_lip[0];
        hh_test_ip[1] = local_lip[1];
        hh_test_ip[2] = local_lip[2];
        hh_test_ip[3] = local_lip[3];
    }
    //10s检查一次
    if(task_run_cnt - now_time_cnt > Tms_CNT(10000))
    {
        now_time_cnt = task_run_cnt;
        getSIPR(hh_test_ip);

        if(hh_test_ip[0] == local_lip[0] &&
           hh_test_ip[1] == local_lip[1] &&
           hh_test_ip[2] == local_lip[2] &&
           hh_test_ip[3] == local_lip[3])
        {
            return;
        }

        DisableDog();
        Init_W5500();
        EnableWDog();
        reset_w5500_cnt++;
    }
}

Uint8 SPI2_SendByte(Uint8 byte)
{
    Uint8 SPIRXD;
    while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG == 1);
    SpiaRegs.SPITXBUF=(byte<<8);
    while(SpiaRegs.SPISTS.bit.INT_FLAG != 1);{}//Wait until data is received
    SPIRXD = SpiaRegs.SPIRXBUF;
    return SPIRXD;

}


void Reset_W5500(void)//reset
{
    W5500_RESET_L;
    DELAY_US(200000);
    W5500_RESET_H;
    DELAY_US(500000);  //1.6s
}

//判断是否符合modbusTCP协议
//符合条件返回1，否则返回0
Uint16 modbusTCP_judge(Uint8* buf, Uint16 len)
{
    Uint16 tmp_len = 0;
    //addr
    if(buf[6] != mgmd_u16SlaveID)
    {
        return 0;
    }

    //fun code
    if((buf[7] != 0x03) && (buf[7] != 0x06) && (buf[7] != 0x10))
    {
        return 0;
    }

    //lenth
    tmp_len = ((buf[4] & 0xFF) << 8) | (buf[5] & 0xFF);
    if(buf[7] == 0x10)
    {
        if(len < (tmp_len + 6))
        {
            return 0;
        }
    }
    else
    {
        if(len < 12)
        {
            return 0;
        }
    }

    return 1;
}


/* 跳转到 APP 前关闭所有已使用的 W5500 socket。 */
void Close_All_Socket(void)
{
    Uint16 i = 0;

    for (i = 0; i < SOCKET_NUM_USE; i++)
    {
        close_socket(i);
    }
}

Uint16 sock_tcps_state[SOCKET_NUM_USE], w5500_linkstate = 0;  //test，看网口状态用
//W5500作为server端运行，一直处于listen状态，等待client的connect请求
void do_tcp_server(SOCKET s, Uint16 index)
{
    Uint16 len=0;
    static Uint16 est_first_flag = 0;
    static Uint32 est_last_time = 0;

    sock_tcps_state[index] = getSn_SR(s);

    w5500_linkstate = IINCHIP_READ(PHYCFGR);

    switch(getSn_SR(s))                                                         /*获取socket的状态*/
    {
        case SOCK_CLOSED:                                                                     /*socket处于关闭状态*/
            est_first_flag = 0;
            //不同socket可以设置相同的端口号
            socket(s, Sn_MR_TCP, local_port, Sn_MR_ND);           /*打开socket*/
          break;

        case SOCK_INIT:                                                                       /*socket已初始化状态*/
            listen(s);                                                              /*socket建立监听*/
          break;

        case SOCK_ESTABLISHED:           /*socket处于连接建立状态*/
            //刚建立连接时更新一下时间，防止直接断开。
            if(est_first_flag == 0)
            {
                est_first_flag = 1;
                est_last_time = task_run_cnt;
            }

            if(getSn_IR(s) & Sn_IR_CON)
            {
                setSn_IR(s, Sn_IR_CON);                                       /*清除接收中断标志位*/
            }
            len=getSn_RX_RSR(s);                                                /*定义len为已接收数据的长度*/
            if(len>0)
            {
                modbus_tcp_cnt[index].recv_all_cnt++;

                recv(s,(Uint8*)W5500_recv_buff, len);       /*接收来自Client的数据*/

                //判断数据是否符合modbusTCP协议
                mgmd_u16RxReady = modbusTCP_judge((Uint8*)W5500_recv_buff, len);
                mgmd_u16Urt1BytesReceived = len;
            }

            //接收数据并回复
            if(mgmd_u16RxReady == 1)
            {
                mgmd_u16RxReady = 0;
                ModbusTCPFunc(W5500_recv_buff, W5500_trans_buff, s, send);
                modbus_tcp_cnt[index].send_frame_cnt++;
                est_last_time = task_run_cnt;   //获取最后一次发送数据的时间
            }

          //超过10s没接收到数据，断开连接
          if((task_run_cnt - est_last_time) > Tms_CNT(20000))
          {
              modbus_tcp_cnt[index].overtime_est_cnt++;
              close_socket(s);
          }

          break;

        case SOCK_CLOSE_WAIT:           /*socket处于等待关闭状态*/
            close_socket(s);
          break;
    }
}




//===========================================================================
// No more.
//===========================================================================

