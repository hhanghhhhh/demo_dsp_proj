#ifndef __APP_EEPROM_CONFIG_H_
#define __APP_EEPROM_CONFIG_H_

#include "TypeDefine.h"

#define EEROM_DATAOK             (6666)
#define EEROM_DATAERR_OVERRANG   (1004)

#define APP_EEPROM_CONFIG_START   0x0100
#define APP_EEPROM_CONFIG_VERSION 1
#define APP_EEPROM_HEADER_MAGIC   0x55AA
#define APP_EEPROM_FOOTER_MAGIC   0xAA55

/*
 * raw 是强制占位区，user 是真实业务数据区。
 * 这里采用 union，让业务侧直接访问 user，EEPROM 侧按 raw 整块搬运。
 * 本工程 C28x 中 Uint16 是一个可寻址 word，写入 EEPROM 时会拆成两个 byte。
 */
#define APP_EEPROM_RAW_WORDS      512
#define APP_EEPROM_WORD_PAGE      64
#define APP_EEPROM_COEF_NUM       28

#define APP_EEPROM_DATAOK         EEROM_DATAOK
#define APP_EEPROM_ERR_HEADER     0x1001
#define APP_EEPROM_ERR_FOOTER     0x1002
#define APP_EEPROM_ERR_VERSION    0x1003
#define APP_EEPROM_ERR_RAW_WORDS  0x1004
#define APP_EEPROM_ERR_CHECKSUM   0x1005

typedef struct
{
    Uint16 vbr_th;
    Uint16 prot_disable;
    Uint16 prot_ovp_th;
    Uint16 prot_fire_th;
    Uint16 prot_fire_drop_ratio_100;
    Uint16 prot_fire_drop_value;
    Uint16 prot_scp_th;
    Uint16 prot_scp_time_ms;
    Uint16 idss_overtime;
    Uint16 vbr_overtime;
    Uint16 vbr_shorttime;
    Uint16 enablerange2_500;
    Uint16 reserve[36];
} APP_EEPROM_PROTECT_CONFIG;

typedef struct
{
    Uint16 coef_enable;
    APP_EEPROM_PROTECT_CONFIG config;
    float32 coef_k[APP_EEPROM_COEF_NUM];
    float32 coef_b[APP_EEPROM_COEF_NUM];
} APP_EEPROM_USER_DATA;

/* 统一 EEPROM 配置块：头、版本、业务 payload、尾、校验和。 */
typedef struct
{
    Uint16 header;
    Uint16 version;
    Uint16 raw_words;
    union
    {
        Uint16 raw[APP_EEPROM_RAW_WORDS];
        APP_EEPROM_USER_DATA user;
    } payload;
    Uint16 footer;
    Uint16 checksum;
} APP_EEPROM_CONFIG;

#define APP_EEPROM_CONFIG_WORDS       sizeof(APP_EEPROM_CONFIG)
#define APP_EEPROM_CONFIG_CHECK_WORDS (sizeof(APP_EEPROM_CONFIG) - 1)

extern APP_EEPROM_CONFIG g_app_eeprom_config;
extern Uint16 g_app_eeprom_status;

/* 按 Uint16 word 读写 EEPROM；内部会把每个 word 拆成/合成两个 EEPROM byte。 */
Uint16 AppEeprom_ReadWords(Uint16 start, Uint16 *buf, Uint16 word_len);
Uint16 AppEeprom_WriteWords(Uint16 start, Uint16 *buf, Uint16 word_len);

/* 配置块生命周期接口：默认值、初始化读取、保存、校验。 */
void AppEepromConfig_LoadDefault(void);
Uint16 AppEepromConfig_Init(void);
Uint16 AppEepromConfig_Save(void);
Uint16 AppEepromConfig_Check(APP_EEPROM_CONFIG *config);

#endif
