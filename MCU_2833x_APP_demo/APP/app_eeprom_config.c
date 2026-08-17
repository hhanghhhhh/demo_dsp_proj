#include "app_eeprom_config.h"
#include "drv_Eeprom.h"
#include "DSP2833x_Examples.h"
#include <string.h>

#define I2C_SUCCESS 0x0000

APP_EEPROM_CONFIG g_app_eeprom_config;
Uint16 g_app_eeprom_status = APP_EEPROM_DATAOK;
static Uint16 s_app_eeprom_byte_buf[APP_EEPROM_WORD_PAGE * 2];

/* 将一个 C28x Uint16 word 拆成两个 EEPROM byte，低字节在前。 */
static void AppEeprom_WordToBytes(Uint16 word, Uint16 *bytes)
{
    bytes[0] = word & 0x00FF;
    bytes[1] = (word >> 8) & 0x00FF;
}

/* 将两个 EEPROM byte 合成为一个 C28x Uint16 word，低字节在前。 */
static Uint16 AppEeprom_BytesToWord(Uint16 *bytes)
{
    return (bytes[0] & 0x00FF) | ((bytes[1] & 0x00FF) << 8);
}

/* 简单累加校验，从结构体开头算到 checksum 字段之前。 */
static Uint16 AppEepromConfig_CalcChecksum(APP_EEPROM_CONFIG *config)
{
    Uint16 i = 0;
    Uint16 checksum = 0;
    Uint16 *word = (Uint16 *)config;

    for (i = 0; i < APP_EEPROM_CONFIG_CHECK_WORDS; i++)
    {
        checksum += word[i];
    }

    return checksum;
}

/* 写入前刷新头、版本、长度、尾和校验和。 */
static void AppEepromConfig_Prepare(APP_EEPROM_CONFIG *config)
{
    config->header = APP_EEPROM_HEADER_MAGIC;
    config->version = APP_EEPROM_CONFIG_VERSION;
    config->raw_words = APP_EEPROM_RAW_WORDS;
    config->footer = APP_EEPROM_FOOTER_MAGIC;
    config->checksum = AppEepromConfig_CalcChecksum(config);
}

/* 按 word 写 EEPROM。每次最多写 64 个 word，即 128 个 EEPROM byte。 */
Uint16 AppEeprom_WriteWords(Uint16 start, Uint16 *buf, Uint16 word_len)
{
    Uint16 rst = I2C_SUCCESS;
    Uint16 i = 0;
    Uint16 write_words = 0;

    while (word_len > 0)
    {
        write_words = (word_len > APP_EEPROM_WORD_PAGE) ? APP_EEPROM_WORD_PAGE : word_len;

        for (i = 0; i < write_words; i++)
        {
            AppEeprom_WordToBytes(buf[i], &s_app_eeprom_byte_buf[i * 2]);
        }

        rst = WRE2ROM(s_app_eeprom_byte_buf, start, write_words * 2);
        if (rst != I2C_SUCCESS)
        {
            return rst;
        }

        start += write_words * 2;
        buf += write_words;
        word_len -= write_words;

        if (word_len > 0)
        {
            DELAY_US(10000);
        }
    }

    return I2C_SUCCESS;
}

/* 按 word 读 EEPROM。每次最多读 64 个 word，即 128 个 EEPROM byte。 */
Uint16 AppEeprom_ReadWords(Uint16 start, Uint16 *buf, Uint16 word_len)
{
    Uint16 rst = I2C_SUCCESS;
    Uint16 i = 0;
    Uint16 read_words = 0;

    while (word_len > 0)
    {
        read_words = (word_len > APP_EEPROM_WORD_PAGE) ? APP_EEPROM_WORD_PAGE : word_len;

        rst = RDE2ROM(s_app_eeprom_byte_buf, start, read_words * 2);
        if (rst != I2C_SUCCESS)
        {
            return rst;
        }

        for (i = 0; i < read_words; i++)
        {
            buf[i] = AppEeprom_BytesToWord(&s_app_eeprom_byte_buf[i * 2]);
        }

        start += read_words * 2;
        buf += read_words;
        word_len -= read_words;

        if (word_len > 0)
        {
            DELAY_US(10000);
        }
    }

    return I2C_SUCCESS;
}

