#ifndef _CONFIG_H_
#define _CONFIG_H_


#include "device.h"
#include "TypeDefine.h"
#define __GNUC__


/***************************************************
 * attribute for mcu ( types, ... )
 ***************************************************/

#define MAX_SOCK_NUM        8   /**< Maxmium number of socket  */

/**
@brief   __DEF_IINCHIP_MAP_xxx__ : define memory map for iinchip
*/
#define __DEF_IINCHIP_MAP_BASE__ 0x0000
 #define COMMON_BASE 0x0000
#define __DEF_IINCHIP_MAP_TXBUF__ (COMMON_BASE + 0x8000) /* Internal Tx buffer address of the iinchip */
#define __DEF_IINCHIP_MAP_RXBUF__ (COMMON_BASE + 0xC000) /* Internal Rx buffer address of the iinchip */
//#define __DEF_IINCHIP_PPP

#define IINCHIP_ISR_DISABLE()
#define IINCHIP_ISR_ENABLE()

#ifndef NULL
#define NULL        ((void *) 0)
#endif


#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif


typedef volatile char vint8;
typedef volatile unsigned char vuint8;


typedef Uint8           SOCKET;



#define HIGH									1
#define LOW										0
	
#define CONFIG_MSG_LEN        sizeof(CONFIG_MSG) - 4 // the 4 bytes OP will not save to EEPROM
#define MAX_BUF_SIZE					1460
#define KEEP_ALIVE_TIME	      30	// 30sec
// SRAM address range is 0x2000 0000 ~ 0x2000 BFFF (48KB)
#define SOCK_BUF_ADDR 				0x20000000
#define AppBackAddress        0x08020000 //from 128K
#define ConfigAddr						0x0800FC00
#define NORMAL_STATE          0
#define NEW_APP_IN_BACK       1 //there is new app in back address
#define CONFIGTOOL_FW_UP      2 //configtool update f/w in app


typedef struct _CONFIG_MSG
{
  Uint8 op[4];//header: FIND;SETT;FACT...
  Uint8 mac[6];
  Uint8 sw_ver[2];
  Uint8 lip[4];
  Uint8 sub[4];
  Uint8 gw[4];
  Uint8 dns[4];	
  Uint8 dhcp;
  Uint8 debug;

  Uint16 fw_len;
  Uint8 state;
  
}CONFIG_MSG;

extern CONFIG_MSG  ConfigMsg, RecvMsg;

#endif


