#include <string.h>
#include "drv_ModbusData.h"
#include "Version.h"
#include "app_eeprom_config.h"
#include "drv_fpga.h"

Uint16* md_r1_buf[MD_R1_SIZE];
Uint16* md_r2_buf[MD_R2_SIZE];
Uint16* md_rw1_buf[MD_RW1_SIZE];
Uint16* md_rw2_buf[MD_RW2_SIZE];
Uint16* md_rw3_buf[MD_RW3_SIZE];
Uint16* md_w1_buf[MD_W1_SIZE];

#define F32_WORD(x, n) (&(((Uint16 *)&(x))[n]))
#define F32_H(x) F32_WORD(x, 1)
#define F32_L(x) F32_WORD(x, 0)

void InitModbusData()
{
    ST_SCI_RX_PAR*  p = &mgmd_stSCIRx;

    memset(&mgmd_stSCIRx, 0, sizeof(mgmd_stSCIRx));

    p->program_type = IN_APP;


//    p->nplc.f32 = 1;
//    p->v_set.f32 = 20;
//    p->i_set.f32 = 1e-3;
//    p->autorange = 1;
//    p->cfg.enablerange2_500 = 1;



    //配置初始化在读取eeprom时
//    p->cfg.vbr_th = 20;
//    p->cfg.ovp_th = 200;
//    p->cfg.fire_th = 100;
//    p->cfg.fire_drop_ratio_100 = 50;   //0.5
//    p->cfg.fire_drop_value = 100;
//    p->cfg.scp_th = 20;
//    p->cfg.scp_time_ms = 200;
//    p->coef.enable.u16 = 0xFFFF;   //enable all coefficient
}


void md_vPrepareData(void)
{
    Uint16 i = 0;

   //0x1000
   //info
   md_r1_buf[0x00] = &mgmd_stSCIRx.dataok;
   md_r1_buf[0x01] = &mgmd_stSCIRx.reserve;
   md_r1_buf[0x02] = &mgmd_stSCIRx.reserve;
   md_r1_buf[0x03] = &mgmd_stSCIRx.reserve;
   md_r1_buf[0x04] = &mgmd_stSCIRx.reserve;
   md_r1_buf[0x05] = &mgmd_stSCIRx.reserve;
   md_r1_buf[0x06] = &mgmd_stSCIRx.reserve;
   md_r1_buf[0x07] = &mgmd_stSCIRx.reserve;
   md_r1_buf[0x08] = &mgmd_stSCIRx.reserve;
   md_r1_buf[0x09] = &mgmd_stSCIRx.reserve;
   md_r1_buf[0x0A] = &mgmd_stSCIRx.reserve;
   md_r1_buf[0x0B] = &mgmd_stSCIRx.reserve;
   md_r1_buf[0x0C] = &mgmd_stSCIRx.reserve;
   // ...
   md_r1_buf[0x10] = &st_version.FPGAversion;
   md_r1_buf[0x11] = &st_version.DSPversion;
   md_r1_buf[0x12] = &st_version.dsp_bulid_time.u16.H;
   md_r1_buf[0x13] = &st_version.dsp_bulid_time.u16.L;
   md_r1_buf[0x14] = &st_version.initok;
   md_r1_buf[0x15] = &st_version.device_num;

   //0x1100
   //data
    md_r2_buf[0x00] = F32_H(mgmd_stSCIRx.isamp);
    md_r2_buf[0x01] = F32_L(mgmd_stSCIRx.isamp);
   md_r2_buf[0x02] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x03] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x04] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x05] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x06] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x07] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x08] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x09] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x0A] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x0B] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x0C] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x0D] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x0E] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x0F] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x10] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x11] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x12] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x13] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x14] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x15] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x16] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x17] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x18] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x19] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x1A] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x1B] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x1C] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x1D] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x1E] = &mgmd_stSCIRx.reserve;
   md_r2_buf[0x1F] = &mgmd_stSCIRx.reserve;


   //0x2000
   md_w1_buf[0x00] = &mgmd_stSCIRx.ip.addr1;
   md_w1_buf[0x01] = &mgmd_stSCIRx.ip.addr2;
   md_w1_buf[0x02] = &mgmd_stSCIRx.ip.addr3;
   md_w1_buf[0x03] = &mgmd_stSCIRx.ip.addr4;
   md_w1_buf[0x04] = &mgmd_stSCIRx.coef_mode1;
   md_w1_buf[0x05] = &mgmd_stSCIRx.coef_mode2;
   md_w1_buf[0x06] = &mgmd_stSCIRx.coef.enable.u16;
   md_w1_buf[0x07] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x08] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x09] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x0A] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x0B] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x0C] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x0D] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x0E] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x0F] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x10] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x11] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x12] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x13] = &mgmd_stSCIRx.reserve;
   //...
   md_w1_buf[0x1E] = &mgmd_stSCIRx.reserve;
   md_w1_buf[0x1F] = &mgmd_stSCIRx.reserve;

   //0x3000
   //parameter
   md_rw1_buf[0x00] = &mgmd_stSCIRx.reserve;    // v_set.u16.H
   md_rw1_buf[0x01] = &mgmd_stSCIRx.reserve;    // v_set.u16.L
   md_rw1_buf[0x02] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x03] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x04] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x05] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x06] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x07] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x08] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x09] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x0A] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x0B] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x0C] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x0D] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x0E] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x0F] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x10] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x11] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x12] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x13] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x14] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x15] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x16] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x17] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x18] = &mgmd_stSCIRx.reserve;
   md_rw1_buf[0x19] = &mgmd_stSCIRx.reserve;

   //0x5000
   //calibration coefficient
//   for(i = 0; i < APP_EEPROM_COEF_NUM; i++)
//   {
//       md_rw2_buf[0x00 + 4*i] = &mgmd_stSCIRx.coef.f32v_adc_p_k[i].u16.H;
//       md_rw2_buf[0x01 + 4*i] = &mgmd_stSCIRx.coef.f32v_adc_p_k[i].u16.L;
//       md_rw2_buf[0x02 + 4*i] = &mgmd_stSCIRx.coef.f32v_adc_p_b[i].u16.H;
//       md_rw2_buf[0x03 + 4*i] = &mgmd_stSCIRx.coef.f32v_adc_p_b[i].u16.L;
//   }

   // 0x7000
   md_rw3_buf[0x00] = &mgmd_stSCIRx.program_type;
   md_rw3_buf[0x01] = &mgmd_stSCIRx.jump_cmd;
}



/**************************************************************************************************
 END OF FILE (EOF)
**************************************************************************************************/