/* 加载出厂默认配置；业务侧只需要给 payload.user 赋值。 */
void AppEepromConfig_LoadDefault(void)
{
    Uint16 i = 0;
    APP_EEPROM_USER_DATA *user = &g_app_eeprom_config.payload.user;

    memset(&g_app_eeprom_config, 0, sizeof(APP_EEPROM_CONFIG));
    user->config.vbr_th = 2;
    user->config.prot_disable = 0;
    user->config.prot_ovp_th = 200;
    user->config.prot_fire_th = 100;
    user->config.prot_fire_drop_ratio_100 = 50;
    user->config.prot_fire_drop_value = 100;
    user->config.prot_scp_th = 20;
    user->config.prot_scp_time_ms = 200;
    user->config.idss_overtime = 100;
    user->config.vbr_overtime = 1000;
    user->config.vbr_shorttime = 200;
    user->config.enablerange2_500 = 1;

    for (i = 0; i < APP_EEPROM_COEF_NUM; i++)
    {
        user->coef_k[i] = 1;
        user->coef_b[i] = 0;
    }
    AppEepromConfig_Prepare(&g_app_eeprom_config);
}

/* 校验 EEPROM 配置块是否合法。 */
Uint16 AppEepromConfig_Check(APP_EEPROM_CONFIG *config)
{
    if (config->header != APP_EEPROM_HEADER_MAGIC)
    {
        return APP_EEPROM_ERR_HEADER;
    }
    if (config->footer != APP_EEPROM_FOOTER_MAGIC)
    {
        return APP_EEPROM_ERR_FOOTER;
    }
    if (config->version != APP_EEPROM_CONFIG_VERSION)
    {
        return APP_EEPROM_ERR_VERSION;
    }
    if (config->raw_words != APP_EEPROM_RAW_WORDS)
    {
        return APP_EEPROM_ERR_RAW_WORDS;
    }
    if (AppEepromConfig_CalcChecksum(config) != config->checksum)
    {
        return APP_EEPROM_ERR_CHECKSUM;
    }
    return APP_EEPROM_DATAOK;
}

/* 上电初始化：整块读取 EEPROM，失败或校验错误则加载默认值。 */
Uint16 AppEepromConfig_Init(void)
{
    Uint16 rst = 0;

    memset(&g_app_eeprom_config, 0, sizeof(APP_EEPROM_CONFIG));

    rst = AppEeprom_ReadWords(APP_EEPROM_CONFIG_START,
                              (Uint16 *)&g_app_eeprom_config,
                              APP_EEPROM_CONFIG_WORDS);
    if (rst != I2C_SUCCESS)
    {
        AppEepromConfig_LoadDefault();
        g_app_eeprom_status = rst;
        return rst;
    }

    rst = AppEepromConfig_Check(&g_app_eeprom_config);
    if (rst != APP_EEPROM_DATAOK)
    {
        AppEepromConfig_LoadDefault();
        g_app_eeprom_status = rst;
        return rst;
    }

    g_app_eeprom_status = APP_EEPROM_DATAOK;
    return APP_EEPROM_DATAOK;
}

/* 保存当前配置：刷新校验信息后整块写入 EEPROM。 */
Uint16 AppEepromConfig_Save(void)
{
    Uint16 rst = 0;

    AppEepromConfig_Prepare(&g_app_eeprom_config);

    rst = AppEeprom_WriteWords(APP_EEPROM_CONFIG_START,
                               (Uint16 *)&g_app_eeprom_config,
                               APP_EEPROM_CONFIG_WORDS);
    g_app_eeprom_status = (rst == I2C_SUCCESS) ? APP_EEPROM_DATAOK : rst;
    return rst;
}
