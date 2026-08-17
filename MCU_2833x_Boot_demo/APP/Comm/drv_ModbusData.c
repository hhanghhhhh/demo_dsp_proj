#include <string.h>
#include "drv_ModbusData.h"
#include "Version.h"

Uint16* md_r1_buf[MD_R1_SIZE];
Uint16* md_r2_buf[MD_R2_SIZE];
Uint16* md_rw1_buf[MD_RW1_SIZE];
Uint16* md_rw2_buf[MD_RW2_SIZE];
Uint16* md_w1_buf[MD_W1_SIZE];


void InitModbusData()
{
    ST_SCI_RX_PAR*  p = &mgmd_stSCIRx;

    memset(&mgmd_stSCIRx, 0, sizeof(mgmd_stSCIRx));

    p->program_type = IN_BOOT;
}


void md_vPrepareData(void)
{
    Uint16 i = 0;

    for(i = 0; i < 10; i++)
    {
        md_r1_buf[i] = &mgmd_stSCIRx.reserve;
        md_r2_buf[i] = &mgmd_stSCIRx.reserve;
        md_w1_buf[i] = &mgmd_stSCIRx.reserve;
        md_rw2_buf[i] = &mgmd_stSCIRx.reserve;
    }

   // 0x7000
   md_rw1_buf[0x00] = &mgmd_stSCIRx.program_type;   //  1--app   2--boot
   md_rw1_buf[0x01] = &mgmd_stSCIRx.jump_cmd;       //  1--boot->app  2--app->boot
   md_rw1_buf[0x02] = &mgmd_stSCIRx.flash_cmd;      //  1--erase  2--write
   md_rw1_buf[0x03] = &mgmd_stSCIRx.flash_ok;       //  1--ok
   md_rw1_buf[0x04] = &mgmd_stSCIRx.crc;
   md_rw1_buf[0x05] = &mgmd_stSCIRx.reserve;
   //...

   md_rw1_buf[0x10] = &mgmd_stSCIRx.addr.u16.H;
   md_rw1_buf[0x11] = &mgmd_stSCIRx.addr.u16.L;
   for(i = 0; i < 120; i++)
   {
       md_rw1_buf[0x12 + i] = &mgmd_stSCIRx.data[i];
   }



}



/**************************************************************************************************
 END OF FILE (EOF)
**************************************************************************************************/
