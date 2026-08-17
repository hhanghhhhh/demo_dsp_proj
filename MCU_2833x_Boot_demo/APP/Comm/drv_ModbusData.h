#ifndef _DRV_MODBUSDATA_H_
#define _DRV_MODBUSDATA_H_

#include "TypeDefine.h"
/**************************************************************************************************
CONSTANTS AND MACROS
**************************************************************************************************/
#define MD_R1_START_ADDR         (0x1000)
#define MD_R1_END_ADDR           (0x1009)
#define MD_R1_SIZE               (MD_R1_END_ADDR - MD_R1_START_ADDR + 1)

#define MD_R2_START_ADDR         (0x1100)
#define MD_R2_END_ADDR           (0x1109)
#define MD_R2_SIZE               (MD_R2_END_ADDR - MD_R2_START_ADDR + 1)

#define MD_W1_START_ADDR         (0x2000)
#define MD_W1_END_ADDR           (0x2009)
#define MD_W1_SIZE               (MD_W1_END_ADDR - MD_W1_START_ADDR + 1)

#define MD_RW1_START_ADDR        (0x7000)
#define MD_RW1_END_ADDR          (0x7090)
#define MD_RW1_SIZE              (MD_RW1_END_ADDR - MD_RW1_START_ADDR + 1)

#define MD_RW2_START_ADDR        (0x5000)
#define MD_RW2_END_ADDR          (0x5009)
#define MD_RW2_SIZE              (MD_RW2_END_ADDR - MD_RW2_START_ADDR + 1)



#define IN_APP          1
#define IN_BOOT         2
#define FLASH_ERASE     1
#define FLASH_WRITE     2
#define JUMP_TO_APP     1
#define JUMP_TO_BOOT    2

/**************************************************************************************************
TYPEDEFS
**************************************************************************************************/
typedef union
{
    float32     f32;
    struct
    {
        Uint16 L;
        Uint16 H;
    }u16;
}MODBUS_DATA;


typedef struct
{
    Uint16 program_type;        //  1--app   2--boot
    Uint16 jump_cmd;            //  1--boot->app  2--app->boot
    Uint16 flash_cmd;           //  1--erase  2--write
    Uint16 flash_ok;            //  1--ok
    Uint16 crc;
    union
    {
        Uint32 u32;
        struct
        {
            Uint16 L;
            Uint16 H;
        }u16;
    }addr;

    Uint16 data[120];

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
