#ifndef _DRV_MGMODBUS_H_
#define _DRV_MGMODBUS_H_

#include "TypeDefine.h"


#define MD_DATA_OFFSET    (13)


#define READ_MULT    (0x03)
#define WRITE_SINGLE (0x06)
#define WRITE_MULT   (0x10)



extern Uint16 mgmd_u16SlaveID;
extern Uint16 mgmd_u16RxReady;
extern Uint32 modbusTCP_rcv_cnt;


extern void mgmd_vInit(void);
extern void ModbusTCPFunc(Uint16* prxdata, Uint16* ptxdata, Uint16 socket, Uint16 (*fsendp)(Uint8, const Uint8*, Uint16));





#endif

/**************************************************************************************************
 END OF FILE (EOF)
**************************************************************************************************/
