#include "socket.h"
#include "w5500.h"
#include "Version.h"

//数据过长会导致数组越界，不接收到本地，仅把读取指针后移
//only clear RD pointer
static void recvfrom_upd_clearRD(SOCKET s, Uint16 len)
{
   Uint16 ptr=0;

   ptr     = IINCHIP_READ(Sn_RX_RD0(s) );
   ptr     = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD1(s));

   ptr += len;

   IINCHIP_WRITE( Sn_RX_RD0(s), (Uint8)((ptr & 0xff00) >> 8));
   IINCHIP_WRITE( Sn_RX_RD1(s), (Uint8)(ptr & 0x00ff));

   IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_RECV);

   /* wait to process the command... */
   while( IINCHIP_READ( Sn_CR(s)) ) ;
   /* ------- */
}


static void NumToString(Uint16 buf[], Uint16 num)
{
    Uint16 tmp;
    //仅处理三位数
    if(num < 1000)
    {
        // 获取百位数
        tmp = num / 100;
        buf[0] = (tmp == 0) ? ' ' : tmp + '0';

        // 获取十位数
        num -= tmp * 100;
        tmp = num / 10;
        buf[1] = (tmp == 0) ? ' ' : tmp + '0';

        // 获取个位数
        num -= tmp * 10;
        buf[2] = num + '0';
    }
}

#define UDP_SEND_LEN    72
//                                           10        20         30        40         50         60        70
//                                  012345678901234567890 123456789012345678901 234567890123456 7890123456789012
Uint8 udp_send_msg[UDP_SEND_LEN] = " UniSiCSUM_          \n IP: xxx.xxx.xxx.xxx\n DSP_ver : xxx\n FPGA_ver: xxx";  //结尾\0
extern Uint8 hh_test_ip[];
static void UdpMsgFraming(void)
{
    Uint16* buf = (Uint16*)udp_send_msg;
    Uint16 i = 0;
    // 将设备编号转化为字符串
    const char* device_str;
    switch(st_version.device_num)
    {
    case DEVICE_HIGHCURRENT2_0:     device_str = "HC2.0"; break;
    case DEVICE_HIGHVOL1_3:         device_str = "HV1.3"; break;
    case DEVICE_HIGHVOL2_0:         device_str = "HV2.0"; break;
    case DEVICE_LOWVOL:             device_str = "LV"; break;
    case DEVICE_LV_BOOT:            device_str = "BOOT_LV"; break;
    case DEVICE_HC_BOOT:            device_str = "BOOT_HC"; break;
    default:                        device_str = "xx"; break;
    }

    // 设备号写入发送缓冲区
    i = 0;
    while(device_str[i] != '\0' && i < 10)
    {
        buf[11+i] = device_str[i];
        i++;
    }

    // 写入设备的IP地址
    for(i = 0; i < 4; i++)
    {
        NumToString(&buf[27 + 4*i], hh_test_ip[i]);
    }

    // 写入DSP和FPGA版本信息
    NumToString(&buf[54], st_version.DSPversion);
    NumToString(&buf[69], st_version.FPGAversion);
}

Uint16 hh_test_udpstate = 0;
void do_udp(SOCKET s)
{
    Uint16 broadcast_ip[4] = {255,255,255,255};
    Uint16 len = 0;
    Uint16 buf[10];
    Uint16 udp_remote_ip[4];
    Uint16 udp_remote_prot = 0;

    hh_test_udpstate = getSn_SR(s);
    switch(hh_test_udpstate)
    {
    case SOCK_CLOSED:
        //初始化socket，端口号1234
        socket(s, Sn_MR_UDP, 1234, 0);
        break;

    case SOCK_UDP:
        //清空接收中断
        if(getSn_IR(s) & Sn_IR_RECV)
        {
            setSn_IR(s, Sn_IR_RECV);
        }

        len=getSn_RX_RSR(s);

        //upd帧头 IP+prot+len 共8字节，接收10字节数据，总长18字节
        if(len > 0 && len <= 18)  // 数据包大小合法
        {
            recvfrom(s, (Uint8*)buf, len, (Uint8*)udp_remote_ip, &udp_remote_prot);

            // 检查是否为 "*IDN?" 命令
            if(buf[0] == '*' && buf[1] == 'I' && buf[2] == 'D' && buf[3] == 'N' && buf[4] == '?')
            {
                UdpMsgFraming();

                //回复广播报文
                sendto(s, udp_send_msg, UDP_SEND_LEN, (Uint8*)broadcast_ip, udp_remote_prot);
            }
        }
        else if(len > 18) //数据过长，清除缓存
        {
            recvfrom_upd_clearRD(s, len);
        }
        break;
    }
}



//===========================================================================
// No more.
//===========================================================================

