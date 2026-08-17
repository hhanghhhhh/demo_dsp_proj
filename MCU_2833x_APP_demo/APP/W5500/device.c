
#include "device.h"
#include "config.h"
#include "socket.h"
#include "w5500.h"
#include <stdio.h> 
#include <string.h>
#include "app_boot_eeprom.h"
CONFIG_MSG  ConfigMsg, RecvMsg;

Uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};		// 选择8个Socket每个Socket发送缓存的大小，在w5500.c的void sysinit()有设置过程
Uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};		// 选择8个Socket每个Socket发送缓存的大小，在w5500.c的void sysinit()有设置过程




Uint8 local_mac[6]={0x00,0x08,0xdc,0x11,0x11,0x80};
Uint8 local_lip[4]={192,168,1,80};
Uint8 local_sub[4]={255,255,255,0};
Uint8 local_gw[4]={192,168,1,1};
Uint8 local_dns[4]={8,8,8,8};

Uint16 local_port = 502;

void set_network(void)			// 配置初始化IP信息并打印，初始化8个Socket
{

  setSHAR(ConfigMsg.mac);
  setSUBR(ConfigMsg.sub);
  setGAR(ConfigMsg.gw);
  setSIPR(ConfigMsg.lip);

  sysinit(txsize, rxsize); 											// 初始化8个socket
  setRTR(2000);														// 设置超时时间
  setRCR(3);														// 设置最大重新发送次数
}

void set_default(void)									//设置默认MAC、IP、GW、SUB、DNS
{  
    memcpy(local_lip, g_app_boot_eeprom_param.ip, 4);
    memcpy(local_mac, local_lip, 4);

    memcpy(ConfigMsg.lip, local_lip, 4);
    memcpy(ConfigMsg.sub, local_sub, 4);
    memcpy(ConfigMsg.gw,  local_gw, 4);
    memcpy(ConfigMsg.mac, local_mac, 6);
    memcpy(ConfigMsg.dns, local_dns, 4);


    ConfigMsg.dhcp=0;
    ConfigMsg.debug=1;
    ConfigMsg.fw_len=0;

    ConfigMsg.state=NORMAL_STATE;
    ConfigMsg.sw_ver[0]=FW_VER_HIGH;
    ConfigMsg.sw_ver[1]=FW_VER_LOW;
}

void write_config_to_eeprom(void)
{
/*  Uint8 data;
  Uint16 i,j;
  Uint16 dAddr=0;
  for (i = 0, j = 0; i < (Uint8)(sizeof(ConfigMsg)-4);i++) 
  {
    data = *(Uint8 *)(ConfigMsg.mac+j);
    at24c16_write(dAddr, data);
    dAddr += 1;
    j +=1;
  }*/
}
void get_config(void)
{
/*  Uint8 tmp=0;
  Uint16 i;
  for (i =0; i < CONFIG_MSG_LEN; i++) 
  {
    tmp=at24c16_read(i);
    *(ConfigMsg.mac+i) = tmp;
  }
  if((ConfigMsg.mac[0]==0xff)&&(ConfigMsg.mac[1]==0xff)&&(ConfigMsg.mac[2]==0xff)&&(ConfigMsg.mac[3]==0xff)&&(ConfigMsg.mac[4]==0xff)&&(ConfigMsg.mac[5]==0xff))
  {
    set_default();
  }*/
}

