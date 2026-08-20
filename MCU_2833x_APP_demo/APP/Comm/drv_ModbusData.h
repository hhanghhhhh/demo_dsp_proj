#ifndef _DRV_MODBUSDATA_H_
#define _DRV_MODBUSDATA_H_

#include "TypeDefine.h"
/**************************************************************************************************
CONSTANTS AND MACROS
**************************************************************************************************/
#define MD_R1_START_ADDR         (0x1000)
#define MD_R1_END_ADDR           (0x101F)
#define MD_R1_SIZE               (MD_R1_END_ADDR - MD_R1_START_ADDR + 1)

#define MD_R2_START_ADDR         (0x1100)
#define MD_R2_END_ADDR           (0x111F)
#define MD_R2_SIZE               (MD_R2_END_ADDR - MD_R2_START_ADDR + 1)

#define MD_W1_START_ADDR         (0x2000)
#define MD_W1_END_ADDR           (0x202F)
#define MD_W1_SIZE               (MD_W1_END_ADDR - MD_W1_START_ADDR + 1)

#define MD_RW1_START_ADDR        (0x3000)
#define MD_RW1_END_ADDR          (0x3019)
#define MD_RW1_SIZE              (MD_RW1_END_ADDR - MD_RW1_START_ADDR + 1)

#define MD_RW2_START_ADDR        (0x5000)
#define MD_RW2_END_ADDR          (0x5080)
#define MD_RW2_SIZE              (MD_RW2_END_ADDR - MD_RW2_START_ADDR + 1)

#define MD_RW3_START_ADDR        (0x7000)
#define MD_RW3_END_ADDR          (0x7010)
#define MD_RW3_SIZE              (MD_RW3_END_ADDR - MD_RW3_START_ADDR + 1)
/**************************************************************************************************
TYPEDEFS
**************************************************************************************************/
#define IN_APP          1
#define IN_BOOT         2
#define JUMP_TO_APP     1
#define JUMP_TO_BOOT    2





typedef struct
{

    //read
    //状态
    Uint16          dataok;

    //常规参数
    float32     isamp;
    float32     vsamp;
    float32     isamp_avg;
    float32     vsamp_avg;

    Uint16          autorange;

    //功能配置参数
    struct
    {
        Uint16          vbr_th;
        Uint16          disable;
        Uint16          ovp_th;
    }cfg;

    //eeprom相关
    Uint16      coef_mode1;
    Uint16      coef_mode2;
    //Ip addr
    struct
    {
        Uint16    addr1;
        Uint16    addr2;
        Uint16    addr3;
        Uint16    addr4;
        Uint16    writeOk;
    }ip;
    //系数的首地址
    struct
    {
        union
        {
            Uint16 u16;
            struct
            {
                Uint16 vadc:1;
                Uint16 vdac:1;
                Uint16 iadc:1;
                Uint16 idac:1;
                Uint16 reserve:12;
            }bit;
        }enable;
//        MODBUS_DATA     f32v_adc_p_k[64];   //预留60个
//        MODBUS_DATA     f32v_adc_p_b[64];
    }coef;

    Uint16    program_type;
    Uint16    jump_cmd;            //  1--boot->app  2--app->boot

    Uint16    reserve;
}ST_SCI_RX_PAR;
extern ST_SCI_RX_PAR    mgmd_stSCIRx;




extern Uint16* md_r1_buf[];
extern Uint16* md_r2_buf[];
extern Uint16* md_rw1_buf[];
extern Uint16* md_rw2_buf[];
extern Uint16* md_rw3_buf[];
extern Uint16* md_w1_buf[];



extern void md_vPrepareData(void);





#endif

/**************************************************************************************************
 END OF FILE (EOF)
**************************************************************************************************/
